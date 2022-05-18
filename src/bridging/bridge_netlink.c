#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <sysrepo.h>

#include "bridging/common.h"
#include "bridge_netlink.h"

int bridge_get_vlan_info(struct nl_sock *socket, struct rtnl_link *bridge_link, bridge_vlan_info_t *vlan_info)
{
	struct nl_msg *msg = NULL;
	unsigned char *msg_buf = NULL;
	int error = 0;

	if (vlan_info == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "vlan_info is NULL", error);
		return -1;
	}

	int if_index = rtnl_link_get_ifindex(bridge_link);
	const char *name = rtnl_link_get_name(bridge_link);

	// send RTM_GETLINK message for the bridge
	error = rtnl_link_build_get_request(if_index, name, &msg);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_build_get_request() failed (%d)", error);
		goto out;
	}
	error = nl_send_auto(socket, msg);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_send_auto() failed (%d)", error);
		goto out;
	}

	// wait for kernel response and ack
	struct sockaddr_nl nla = {0};
	int len = nl_recv(socket, &nla, &msg_buf, NULL);
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