#include <linux/limits.h>
#include <linux/if.h>
#include <sys/socket.h>
#include <errno.h>
#include <sysrepo.h>
#include <sysrepo/xpath.h>
#include <libyang/tree_data.h>
#include <netlink/cache.h>
#include <netlink/errno.h>
#include <netlink/netlink.h>
#include "utils/memory.h"
#include <string.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <libyang/libyang.h>

#define LD_MAX_LINKS 100 // TODO: check this

typedef struct {
	char *name;
	char *description;
	char *type;
	char *enabled;
	bool delete;
} link_data_t;

typedef struct {
	link_data_t links[LD_MAX_LINKS];
	uint8_t count;
} link_data_list_t;

static link_data_list_t link_data_list = {0};

// TODO: update
const char *link_types[] = {
	"bridge",
	"bond",
	"dummy"
	};

#define BASE_YANG_MODEL "ietf-interfaces"

#define SYSREPOCFG_EMPTY_CHECK_COMMAND "sysrepocfg -X -d running -m " BASE_YANG_MODEL

// config data
#define INTERFACES_YANG_MODEL "/" BASE_YANG_MODEL ":interfaces"
#define INTERFACE_LIST_YANG_PATH INTERFACES_YANG_MODEL "/interface"

// callbacks
static int interfaces_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
static int interfaces_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

// helper functions
static bool system_running_datastore_is_empty_check(void);
static int load_data(sr_session_ctx_t *session);
static char *interfaces_xpath_get(const struct lyd_node *node);
int set_config_value(const char *xpath, const char *value);
int delete_config_value(const char *xpath, const char *value);
int update_link_info(link_data_list_t *ld, sr_change_oper_t operation);

void link_init(link_data_t *l);
void link_set_name(link_data_t *l, char *name);
void link_data_list_init(link_data_list_t *ld);
int link_data_list_remove(link_data_list_t *ld, char *name);
int link_data_list_add(link_data_list_t *ld, char *name);
int link_data_list_set_description(link_data_list_t *ld, char *name, char *description);
int link_data_list_set_type(link_data_list_t *ld, char *name, char *type);
int link_data_list_set_enabled(link_data_list_t *ld, char *name, char *enabled);
int link_data_list_set_delete(link_data_list_t *ld, char *name, bool delete);
void link_data_list_free(link_data_list_t *ld);
void link_data_free(link_data_t *l);

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *startup_session = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	*private_data = NULL;

	link_data_list_init(&link_data_list);

	SRP_LOG_INFMSG("start session to startup datastore");

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	*private_data = startup_session;

	/* TODO: Uncomment when load_data is implemented (after operational data is done)
	if (system_running_datastore_is_empty_check() == true) {
		SRP_LOG_INFMSG("running DS is empty, loading data");

		error = load_data(session);
		if (error) {
			SRP_LOG_ERRMSG("load_data error");
			goto error_out;
		}

		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} */

	SRP_LOG_INFMSG("subscribing to module change");

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

static bool system_running_datastore_is_empty_check(void)
{
	FILE *sysrepocfg_DS_empty_check = NULL;
	bool is_empty = false;

	sysrepocfg_DS_empty_check = popen(SYSREPOCFG_EMPTY_CHECK_COMMAND, "r");
	if (sysrepocfg_DS_empty_check == NULL) {
		SRP_LOG_WRN("could not execute %s", SYSREPOCFG_EMPTY_CHECK_COMMAND);
		is_empty = true;
		goto out;
	}

	if (fgetc(sysrepocfg_DS_empty_check) == EOF) {
		is_empty = true;
	}

out:
	if (sysrepocfg_DS_empty_check) {
		pclose(sysrepocfg_DS_empty_check);
	}

	return is_empty;
}

static int load_data(sr_session_ctx_t *session)
{
	int error = 0;

	// TODO: call function to gather everything from interfaces_state_data_cb

	error = sr_apply_changes(session, 0, 0);
	if (error) {
		SRP_LOG_ERR("sr_apply_changes error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	return 0;
error_out:
	return -1;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

	link_data_list_free(&link_data_list);

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
					error = set_config_value(node_xpath, node_value);
					if (error) {
						SRP_LOG_ERR("set_config_value error (%d)", error);
						goto error_out;
					}
				} else if (operation == SR_OP_DELETED) {
					error = delete_config_value(node_xpath, node_value);
					if (error) {
						SRP_LOG_ERR("delete_config_value error (%d)", error);
						goto error_out;
					}
				}
			}
			FREE_SAFE(node_xpath);
			node_value = NULL;
		}
		update_link_info(&link_data_list, operation);
	}
	goto out;

error_out:
	// nothing for now
out:
	FREE_SAFE(node_xpath);
	sr_free_change_iter(system_change_iter);
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int set_config_value(const char *xpath, const char *value)
{
	int error = SR_ERR_OK;
	char *interface_node = NULL;
	char *interface_node_name = NULL;
	sr_xpath_ctx_t state = {0};

	interface_node = sr_xpath_node_name((char *) xpath);
	if (interface_node == NULL) {
		SRP_LOG_ERRMSG("sr_xpath_node_name error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	interface_node_name = sr_xpath_key_value((char *) xpath, "interface", "name", &state);
	if (interface_node_name == NULL) {
		SRP_LOG_ERRMSG("sr_xpath_key_value error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	error = link_data_list_add(&link_data_list, interface_node_name);
	if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_add error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
	}

	if (strcmp(interface_node, "description") == 0) {
		// change desc
		error = link_data_list_set_description(&link_data_list, interface_node_name, (char *)value);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_description error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "type") == 0) {
		// change type

		if (strstr(value, "ethernetCsmacd") != NULL) {
			// can't add a new eth interface
			//link_data_list_set_type(&link_data_list, interface_node_name, "eth");
		} else if (strstr(value, "softwareLoopback") != NULL) {
			// can't add a new lo interface
			//link_data_list_set_type(&link_data_list, interface_node_name, "lo");
		}

		// TODO: update this
		error = link_data_list_set_type(&link_data_list, interface_node_name, "dummy");
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_type error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "enabled") == 0) {
		// change enabled
		error = link_data_list_set_enabled(&link_data_list, interface_node_name, (char *)value);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_enabled error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	}

	goto out;

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int delete_config_value(const char *xpath, const char *value)
{
	int error = SR_ERR_OK;
	//return SR_ERR_OK;
	char *interface_node = NULL;
	char *interface_node_name = NULL;
	sr_xpath_ctx_t state = {0};

	interface_node = sr_xpath_node_name((char *) xpath);
	if (interface_node == NULL) {
		SRP_LOG_ERRMSG("sr_xpath_node_name error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	interface_node_name = sr_xpath_key_value((char *) xpath, "interface", "name", &state);
	if (interface_node_name == NULL) {
		SRP_LOG_ERRMSG("sr_xpath_key_value error");
		error = SR_ERR_CALLBACK_FAILED;
		goto out;
	}

	if (strcmp(interface_node, "name") == 0) {
		// mark for deletion
		error = link_data_list_set_delete(&link_data_list, interface_node_name, true);
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_delete error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "description") == 0) {
		// set description to empty string
		error = link_data_list_set_description(&link_data_list, interface_node_name, "");
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_description error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	} else if (strcmp(interface_node, "enabled") == 0) {
		// set enabled to false
		error = link_data_list_set_enabled(&link_data_list, interface_node_name, "");
		if (error != 0) {
			SRP_LOG_ERRMSG("link_data_list_set_enabled error");
			error = SR_ERR_CALLBACK_FAILED;
			goto out;
		}
	}

out:

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int update_link_info(link_data_list_t *ld, sr_change_oper_t operation)
{
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;

	int error = SR_ERR_OK;

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("nl_socket_alloc error: invalid socket");
		goto out;
	}

	if ((error = nl_connect(socket, NETLINK_ROUTE)) != 0) {
		SRP_LOG_ERR("nl_connect error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache error (%d): %s", error, nl_geterror(error));
		goto out;
	}

	for (int i = 0; i < ld->count; i++) {
		char *name = ld->links[i].name;
		char *type = ld->links[i].type;
		char *description = ld->links[i].description;
		char *enabled = ld->links[i].enabled;
		bool delete = ld->links[i].delete;

		struct rtnl_link *old = rtnl_link_get_by_name(cache, name);
		struct rtnl_link *request = rtnl_link_alloc();

		// delete if marked for deletion
		if (delete) {
			// delete the link
			error = rtnl_link_delete(socket, old);
			if (error < 0) {
				SRP_LOG_ERR("rtnl_link_delete error (%d): %s", error, nl_geterror(error));
				goto out;
			}

			// free the link from link_data_list
			link_data_free(&ld->links[i]);

			// cleanup
			if (old != NULL) {
				rtnl_link_put(old);
			}

			if (request != NULL) {
				rtnl_link_put(request);
			}

			// and continue
			continue;
		}

		// desc
		if (description != NULL) {
			rtnl_link_set_ifalias(request, description);
		}

		// type
		if (type != NULL) {
			error = rtnl_link_set_type(request, type);
			if (error < 0) {
				SRP_LOG_ERR("rtnl_link_set_type error (%d): %s", error, nl_geterror(error));
				goto out;
			}
		}

		// enabled
		if (enabled != NULL) {
			if (strcmp(enabled, "true") == 0) {
				// set the interface to UP
				rtnl_link_set_flags(request, (unsigned int)rtnl_link_str2flags("up"));
				rtnl_link_set_operstate(request, IF_OPER_UP);
			} else {
				// set the interface to DOWN
				rtnl_link_unset_flags(request, (unsigned int)rtnl_link_str2flags("up"));
				rtnl_link_set_operstate(request, IF_OPER_DOWN);
			}
		}

		if (old != NULL) {
			// the interface with name already exists, change it
			error = rtnl_link_change(socket, old, request, 0);
			if (error != 0) {
				SRP_LOG_ERR("rtnl_link_change error (%d): %s", error, nl_geterror(error));
				goto out;
			}
		} else {
			if (operation != SR_OP_DELETED) {
				// the interface doesn't exist

				// set the new name
				rtnl_link_set_name(request, name);

				// add the interface
				// note: if type is not set, you can't add the new link
				error = rtnl_link_add(socket, request, NLM_F_CREATE);
				if (error != 0) {
					SRP_LOG_ERR("rtnl_link_add error (%d): %s", error, nl_geterror(error));
					goto out;
				}
			}
		}
		rtnl_link_put(old);
		rtnl_link_put(request);
	}

out:
	nl_socket_free(socket);
	nl_cache_free(cache);
	
	return error;
}

// TODO: move these functions to a file in utils?
void link_init(link_data_t *l)
{
	l->name = NULL;
	l->description = NULL;
	l->type = NULL;
	l->enabled = NULL;
	l->delete = false;
}

void link_data_list_init(link_data_list_t *ld)
{
	for (int i = 0; i < LD_MAX_LINKS; i++) {
		link_init(&ld->links[i]);
	}
	ld->count = 0;
}

void link_set_name(link_data_t *l, char *name)
{
	unsigned long tmp_len = 0;
	tmp_len = strlen(name);
	l->name = xmalloc(sizeof(char) * (tmp_len + 1));
	memcpy(l->name, name, tmp_len);
	l->name[tmp_len] = 0;
}

int link_data_list_add(link_data_list_t *ld, char *name)
{
	bool name_found = false;

	if (ld->count >= LD_MAX_LINKS) {
		return EINVAL;
	}

	for (int i = 0; i < ld->count; i++) {
		if (ld->links[i].name != NULL) { // in case we deleted a link it will be NULL
			if (strcmp(ld->links[i].name, name) == 0) {
				name_found = true;
				break;
			}
		}
	}

	if (!name_found) {
		// set the new link to the first free one in the list
		// the one with name == 0
		int pos = ld->count;
		for (int i = 0; i < ld->count; i++) {
			if (ld->links[i].name == 0) {
				pos = i;
				// in case a link was deleted don't increase the counter
				if (pos < ld->count) {
					ld->count--;
				}
				break;
			}
		}
		link_set_name(&ld->links[pos], name);
		++ld->count;
	}
	
	return 0;
}

int link_data_list_set_description(link_data_list_t *ld, char *name, char *description)
{
	int error = 0;
	int name_found = 0;

	for (int i = 0; i < ld->count; i++) {
		if (strcmp(ld->links[i].name, name) == 0) {
			name_found = 1;

			if (ld->links[i].description  != NULL) {
				FREE_SAFE(ld->links[i].description );
			}

			unsigned long tmp_len = 0;
			tmp_len = strlen(description);
			ld->links[i].description = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(ld->links[i].description, description, tmp_len);
			ld->links[i].description[tmp_len] = 0;

			break;
		}
	}
	if (!name_found) {
		// error
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_type(link_data_list_t *ld, char *name, char *type)
{
	int error = 0;
	int name_found = 0;

	for (int i = 0; i < ld->count; i++) {
		if (strcmp(ld->links[i].name, name) == 0) {
			name_found = 1;

			if (ld->links[i].type  != NULL) {
				FREE_SAFE(ld->links[i].type );
			}

			unsigned long tmp_len = 0;
			tmp_len = strlen(type);
			ld->links[i].type = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(ld->links[i].type, type, tmp_len);
			ld->links[i].type[tmp_len] = 0;

			break;
		}
	}
	if (!name_found) {
		// error
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_enabled(link_data_list_t *ld, char *name, char *enabled)
{
	int error = 0;
	int name_found = 0;

	for (int i = 0; i < ld->count; i++) {
		if (strcmp(ld->links[i].name, name) == 0) {
			name_found = 1;

			if (ld->links[i].enabled  != NULL) {
				FREE_SAFE(ld->links[i].enabled );
			}

			unsigned long tmp_len = 0;
			tmp_len = strlen(enabled);
			ld->links[i].enabled = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(ld->links[i].enabled, enabled, tmp_len);
			ld->links[i].enabled[tmp_len] = 0;

			break;
		}
	}
	if (!name_found) {
		// error
		error = EINVAL;
	}
	return error;
}

int link_data_list_set_delete(link_data_list_t *ld, char *name, bool delete)
{
	int error = 0;
	int name_found = 0;

	for (int i = 0; i < ld->count; i++) {
		if (strcmp(ld->links[i].name, name) == 0) {
			name_found = 1;
			ld->links[i].delete = delete;
			break;
		}
	}
	if (!name_found) {
		// error
		error = EINVAL;
	}
	return error;
}

void link_data_free(link_data_t *l)
{
	if (l->name) {
		FREE_SAFE(l->name);
	}

	if (l->description) {
		FREE_SAFE(l->description);
	}

	if (l->type) {
		FREE_SAFE(l->type);
	}

	if (l->enabled) {
		FREE_SAFE(l->enabled);
	}
}

void link_data_list_free(link_data_list_t *ld)
{
	for (int i = 0; i < ld->count; i++) {
		link_data_free(&ld->links[i]);
	}
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
		SRP_LOG_ERRMSG("nl_socket_alloc error: invalid socket");
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
