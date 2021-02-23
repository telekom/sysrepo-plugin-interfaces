#include <linux/limits.h>
#include <sys/socket.h>
#include <sysrepo.h>
#include "libyang/tree_data.h"
#include "netlink/cache.h"
#include "netlink/errno.h"
#include "netlink/netlink.h"
#include "utils/memory.h"
#include <string.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <libyang/libyang.h>

#define BASE_YANG_MODEL "ietf-interfaces"

// config data
#define INTERFACES_YANG_MODEL "/" BASE_YANG_MODEL ":interfaces"
#define INTERFACE_LIST_YANG_PATH INTERFACES_YANG_MODEL "/interface"

// callbacks
static int interfaces_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
static int interfaces_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

// helper functions
static char *interfaces_xpath_get(const struct lyd_node *node);

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *startup_session = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	*private_data = NULL;

	SRP_LOG_INFMSG("start session to startup datastore");

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	*private_data = startup_session;

	// sub to any module change - for now
	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, "/" BASE_YANG_MODEL ":*//*", interfaces_module_change_cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_oper_get_items_subscribe(session, BASE_YANG_MODEL, INTERFACES_YANG_MODEL "/*", interfaces_state_data_cb, NULL, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_oper_get_items_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRP_LOG_INFMSG("plugin init done");

	goto out;

error_out:
	sr_unsubscribe(subscription);

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

	SRP_LOG_INFMSG("plugin cleanup finished");
}

static int interfaces_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;
	sr_change_iter_t *system_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const struct lyd_node *node = NULL;
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	bool prev_default = false;
	char *node_xpath = NULL;
	const char *node_value = NULL;
	struct lyd_node_leaf_list *node_leaf_list;
	struct lys_node_leaf *schema_node_leaf;

	SRP_LOG_INF("module_name: %s, xpath: %s, event: %d, request_id: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRP_LOG_ERR("aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	}

	if (event == SR_EV_DONE) {
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	if (event == SR_EV_CHANGE) {
		error = sr_get_changes_iter(session, xpath, &system_change_iter);
		if (error) {
			SRP_LOG_ERR("sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
		while (sr_get_change_tree_next(session, system_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			node_xpath = interfaces_xpath_get(node);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				node_leaf_list = (struct lyd_node_leaf_list *) node;
				node_value = node_leaf_list->value_str;
				if (node_value == NULL) {
					schema_node_leaf = (struct lys_node_leaf *) node_leaf_list->schema;
					node_value = schema_node_leaf->dflt ? schema_node_leaf->dflt : "";
				}
			}

			SRP_LOG_DBG("node_xpath: %s; prev_val: %s; node_val: %s; operation: %d", node_xpath, prev_value, node_value, operation);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				if (operation == SR_OP_CREATED || operation == SR_OP_MODIFIED) {
					if (error) {
						SRP_LOG_ERR("set_config_value error (%d)", error);
						goto error_out;
					}
				}
			}
			FREE_SAFE(node_xpath);
			node_value = NULL;
		}
	}
	goto out;

error_out:
	// nothing for now
out:
	FREE_SAFE(node_xpath);
	sr_free_change_iter(system_change_iter);
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int interfaces_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	const struct ly_ctx *ly_ctx = NULL;
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rtnl_link *link = NULL;
	char tmp_buffer[PATH_MAX] = {0};
	char xpath_buffer[PATH_MAX] = {0};
	char interface_path_buffer[PATH_MAX] = {0};

	if (*parent == NULL) {
		ly_ctx = sr_get_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			error = SR_ERR_CALLBACK_FAILED;
			goto error_out;
		}
		*parent = lyd_new_path(NULL, ly_ctx, request_xpath, NULL, 0, 0);
	}

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERR("nl_socket_alloc error: invalid socket");
		goto error_out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRP_LOG_ERR("nl_connect error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		snprintf(interface_path_buffer, sizeof(interface_path_buffer) / sizeof(char), "%s[name=\"%s\"]", INTERFACE_LIST_YANG_PATH, rtnl_link_get_name(link));

		// name
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/name", interface_path_buffer);
		SRP_LOG_DBG("%s = %s", xpath_buffer, rtnl_link_get_name(link));
		lyd_new_path(*parent, ly_ctx, xpath_buffer, rtnl_link_get_name(link), LYD_ANYDATA_CONSTSTRING, 0);

		// oper-status
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/oper-status", interface_path_buffer);
		rtnl_link_operstate2str(rtnl_link_get_operstate(link), tmp_buffer, sizeof(tmp_buffer));
		SRP_LOG_DBG("%s = %s", xpath_buffer, tmp_buffer);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// statistics test
		uint64_t unk_protos = rtnl_link_get_stat(link, RTNL_LINK_IP6_INUNKNOWNPROTOS);
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-unknown-protos", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", unk_protos);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// continue to next link node
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	nl_cache_free(cache);

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	nl_socket_free(socket);
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static char *interfaces_xpath_get(const struct lyd_node *node)
{
	char *xpath_node = NULL;
	char *xpath_leaflist_open_bracket = NULL;
	size_t xpath_trimed_size = 0;
	char *xpath_trimed = NULL;

	if (node->schema->nodetype == LYS_LEAFLIST) {
		xpath_node = lyd_path(node);
		xpath_leaflist_open_bracket = strrchr(xpath_node, '[');
		if (xpath_leaflist_open_bracket == NULL) {
			return xpath_node;
		}

		xpath_trimed_size = (size_t) xpath_leaflist_open_bracket - (size_t) xpath_node + 1;
		xpath_trimed = xcalloc(1, xpath_trimed_size);
		strncpy(xpath_trimed, xpath_node, xpath_trimed_size - 1);
		xpath_trimed[xpath_trimed_size - 1] = '\0';

		FREE_SAFE(xpath_node);

		return xpath_trimed;
	} else {
		return lyd_path(node);
	}
}

#ifndef PLUGIN
#include <signal.h>
#include <unistd.h>

volatile int exit_application = 0;

static void sigint_handler(__attribute__((unused)) int signum);

int main(void)
{
	int error = SR_ERR_OK;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *session = NULL;
	void *private_data = NULL;

	sr_log_stderr(SR_LL_DBG);

	error = sr_connect(SR_CONN_DEFAULT, &connection);
	if (error) {
		SRP_LOG_ERR("sr_connect error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_session_start(connection, SR_DS_RUNNING, &session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_plugin_init_cb(session, &private_data);
	if (error) {
		SRP_LOG_ERRMSG("sr_plugin_init_cb error");
		goto out;
	}

	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, SIG_IGN);
	while (!exit_application) {
		sleep(1);
	}

out:
	sr_plugin_cleanup_cb(session, private_data);
	sr_disconnect(connection);

	return error ? -1 : 0;
}

static void sigint_handler(__attribute__((unused)) int signum)
{
	SRP_LOG_INFMSG("Sigint called, exiting...");
	exit_application = 1;
}

#endif
