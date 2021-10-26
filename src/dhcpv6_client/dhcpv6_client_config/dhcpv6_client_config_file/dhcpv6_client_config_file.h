/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DHCPV6_CLIENT_CONFIG_FILE_ONCE_H
#define DHCPV6_CLIENT_CONFIG_FILE_ONCE_H

#include "../../dhcpv6_client_list/dhcpv6_client_list.h"
#include "utils/memory.h"

#include <sysrepo.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <ctype.h>

#define DHCLIENT_CONFIG_FILE "/etc/dhclient.conf"
#define TMP_DHCLIENT_CONFIG_FILE "tmp_dhclient.conf"
#define BAK_DHCLIENT_CONFIG_FILE "dhclient.conf.bak"

int dhcpv6_client_config_file_read(config_data_list_t *ccl);
void dhcpv6_client_config_file_check_entries(char *line, char **opt_line, char **opt);
int dhcpv6_client_config_file_parse_entries(char *opt_line, char *opt, char **opt_vals);

int dhcpv6_client_config_file_write_list(config_data_list_t ccl);
int dhcpv6_client_config_file_check(char *if_name, bool *del);
int dhcpv6_client_config_file_remove_entry(char *if_name);
int dhcpv6_client_config_file_write(dhcpv6_client_config_t client_config);
int dhcpv6_client_config_file_write_if_nam(char *if_name, FILE *fp);
int dhcpv6_client_config_file_write_duid(char *duid, FILE *fp);
int dhcpv6_client_config_file_write_oro(oro_options_t oro, FILE *fp);
int dhcpv6_client_config_file_write_rpd_cmt(bool rapid_commit_opt, FILE *fp);
int dhcpv6_client_config_file_write_usr_cls(user_class_option_t user_class, FILE *fp);
int dhcpv6_client_config_file_write_rcn_acpt(bool reconfigure_accept_opt, FILE *fp);
// int dhcpv6_client_config_file_write_vnd_cls_opt(vendor_class_option_t vendor_class_opt, FILE *fp);
int dhcpv6_client_config_file_write_vnd_spec_opt(vendor_specific_information_options_t vendor_opts, FILE *fp);
int dhcpv6_client_config_file_write_closing_brckt(FILE *fp);

char *strstrip(char *s);

#endif /* DHCPV6_CLIENT_CONFIG_FILE_ONCE_H */