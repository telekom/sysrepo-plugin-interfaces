#ifndef DHCPV6_CLIENT_ONCE_H
#define DHCPV6_CLIENT_ONCE_H

#include <sysrepo.h>
#include <sysrepo/xpath.h>
#include <libyang/libyang.h>
#include <libyang/tree_data.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>

#include "../../memory.h"
#include "../dhcpv6_client_list/dhcpv6_client_list.h"
#include "../dhcpv6_client_config/dhcpv6_client_config.h"
#include "../dhcpv6_client_config/dhcpv6_client_config_file/dhcpv6_client_config_file.h"

/* client config list which holds all configuration parameters for each interface */
static config_data_list_t client_config_list = {0};

int dhcpv6_client_subscribe(sr_session_ctx_t *session, void **private_data, sr_subscription_ctx_t **subscription);
int dhcpv6_client_init(sr_session_ctx_t *session, sr_session_ctx_t *startup_session);

#endif /* DHCPV6_CLIENT_ONCE_H */
