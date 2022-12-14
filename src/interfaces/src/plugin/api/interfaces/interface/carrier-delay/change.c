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
#include "change.h"
#include "plugin/common.h"

#include <sysrepo.h>

int interfaces_interface_carrier_delay_change_up(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        break;
    case SR_OP_MODIFIED:
        break;
    case SR_OP_DELETED:
        break;
    case SR_OP_MOVED:
        break;
    }

    return error;
}

int interfaces_interface_carrier_delay_change_down(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx)
{
    int error = 0;
    const char* node_name = LYD_NAME(change_ctx->node);
    const char* node_value = lyd_get_value(change_ctx->node);

    SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

    switch (change_ctx->operation) {
    case SR_OP_CREATED:
        break;
    case SR_OP_MODIFIED:
        break;
    case SR_OP_DELETED:
        break;
    case SR_OP_MOVED:
        break;
    }

    return error;
}
