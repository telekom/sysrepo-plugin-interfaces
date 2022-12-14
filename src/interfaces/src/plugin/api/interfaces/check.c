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
#include "check.h"
#include "plugin/api/interfaces/load.h"
#include "plugin/common.h"
#include "src/uthash.h"
#include "srpc/types.h"
#include "sysrepo.h"

#include <srpc.h>

srpc_check_status_t interfaces_check_interface(interfaces_ctx_t* ctx, const interfaces_interface_hash_element_t* if_hash)
{
    srpc_check_status_t status = srpc_check_status_none;
    int error = 0;

    // system data
    interfaces_interface_hash_element_t* system_if_hash = NULL;
    const interfaces_interface_hash_element_t *if_iter = NULL, *if_tmp = NULL;

    // load system interfaces
    // SRPC_SAFE_CALL_ERR(error, interfaces_load_interface(ctx, &system_if_hash), error_out);

    HASH_ITER(hh, if_hash, if_iter, if_tmp)
    {
    }

    status = srpc_check_status_non_existant;

    goto out;

error_out:
    status = srpc_check_status_error;

out:
    // free system interface data
    interfaces_interface_hash_free(&system_if_hash);

    return status;
}
