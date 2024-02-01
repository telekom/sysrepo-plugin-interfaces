/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "load.h"
#include "plugin/common.h"
#include "plugin/data/interfaces/interface.h"
#include "sysrepo.h"

#include <linux/if.h>
#include <net/if_arp.h>
#include <srpc.h>

int interfaces_interface_load_name(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link)
{
    int error = 0;

    const char* nl_if_name = NULL;

    nl_if_name = rtnl_link_get_name(link);

    // set element values
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_name(element, nl_if_name), error_out);

    goto out;

error_out:
    error = -1;

out:
    return error;
}

int interfaces_interface_load_type(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link)
{
    int error = 0;

    const char* nl_if_type = NULL;
    const char* ly_if_type = NULL;

    // interface type - nl version -> convert to libyang version
    nl_if_type = rtnl_link_get_type(link);

    SRPC_SAFE_CALL_ERR(error, interfaces_interface_type_nl2ly(nl_if_type, &ly_if_type), error_out);
    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_type(element, ly_if_type), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

int interfaces_interface_load_enabled(interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link)
{
    int error = 0;

    uint8_t nl_enabled = (rtnl_link_get_operstate(link) == IF_OPER_UP || rtnl_link_get_operstate(link) == IF_OPER_UNKNOWN);

    SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_enabled(element, nl_enabled), error_out);

    goto out;

error_out:
    error = -1;

out:

    return error;
}

int interfaces_interface_load_parent_interface(interfaces_nl_ctx_t* nl_ctx, interfaces_ctx_t* ctx, interfaces_interface_hash_element_t** element, struct rtnl_link* link)
{
    char tmp[IFNAMSIZ];
    const char* parent_interface = NULL;
	struct nl_msg *msg = NULL;
	unsigned char *msg_buf = NULL;
    int error = 0;
	int len = 0;

	int if_index = rtnl_link_get_ifindex(link);
	const char *name = rtnl_link_get_name(link);

	// send RTM_GETLINK message for the bridge
	error = rtnl_link_build_get_request(if_index, name, &msg);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rtnl_link_build_get_request() failed (%d)", error);
		goto out;
	}
	len = nl_send_auto(nl_ctx->socket, msg);
	if (len < 0) {
		error = len;
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_send_auto() failed (%d)", error);
		goto out;
	}

	// wait for kernel response and ack
	struct sockaddr_nl nla = {0};
	len = nl_recv(nl_ctx->socket, &nla, &msg_buf, NULL);
	if (len <= 0) {
		error = len;
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_recv() failed (%d)", error);
		goto out;
	}
	error = nl_wait_for_ack(nl_ctx->socket);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "nl_wait_for_ack() failed (%d)", error);
		goto out;
	}

	// validate message type
	struct nlmsghdr *hdr = (struct nlmsghdr *) msg_buf;
	if (hdr->nlmsg_type != RTM_NEWLINK) {
        error = -1;
		SRPLG_LOG_ERR(PLUGIN_NAME, "unexpected message type in received response (%d)", hdr->nlmsg_type);
		goto out;
	}
	// RTM_NEWLINK messages contain an ifinfomsg structure
	// followed by a series of rtattr structures, see `man 7 rtnetlink`.
    struct ifinfomsg* ifinfo_msg = (struct ifinfomsg *)nlmsg_data(hdr);
    if ( ifinfo_msg->ifi_type != ARPHRD_ETHER )
    {
		goto out;
    }

	int proto_header_len = sizeof(struct ifinfomsg);
    struct nlattr *ifla_link = nlmsg_find_attr(hdr, proto_header_len, IFLA_LINK);
    if (ifla_link)
    {
        int ifi_index = *(int*)nla_data(ifla_link);
        parent_interface = rtnl_link_i2name(nl_ctx->link_cache, ifi_index, tmp, sizeof(tmp));
    }

out:

    if (error == 0)
    {
        SRPC_SAFE_CALL_ERR(error, interfaces_interface_hash_element_set_parent_interface(element, parent_interface), out);
    }

	if (msg != NULL) {
		nlmsg_free(msg);
	}
	if (msg_buf != NULL) {
		free(msg_buf);
	}

    return error;
}
