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
#ifndef INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_AUTOCONF_CHANGE_H
#define INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_AUTOCONF_CHANGE_H

#include <srpc.h>
#include <utarray.h>

int interfaces_interface_ipv6_autoconf_change_temporary_preferred_lifetime_init(void* priv);
int interfaces_interface_ipv6_autoconf_change_temporary_preferred_lifetime(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv6_autoconf_change_temporary_preferred_lifetime_free(void* priv);
int interfaces_interface_ipv6_autoconf_change_temporary_valid_lifetime_init(void* priv);
int interfaces_interface_ipv6_autoconf_change_temporary_valid_lifetime(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv6_autoconf_change_temporary_valid_lifetime_free(void* priv);
int interfaces_interface_ipv6_autoconf_change_create_temporary_addresses_init(void* priv);
int interfaces_interface_ipv6_autoconf_change_create_temporary_addresses(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv6_autoconf_change_create_temporary_addresses_free(void* priv);
int interfaces_interface_ipv6_autoconf_change_create_global_addresses_init(void* priv);
int interfaces_interface_ipv6_autoconf_change_create_global_addresses(void* priv, sr_session_ctx_t* session, const srpc_change_ctx_t* change_ctx);
void interfaces_interface_ipv6_autoconf_change_create_global_addresses_free(void* priv);

#endif // INTERFACES_PLUGIN_API_INTERFACES_INTERFACE_IPV6_AUTOCONF_CHANGE_H