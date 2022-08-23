#include <stdbool.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_bridge.h>
#include <linux/if_arp.h>

#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <netlink/route/link/bridge.h>
#include <sysrepo.h>

#include "bridging/common.h"
#include "bridge_netlink.h"
#include "utils/memory.h"

int bridge_get_vlan_info(struct nl_sock *socket, struct rtnl_link *bridge_link, bridge_vlan_info_t *vlan_info)
{
	struct nl_msg *msg = NULL;
	unsigned char *msg_buf = NULL;
	int error = 0;
	int len = 0;

	if (vlan_info == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "vlan_info is NULL");
		goto out;
	}

	int if_index = rtnl_link_get_ifindex(bridge_link);
	const char *name = rtnl_link_get_name(bridge_link);

	// send RTM_GETLINK message for the bridge
	error = rtnl_link_build_get_request(if_index, name, &msg);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_build_get_request() failed (%d)", error);
		goto out;
	}
	len = nl_send_auto(socket, msg);
	if (len < 0) {
		error = len;
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_send_auto() failed (%d)", error);
		goto out;
	}

	// wait for kernel response and ack
	struct sockaddr_nl nla = {0};
	len = nl_recv(socket, &nla, &msg_buf, NULL);
	if (len <= 0) {
		error = len;
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_recv() failed (%d)", error);
		goto out;
	}
	error = nl_wait_for_ack(socket);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_wait_for_ack() failed (%d)", error);
		goto out;
	}

	// validate message type
	struct nlmsghdr *hdr = (struct nlmsghdr *) msg_buf;
	if (hdr->nlmsg_type != RTM_NEWLINK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unexpected message type in received response (%d)", hdr->nlmsg_type);
		goto out;
	}
	// RTM_NEWLINK messages contain an ifinfomsg structure
	// followed by a series of rtattr structures, see `man 7 rtnetlink`.
	int proto_header_len = sizeof(struct ifinfomsg);

	// Find IFLA_BR_VLAN_PROTOCOL and IFLA_BR_VLAN_FILTERING attributes,
	// which are nested in IFLA_LINKINFO->IFLA_INFO_DATA.
	struct nlattr *ifla_linkinfo = nlmsg_find_attr(hdr, proto_header_len, IFLA_LINKINFO);
	if (ifla_linkinfo == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "IFLA_LINKINFO attribute not found.");
		goto out;
	}
	struct nlattr *ifla_info_data = nla_find(nla_data(ifla_linkinfo), nla_len(ifla_linkinfo), IFLA_INFO_DATA);
	if (ifla_info_data == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "IFLA_INFO_DATA attribute not found.");
		goto out;
	}
	struct nlattr *br_vlan_filtering = nla_find(nla_data(ifla_info_data), nla_len(ifla_info_data), IFLA_BR_VLAN_FILTERING);
	if (br_vlan_filtering == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "IFLA_BR_VLAN_FILTERING attribute not found.");
		goto out;
	}
	struct nlattr *br_vlan_proto = nla_find(nla_data(ifla_info_data), nla_len(ifla_info_data), IFLA_BR_VLAN_PROTOCOL);
	if (br_vlan_proto == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "IFLA_BR_VLAN_PROTOCOL attribute not found.");
		goto out;
	}

	// fill vlan_info
	vlan_info->vlan_filtering = nla_get_u8(br_vlan_filtering);
	vlan_info->vlan_proto = nla_get_u16(br_vlan_proto);

	SRPLG_LOG_DBG(PLUGIN_NAME, "vlan_info for %s:", rtnl_link_get_name(bridge_link));
	SRPLG_LOG_DBG(PLUGIN_NAME, "  vlan_info->vlan_filtering == %d", vlan_info->vlan_filtering);
	if (vlan_info->vlan_proto == htons(ETH_P_8021Q)) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "  vlan_info->vlan_proto == htons(ETH_P_8021Q)");
	} else if (vlan_info->vlan_proto == htons(ETH_P_8021AD)) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "  vlan_info->vlan_proto == htons(ETH_P_8021AD)");
	}

out:
	if (msg != NULL) {
		nlmsg_free(msg);
	}
	if (msg_buf != NULL) {
		free(msg_buf);
	}

	return error;
}

int send_nl_request_and_error_check(struct nl_sock *socket, struct nl_msg *msg)
{
	unsigned char *msg_buf = NULL;
	int error = 0;

	int len = nl_send_sync(socket, msg);
	if (len < 0) {
		error = len;
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_send_auto() failed (%d)", error);
		goto out;
	}
out:
	if (msg_buf) {
		free(msg_buf);
	}
	return error;
}

int bridge_set_vlan_config(struct nl_sock *socket, int bridge_link_idx, bridge_vlan_info_t *vlan_info)
{
	struct nl_msg *msg = NULL;
	struct nlattr *link_info = NULL;
	struct nlattr *info_data = NULL;
	int error = 0;

	if (vlan_info == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "vlan_info is NULL");
		goto out;
	}
	msg = nlmsg_alloc_simple(RTM_NEWLINK, NLM_F_REQUEST | NLM_F_ACK);
	if (msg == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_alloc() failed");
		goto out;
	}
	// fill RTM_NEWLINK message header
	struct ifinfomsg ifinfo = {
		.ifi_family = AF_UNSPEC,
		.ifi_type   = ARPHRD_NETROM,
		.ifi_index  = bridge_link_idx,
		.ifi_flags  = 0,
		.ifi_change = 0
	};
	error = nlmsg_append(msg, &ifinfo, sizeof(ifinfo), nlmsg_padlen(sizeof(ifinfo)));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_append() failed (%d)", error);
		goto out;
	}
	// open nested attributes IFLA_LINKINFO->IFLA_INFO_DATA
	link_info = nla_nest_start(msg, IFLA_LINKINFO);
	if (link_info == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_nest_start() failed");
		error = -1;
		goto out;
	}
	error = nla_put_string(msg, IFLA_INFO_KIND, "bridge");
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_put_string() failed");
		goto out;
	}
	info_data = nla_nest_start(msg, IFLA_INFO_DATA);
	if (info_data == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_nest_start() failed");
		error = -1;
		goto out;
	}
	// add bridge vlan attributes
	error = nla_put_u8(msg, IFLA_BR_VLAN_FILTERING, vlan_info->vlan_filtering);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_put_u8() failed");
		goto out;
	}
	if (vlan_info->vlan_proto) {
		error = nla_put_u16(msg, IFLA_BR_VLAN_PROTOCOL, vlan_info->vlan_proto);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "nla_put_u16() failed");
			goto out;
		}
	}
	// close nested attributes
	error = nla_nest_end(msg, info_data);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_nest_end() failed");
		goto out;
	}
	error = nla_nest_end(msg, link_info);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_nest_end() failed");
		goto out;
	}
	error = send_nl_request_and_error_check(socket, msg);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "send_nl_request_and_error_check() failed");
	}
out:
	//if (msg != NULL) {
	//	nlmsg_free(msg);
	//}
	return error;
}


int bridge_set_ageing_time(struct nl_sock *socket, int bridge_link_idx, unsigned ageing_time)
{
	struct nl_msg *msg = NULL;
	struct nlattr *link_info = NULL;
	struct nlattr *info_data = NULL;
	int error = 0;

	msg = nlmsg_alloc_simple(RTM_NEWLINK, NLM_F_REQUEST | NLM_F_ACK);
	if (msg == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_alloc() failed");
		goto out;
	}
	// fill RTM_NEWLINK message header
	struct ifinfomsg ifinfo = {
		.ifi_family = AF_UNSPEC,
		.ifi_type   = ARPHRD_NETROM,
		.ifi_index  = bridge_link_idx,
		.ifi_flags  = 0,
		.ifi_change = 0
	};
	error = nlmsg_append(msg, &ifinfo, sizeof(ifinfo), nlmsg_padlen(sizeof(ifinfo)));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_append() failed (%d)", error);
		goto out;
	}
	// open nested attributes IFLA_LINKINFO->IFLA_INFO_DATA
	link_info = nla_nest_start(msg, IFLA_LINKINFO);
	if (link_info == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_nest_start() failed");
		error = -1;
		goto out;
	}
	error = nla_put_string(msg, IFLA_INFO_KIND, "bridge");
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_put_string() failed");
		goto out;
	}
	info_data = nla_nest_start(msg, IFLA_INFO_DATA);
	if (info_data == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_nest_start() failed");
		error = -1;
		goto out;
	}
	// add bridge vlan attributes
	error = nla_put_u32(msg, IFLA_BR_AGEING_TIME, ageing_time);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_put_u8() failed");
		goto out;
	}

	// close nested attributes
	error = nla_nest_end(msg, info_data);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_nest_end() failed");
		goto out;
	}
	error = nla_nest_end(msg, link_info);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_nest_end() failed");
		goto out;
	}
	error = send_nl_request_and_error_check(socket, msg);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "send_nl_request_and_error_check() failed");
	}


out:
	return error;
}

int bridge_vlan_msg_open(struct nl_msg **msg, struct rtnl_link *link, bool delete)
{
	struct nlattr *afspec = NULL;
	int error = 0;

	uint8_t request_type = RTM_SETLINK; // create or update vlans on this link
	if (delete) {
		request_type = RTM_DELLINK; // delete vlans
	}
	*msg = nlmsg_alloc_simple(request_type, NLM_F_REQUEST | NLM_F_ACK);
	if (*msg == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_alloc() failed");
		goto out;
	}
	// fill message header
	struct ifinfomsg ifinfo = {
		.ifi_family = AF_BRIDGE,
		.ifi_type   = ARPHRD_NETROM,
		.ifi_index  = rtnl_link_get_ifindex(link),
		.ifi_flags  = 0,
		.ifi_change = 0
	};
	error = nlmsg_append(*msg, &ifinfo, sizeof(ifinfo), nlmsg_padlen(sizeof(ifinfo)));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_append() failed (%d)", error);
		goto out;
	}

	uint16_t cmd_flags = 0;
	if (rtnl_link_is_bridge(link)) {
		cmd_flags = BRIDGE_FLAGS_SELF;
	} else {
		cmd_flags = BRIDGE_FLAGS_MASTER; // send command to bridge master
	}
	error = nla_put_u16(*msg, IFLA_BRIDGE_FLAGS, cmd_flags);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_put_u16() failed");
		goto out;
	}

	afspec = nla_nest_start(*msg, IFLA_AF_SPEC);
	if (afspec == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_nest_start() failed");
		error = -1;
		goto out;
	}
out:
	return error;
}

int bridge_vlan_msg_add_vlan(struct nl_msg *msg, struct bridge_vlan_info vinfo)
{
	int error = nla_put(msg, IFLA_BRIDGE_VLAN_INFO, sizeof(vinfo), &vinfo);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_put() failed");
	}
	return error;
}

int bridge_vlan_msg_finalize_and_send(struct nl_sock *socket, struct nl_msg *msg)
{
	int error = 0;
	struct nlattr *afspec = nlmsg_find_attr(nlmsg_hdr(msg), sizeof(struct ifinfomsg), IFLA_AF_SPEC);
	if (afspec == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_find_attr() failed, can't find IFLA_AF_SPEC attribute");
		error = -1;
		goto out;
	}
	error = nla_nest_end(msg, afspec);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nla_nest_end() failed");
		goto out;
	}
	error = send_nl_request_and_error_check(socket, msg);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "send_nl_request_and_error_check() failed");
	}
out:
	return error;
}


int bridge_get_vlan_list(struct nl_sock *socket, struct rtnl_link *link, struct bridge_vlan_info **list_dest, size_t *count)
{
	int error = 0;
	int len = 0;
	struct nl_msg *msg = NULL;
	unsigned char *msg_buf = NULL;
	struct sockaddr_nl nla = {0};
	struct nlmsghdr *hdr = NULL;
	struct nlattr *current_vlan_entry = NULL;
	struct nlattr *entry_info = NULL;
	int remaining = 0;
	size_t list_size = 16; /* initial size */
	int proto_header_len = 0;
	int if_index = rtnl_link_get_ifindex(link);
	struct bridge_vlan_info *port_vlan_list = NULL;

	msg = nlmsg_alloc_simple(RTM_GETVLAN, NLM_F_REQUEST | NLM_F_DUMP);
	if (msg == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nlmsg_alloc() failed");
		goto error_out;
	}
	// fill RTM_GETVLAN message header
	struct br_vlan_msg vlan_msg = {
		.family = AF_BRIDGE,
		.ifindex = (unsigned) if_index,
	};
	error = nlmsg_append(msg, &vlan_msg, sizeof(vlan_msg), nlmsg_padlen(sizeof(vlan_msg)));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_append() failed (%d)", error);
		goto error_out;
	}
	len = nl_send_auto(socket, msg);
	if (len < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_send_auto() failed (%d)", len);
		goto error_out;
	}

	// wait for kernel response
	len = nl_recv(socket, &nla, &msg_buf, NULL);
	if (len <= 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_recv() failed (%d)", len);
		goto error_out;
	}
	// validate message type
	hdr = (struct nlmsghdr *) msg_buf;
	if (hdr->nlmsg_type != RTM_NEWVLAN) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unexpected message type in received response (%d)", hdr->nlmsg_type);
		goto error_out;
	}

	// find first vlan entry
	proto_header_len = sizeof(struct br_vlan_msg);
	current_vlan_entry = nlmsg_find_attr(hdr, proto_header_len, BRIDGE_VLANDB_ENTRY);
	if (current_vlan_entry == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "BRIDGE_VLANDB_ENTRY attribute not found.");
		goto error_out;
	}
	remaining = nlmsg_attrlen(hdr, sizeof(struct br_vlan_msg));
	*count = 0;
	port_vlan_list = xcalloc(list_size, sizeof(struct bridge_vlan_info));
	while (nla_ok(current_vlan_entry, remaining)) {
		if (nla_type(current_vlan_entry) != BRIDGE_VLANDB_ENTRY) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "unexpected attribute type for current_vlan_entry: %d", nla_type(current_vlan_entry));
			goto error_out;
		}
		entry_info = nla_data(current_vlan_entry);
		if (nla_type(entry_info) != BRIDGE_VLANDB_ENTRY_INFO) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "unexpected attribute type for entry_info: %d", nla_type(entry_info));
			goto error_out;
		}
		if (*count == list_size) {
			// double list size
			port_vlan_list = xrealloc(port_vlan_list, sizeof(struct bridge_vlan_info) * list_size * 2);
		}
		// append bridge_vlan_info struct to list (copy data from entry_info attribute)
		len = nla_memcpy(&port_vlan_list[*count], entry_info, sizeof(struct bridge_vlan_info));
		if (len != sizeof(struct bridge_vlan_info)) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "nla_memcpy error - unexpected number of bytes copied: %u", len);
			goto error_out;
		}
		(*count)++;
		current_vlan_entry = nla_next(current_vlan_entry, &remaining);
	}
	*list_dest = port_vlan_list;

	goto out;

error_out:
	error = -1;
out:
	if (msg != NULL) {
		nlmsg_free(msg);
	}
	if (msg_buf != NULL) {
		FREE_SAFE(msg_buf);
	}

	return error;
}