#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <linux/genetlink.h>
#include <linux/limits.h>
#include <linux/neighbour.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <sys/socket.h>
#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <netlink/route/route.h>
#include <net/if.h>

#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include "routing.h"
#include "rib.h"
#include "rib/list.h"
#include "rib/description_pair.h"
#include "route/list.h"
#include "route/list_hash.h"
#include "control_plane_protocol.h"
#include "control_plane_protocol/list.h"
#include "utils/memory.h"

// dir for storing data used by the plugin - usually build directory of the plugin
#define ROUTING_PLUGIN_DATA_DIR "ROUTING_PLUGIN_DATA_DIR"

// other #define's used in the plugin
#define ROUTING_PROTOS_MAP_FNAME "protos_map"
#define ROUTING_RIBS_DESCRIPTIONS_MAP_FNAME "ribs_map"
#define ROUTING_RIBS_COUNT 256
#define ROUTING_PROTOS_COUNT 256

#define BASE_YANG_MODEL "ietf-routing"

// base of all - routing container
#define ROUTING_CONTAINER_YANG_PATH "/" BASE_YANG_MODEL ":routing"

// router ID
#define ROUTING_ROUTER_ID_YANG_PATH ROUTING_CONTAINER_YANG_PATH "/router-id"

// interfaces container
#define ROUTING_INTERFACES_CONTAINER_YANG_PATH ROUTING_CONTAINER_YANG_PATH "/interfaces"
#define ROUTING_INTERFACE_LEAF_LIST_YANG_PATH ROUTING_INTERFACES_CONTAINER_YANG_PATH "/interface"

// control plane protocols container
#define ROUTING_CONTROL_PLANE_PROTOCOLS_CONTAINER_YANG_PATH ROUTING_CONTAINER_YANG_PATH "/control-plane-protocols"
#define ROUTING_CONTROL_PLANE_PROTOCOL_LIST_YANG_PATH ROUTING_CONTROL_PLANE_PROTOCOLS_CONTAINER_YANG_PATH "/control-plane-protocol"

// ribs
#define ROUTING_RIBS_CONTAINER_YANG_PATH ROUTING_CONTAINER_YANG_PATH "/ribs"
#define ROUTING_RIB_LIST_YANG_PATH ROUTING_RIBS_CONTAINER_YANG_PATH "/rib"
#define ROUTING_RIB_LIST_ACTIVE_ROUTE_RPC_PATH ROUTING_RIB_LIST_YANG_PATH "/active-route"

// sysrepocfg
#define SYSREPOCFG_EMPTY_CHECK_COMMAND "sysrepocfg -X -d running -m " BASE_YANG_MODEL

// module changes
static int routing_module_change_control_plane_protocol_list_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
static int routing_module_change_rib_list_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);

// module changes helpers - changing/deleting values

// getting xpath from the node
static char *routing_xpath_get(const struct lyd_node *node);

// control-plane-protocol list module changes
static int routing_control_plane_protocol_set_description(const char *type, const char *name, const char *description);

// rib list module changes
static int routing_rib_set_address_family(const char *name, const char *address_family);
static int routing_rib_set_description(const char *name, const char *description);

// operational callbacks
static int routing_oper_get_interfaces_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);
static int routing_oper_get_rib_routes_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

// rpc callbacks
static int routing_rpc_active_route_cb(sr_session_ctx_t *session, const char *xpath, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

// initial loading into the datastore
static int routing_load_data(sr_session_ctx_t *session);
static int routing_load_ribs(sr_session_ctx_t *session, struct lyd_node *routing_container_node);
static int routing_collect_ribs(struct nl_cache *routes_cache, struct rib_list *ribs);
static int routing_collect_routes(struct nl_cache *routes_cache, struct rib_list *ribs);
static int routing_load_control_plane_protocols(sr_session_ctx_t *session, struct lyd_node *routing_container_node);
static int routing_build_rib_descriptions(struct rib_list *ribs);
static inline int routing_is_rib_known(int table);
static int routing_build_protos_map(struct control_plane_protocol map[ROUTING_PROTOS_COUNT]);
static inline int routing_is_proto_type_known(int type);
static bool routing_running_datastore_is_empty(void);

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;

	// sysrepo
	sr_session_ctx_t *startup_session = NULL;
	sr_conn_ctx_t *connection = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	*private_data = NULL;

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error: %d -> %s", error, sr_strerror(error));
	}

	*private_data = startup_session;

	if (routing_running_datastore_is_empty()) {
		SRP_LOG_INFMSG("running datasore is empty -> loading data");
		error = routing_load_data(session);
		if (error) {
			SRP_LOG_ERRMSG("error loading initial data into the datastore... exiting");
			goto error_out;
		}
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	SRP_LOG_INFMSG("subscribing to module change");

	// control-plane-protocol list module changes
	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, ROUTING_CONTROL_PLANE_PROTOCOL_LIST_YANG_PATH "//*", routing_module_change_control_plane_protocol_list_cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// rib list module changes
	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, ROUTING_RIB_LIST_YANG_PATH "//*", routing_module_change_rib_list_cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRP_LOG_INFMSG("subscribing to interfaces operational data");

	// interface leaf-list oper data
	error = sr_oper_get_items_subscribe(session, BASE_YANG_MODEL, ROUTING_INTERFACE_LEAF_LIST_YANG_PATH, routing_oper_get_interfaces_cb, NULL, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_oper_get_items_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// RIB oper data
	error = sr_oper_get_items_subscribe(session, BASE_YANG_MODEL, ROUTING_RIB_LIST_YANG_PATH, routing_oper_get_rib_routes_cb, NULL, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_oper_get_items_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRP_LOG_INFMSG("subscribing to plugin RPCs/actions");

	// active-route RPC/action
	error = sr_rpc_subscribe(session, ROUTING_RIB_LIST_ACTIVE_ROUTE_RPC_PATH, routing_rpc_active_route_cb, NULL, 1, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	goto out;
error_out:
	SRP_LOG_ERR("error occured while initializing the plugin -> %d", error);
out:
	return error;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
}

static int routing_module_change_control_plane_protocol_list_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;

	// sysrepo
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;
	sr_change_iter_t *routing_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	sr_xpath_ctx_t xpath_ctx = {0};

	// libyang
	const struct lyd_node *node = NULL;
	struct lyd_node_leaf_list *node_leaf_list;
	struct lys_node_leaf *schema_node_leaf;

	// temp helper vars
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	bool prev_default = false;
	char *node_xpath = NULL;
	const char *node_value = NULL;

	// keys
	char *type_key = NULL;
	char *name_key = NULL;
	char *node_name = NULL;

	SRP_LOG_INF("module_name: %s, xpath: %s, event: %d, request_id: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRP_LOG_ERR("aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		error = sr_get_changes_iter(session, xpath, &routing_change_iter);
		if (error) {
			SRP_LOG_ERR("sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, routing_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			node_xpath = routing_xpath_get(node);

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
				node_name = sr_xpath_last_node(node_xpath, &xpath_ctx);
				name_key = sr_xpath_key_value(node_xpath, "control-plane-protocol", "name", &xpath_ctx);
				type_key = sr_xpath_key_value(node_xpath, "control-plane-protocol", "type", &xpath_ctx);

				if (strncmp(node_name, "description", sizeof("description") - 1) == 0) {
					error = routing_control_plane_protocol_set_description(type_key, name_key, node_value);
					if (error != 0) {
						SRP_LOG_ERRMSG("routing_control_plane_protocol_set_description failed");
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
	SRP_LOG_ERR("error applying control plane protocols module changes");
	error = -1;

out:
	FREE_SAFE(node_xpath);
	sr_free_change_iter(routing_change_iter);

	return error != 0 ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int routing_module_change_rib_list_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;

	// sysrepo
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;
	sr_change_iter_t *routing_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	sr_xpath_ctx_t xpath_ctx = {0};

	// libyang
	const struct lyd_node *node = NULL;
	struct lyd_node_leaf_list *node_leaf_list;
	struct lys_node_leaf *schema_node_leaf;

	// temp helper vars
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	bool prev_default = false;
	char *node_xpath = NULL;
	const char *node_value = NULL;

	// node stuff
	char *name_key = NULL;
	char *node_name = NULL;

	SRP_LOG_INF("module_name: %s, xpath: %s, event: %d, request_id: %u", module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRP_LOG_ERR("aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		error = sr_get_changes_iter(session, xpath, &routing_change_iter);
		if (error) {
			SRP_LOG_ERR("sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, routing_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			node_xpath = routing_xpath_get(node);

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
				node_name = sr_xpath_last_node(node_xpath, &xpath_ctx);
				name_key = sr_xpath_key_value(node_xpath, "rib", "name", &xpath_ctx);

				if (strncmp(node_name, "description", sizeof("description") - 1) == 0) {
					error = routing_rib_set_description(name_key, node_value);
					if (error != 0) {
						SRP_LOG_ERRMSG("routing_rib_set_description failed");
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
	SRP_LOG_ERR("error applying ribs module changes");
	error = -1;

out:
	FREE_SAFE(node_xpath);
	sr_free_change_iter(routing_change_iter);

	return error != 0 ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int routing_control_plane_protocol_set_description(const char *type, const char *name, const char *description)
{
	int error = 0;

	struct control_plane_protocol protos[ROUTING_PROTOS_COUNT] = {0};

	// temp variables and buffers
	char *data_dir = NULL;
	char path_buffer[PATH_MAX] = {0};
	char line_buffer[PATH_MAX] = {0};
	int tmp_type = 0;
	char type_buffer[100] = {0};
	char desc_buffer[256] = {0};
	FILE *fptr = NULL;

	data_dir = getenv(ROUTING_PLUGIN_DATA_DIR);
	if (data_dir == NULL) {
		SRP_LOG_ERR("unable to use environment variable for the plugin data dir: %s", ROUTING_PLUGIN_DATA_DIR);
		goto error_out;
	}

	for (int i = 0; i < ROUTING_PROTOS_COUNT; i++) {
		control_plane_protocol_init(&protos[i]);
	}

	// check if file exists first
	snprintf(path_buffer, sizeof(path_buffer), "%s%s%s", data_dir, (data_dir[strlen(data_dir) - 1] == '/' ? "" : "/"), ROUTING_PROTOS_MAP_FNAME);

	if (access(path_buffer, F_OK) != 0) {
		SRP_LOG_ERRMSG("protocols map file doesnt exist - error");
		goto error_out;
	} else {
		fptr = fopen(path_buffer, "r");
		if (fptr == NULL) {
			SRP_LOG_ERR("unable to open file %s", path_buffer);
			goto error_out;
		}

		SRP_LOG_DBG("protocols map file exists - reading map values and changing description of '%s' to %s", name, description);

		while (fgets(line_buffer, sizeof(line_buffer), fptr) != NULL) {
			const int read_n = sscanf(line_buffer, "%d %s \"%256[^\"]\"", &tmp_type, type_buffer, desc_buffer);
			if (read_n == 3) {
				if (tmp_type >= 0 && tmp_type <= ROUTING_PROTOS_COUNT) {
					rtnl_route_proto2str(tmp_type, protos[tmp_type].name, sizeof(protos[tmp_type].name));
					protos[tmp_type].type = xstrdup(type_buffer);

					if (strcmp(protos[tmp_type].name, name) == 0) {
						// don't read this description - change it
						protos[tmp_type].description = xstrdup(description);
					} else {
						protos[tmp_type].description = xstrdup(desc_buffer);
					}
					protos[tmp_type].initialized = 1;
				} else {
					SRP_LOG_ERR("invalid protocol type found in the protocol types map file: %d", tmp_type);
					goto error_out;
				}
			} else {
				SRP_LOG_ERR("unable to properly read the protocol types map file format; read %d values", read_n);
				goto error_out;
			}
		}

		// return to the beginning and write protocols again - reopen because clearing the content of a file is needed
		fclose(fptr);
		fptr = fopen(path_buffer, "w");

		for (int i = 0; i < ROUTING_PROTOS_COUNT; i++) {
			if (protos[i].initialized == 1) {
				fprintf(fptr, "%d\t%s\t\"%s\"\n", i, protos[i].type, protos[i].description);
			}
		}
	}
	goto out;

error_out:
	error = -1;
out:
	if (fptr) {
		fclose(fptr);
	}

	for (int i = 0; i < ROUTING_PROTOS_COUNT; i++) {
		control_plane_protocol_free(&protos[i]);
	}
	return error;
}

static int routing_rib_set_address_family(const char *name, const char *address_family)
{
	int error = 0;
	return error;
}

static int routing_rib_set_description(const char *name, const char *description)
{
	int error = 0;

	// calcualte sizes for faster string comparisons later
	const size_t DESC_LEN = strlen(description);
	const size_t NAME_LEN = strlen(name);

	// file stream
	FILE *fptr = NULL;

	// buffers and temporary values
	char path_buffer[PATH_MAX] = {0};
	char line_buffer[PATH_MAX] = {0};
	const char *data_dir = NULL;
	struct rib_description_pair tmp_description = {0};

	// list of descriptions
	struct {
		struct rib_description_pair *list;
		int size;
	} rib_descriptions = {0};

	// first read all descriptions from the map file and change the wanted description
	// after that -> write those descriptions back to the file

	// get the data_dir variable using getenv
	data_dir = getenv(ROUTING_PLUGIN_DATA_DIR);
	if (data_dir == NULL) {
		SRP_LOG_ERR("unable to use environment variable for the plugin data dir: %s", ROUTING_PLUGIN_DATA_DIR);
		goto error_out;
	}

	// create file path and see if it exists
	snprintf(path_buffer, sizeof(path_buffer), "%s%s%s", data_dir, (data_dir[strlen(data_dir) - 1] == '/') ? "" : "/", ROUTING_RIBS_DESCRIPTIONS_MAP_FNAME);
	SRP_LOG_DBG("RIBs description map file path: %s", path_buffer);

	if (access(path_buffer, F_OK) == 0) {
		// file exists
		fptr = fopen(path_buffer, "r");
		if (fptr == NULL) {
			SRP_LOG_ERR("unable to open file %s", path_buffer);
			goto error_out;
		}

		while (fgets(line_buffer, sizeof(line_buffer), fptr) != NULL) {
			const int read_n = sscanf(line_buffer, "%s \"%256[^\"]\"", tmp_description.name, tmp_description.description);
			if (read_n == 2) {
				// add the description to the list
				rib_descriptions.list = xrealloc(rib_descriptions.list, sizeof(struct rib_description_pair) * (unsigned) (rib_descriptions.size + 1));
				memcpy(&rib_descriptions.list[rib_descriptions.size], &tmp_description, sizeof(struct rib_description_pair));
				rib_descriptions.size += 1;
			} else {
				SRP_LOG_ERR("unable to correctly read 2 values from the RIBs map file");
				goto error_out;
			}
		}
	} else {
		// file doesn't exist -> error
		SRP_LOG_ERR("%s file doesn't exist", path_buffer);
		goto error_out;
	}

	bool description_changed = false;

	SRP_LOG_DBG("setting description of %s to \"%s\"", name, description);

	for (int i = 0; i < rib_descriptions.size; i++) {
		struct rib_description_pair *PAIR = &rib_descriptions.list[i];
		if (strncmp(PAIR->name, name, NAME_LEN) == 0) {
			// found description -> change it
			if (DESC_LEN <= ROUTING_RIB_DESCRIPTION_SIZE) {
				memcpy(PAIR->description, description, DESC_LEN);
				PAIR->description[DESC_LEN] = 0;
				SRP_LOG_DBG("description = %s", PAIR->description);
				description_changed = true;
			} else {
				SRP_LOG_ERR("unable to set description: %d characters max...", ROUTING_RIB_DESCRIPTION_SIZE);
				goto error_out;
			}
			break;
		}
	}

	if (description_changed == false) {
		goto error_out;
	}

	// write changes back to the file
	fclose(fptr);
	fptr = fopen(path_buffer, "w");

	for (int i = 0; i < rib_descriptions.size; i++) {
		SRP_LOG_DBG("%s = %s", rib_descriptions.list[i].name, rib_descriptions.list[i].description);
		fprintf(fptr, "%s\t\"%s\"\n", rib_descriptions.list[i].name, rib_descriptions.list[i].description);
	}

	goto out;

error_out:
	SRP_LOG_ERR("unable to set description for %s", name);
	error = -1;

out:
	FREE_SAFE(rib_descriptions.list);
	if (fptr != NULL) {
		fclose(fptr);
	}
	return error;
}

static char *routing_xpath_get(const struct lyd_node *node)
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

static int routing_oper_get_interfaces_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	// error handling
	int error = SR_ERR_OK;

	// libyang
	const struct ly_ctx *ly_ctx = NULL;

	// libnl
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rtnl_link *link = NULL;

	if (*parent == NULL) {
		ly_ctx = sr_get_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			goto error_out;
		}
		lyd_new_path(*parent, ly_ctx, request_xpath, NULL, LYD_ANYDATA_CONSTSTRING, 0);
	}

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("unable to init nl_sock struct...");
		goto error_out;
	}

	error = nl_connect(socket, NETLINK_ROUTE);
	if (error != 0) {
		SRP_LOG_ERR("nl_connect failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = rtnl_link_alloc_cache(socket, AF_UNSPEC, &cache);
	if (error != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	SRP_LOG_DBGMSG("adding interfaces to the list");

	link = (struct rtnl_link *) nl_cache_get_first(cache);
	while (link) {
		const char *name = rtnl_link_get_name(link);
		SRP_LOG_DBG("adding interface '%s' ", name);
		lyd_new_path(*parent, ly_ctx, ROUTING_INTERFACE_LEAF_LIST_YANG_PATH, (void *) name, LYD_ANYDATA_STRING, 0);
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;
out:
	// free allocated objects
	nl_cache_free(cache);
	nl_socket_free(socket);

	return error;
}

static void foreach_nexthop(struct rtnl_nexthop *nh, void *arg)
{
	struct route_next_hop *nexthop = arg;
	route_next_hop_add_list(nexthop, rtnl_route_nh_get_ifindex(nh), rtnl_route_nh_get_gateway(nh));
}

static int routing_oper_get_rib_routes_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	int nl_err = 0;

	// libyang
	const struct ly_ctx *ly_ctx = NULL;
	const struct lys_module *ly_uv4mod = NULL, *ly_uv6mod = NULL;
	struct lyd_node *ly_node = NULL, *routes_node = NULL, *nh_node = NULL, *nh_list_node = NULL;

	// libnl
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct nl_cache *link_cache = NULL;
	struct rib_list ribs = {0};

	// temp buffers
	char routes_buffer[PATH_MAX];
	char value_buffer[PATH_MAX];
	char ip_buffer[INET6_ADDRSTRLEN];
	char prefix_buffer[INET6_ADDRSTRLEN + 1 + 3];

	ly_ctx = sr_get_context(sr_session_get_connection(session));

	ly_uv4mod = ly_ctx_get_module(ly_ctx, "ietf-ipv4-unicast-routing", "2018-03-13", 0);
	ly_uv6mod = ly_ctx_get_module(ly_ctx, "ietf-ipv6-unicast-routing", "2018-03-13", 0);

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("unable to init nl_sock struct...");
		goto error_out;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		SRP_LOG_ERR("nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_alloc_cache(socket, AF_UNSPEC, 0, &cache);
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_table_names("/etc/iproute2/rt_tables");
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_protocol_names("/etc/iproute2/rt_protos");
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	error = routing_collect_routes(cache, &ribs);
	if (error != 0) {
		goto error_out;
	}

	for (int hash_iter = 0; hash_iter < ribs.size; hash_iter++) {
		// create new routes container for every table
		const struct route_list_hash *ROUTES_HASH = &ribs.list[hash_iter].routes;
		const int ADDR_FAMILY = ribs.list[hash_iter].address_family;
		const char *TABLE_NAME = ribs.list[hash_iter].name;
		snprintf(routes_buffer, sizeof(routes_buffer), "%s[name='%s-%s']/routes", ROUTING_RIB_LIST_YANG_PATH, ADDR_FAMILY == AF_INET ? "ipv4" : "ipv6", TABLE_NAME);
		routes_node = lyd_new_path(*parent, ly_ctx, routes_buffer, NULL, LYD_ANYDATA_CONSTSTRING, LYD_PATH_OPT_UPDATE);

		for (int i = 0; i < ROUTES_HASH->size; i++) {
			struct route_list *list_ptr = &ROUTES_HASH->list_route[i];
			struct nl_addr *dst_prefix = ROUTES_HASH->list_addr[i];
			nl_addr2str(dst_prefix, ip_buffer, sizeof(ip_buffer));

			// check for prefix - libnl doesn't write prefix into the buffer if its 8*4/8*6 i.e. only that address/no subnet
			if (strchr(ip_buffer, '/') == NULL) {
				snprintf(prefix_buffer, sizeof(prefix_buffer), "%s/%d", ip_buffer, nl_addr_get_prefixlen(dst_prefix));
			} else {
				snprintf(prefix_buffer, sizeof(prefix_buffer), "%s", ip_buffer);
			}

			for (int j = 0; j < list_ptr->size; j++) {
				const struct route *ROUTE = &list_ptr->list[j];
				// create a new list and after that add properties to it
				ly_node = lyd_new_path(routes_node, ly_ctx, "routes/route", NULL, LYD_ANYDATA_CONSTSTRING, 0);

				// route-preference
				snprintf(value_buffer, sizeof(value_buffer), "%u", ROUTE->preference);
				SRP_LOG_DBG("route-preference = %s", value_buffer);
				lyd_new_path(ly_node, ly_ctx, "route-preference", (void *) value_buffer, LYD_ANYDATA_STRING, 0);

				// next-hop container
				const union route_next_hop_value *NEXTHOP = &ROUTE->next_hop.value;
				nh_node = lyd_new_path(ly_node, ly_ctx, "next-hop", NULL, LYD_ANYDATA_STRING, 0);
				switch (ROUTE->next_hop.kind) {
					case route_next_hop_kind_none:
						break;
					case route_next_hop_kind_simple: {
						struct rtnl_link *iface = rtnl_link_get(link_cache, ROUTE->next_hop.value.simple.ifindex);
						const char *if_name = rtnl_link_get_name(iface);

						// outgoing-interface
						SRP_LOG_DBG("outgoing-interface = %s", if_name);
						lyd_new_path(nh_node, ly_ctx, "outgoing-interface", (void *) if_name, LYD_ANYDATA_CONSTSTRING, 0);

						// next-hop-address
						if (NEXTHOP->simple.addr) {
							nl_addr2str(NEXTHOP->simple.addr, ip_buffer, sizeof(ip_buffer));
							if (ADDR_FAMILY == AF_INET && ly_uv4mod != NULL) {
								SRP_LOG_DBG("next-hop-address = %s", ip_buffer);
								lyd_new_leaf(nh_node, ly_uv4mod, "next-hop-address", ip_buffer);
							} else if (ADDR_FAMILY == AF_INET6 && ly_uv6mod != NULL) {
								SRP_LOG_DBG("destination-prefix = %s", prefix_buffer);
								lyd_new_leaf(nh_node, ly_uv6mod, "next-hop-address", prefix_buffer);
							}
						}
						rtnl_link_put(iface);
						break;
					}
					case route_next_hop_kind_special:
						break;
					case route_next_hop_kind_list: {
						const struct route_next_hop_list *NEXTHOP_LIST = &ROUTE->next_hop.value.list;
						nh_list_node = lyd_new_path(nh_node, ly_ctx, "next-hop-list/next-hop", NULL, LYD_ANYDATA_CONSTSTRING, 0);
						for (int k = 0; k < NEXTHOP_LIST->size; k++) {
							struct rtnl_link *iface = rtnl_link_get(link_cache, NEXTHOP_LIST->list[k].ifindex);
							const char *if_name = rtnl_link_get_name(iface);

							// outgoing-interface
							SRP_LOG_DBG("outgoing-interface = %s", if_name);
							lyd_new_path(nh_list_node, ly_ctx, "outgoing-interface", (void *) if_name, LYD_ANYDATA_CONSTSTRING, 0);

							// next-hop-address
							if (NEXTHOP_LIST->list[k].addr) {
								nl_addr2str(NEXTHOP_LIST->list[k].addr, ip_buffer, sizeof(ip_buffer));
								if (ADDR_FAMILY == AF_INET && ly_uv4mod != NULL) {
									SRP_LOG_DBG("next-hop/next-hop-list/next-hop/next-hop-address = %s", ip_buffer);
									lyd_new_leaf(nh_list_node, ly_uv4mod, "next-hop-address", ip_buffer);
								} else if (ADDR_FAMILY == AF_INET6 && ly_uv6mod != NULL) {
									SRP_LOG_DBG("destination-prefix = %s", prefix_buffer);
									lyd_new_leaf(nh_list_node, ly_uv6mod, "next-hop-address", prefix_buffer);
								}
							}
							rtnl_link_put(iface);
						}
						break;
					}
				}

				// destination-prefix
				if (ADDR_FAMILY == AF_INET && ly_uv4mod != NULL) {
					SRP_LOG_DBG("destination-prefix = %s", prefix_buffer);
					lyd_new_leaf(ly_node, ly_uv4mod, "destination-prefix", prefix_buffer);
				} else if (ADDR_FAMILY == AF_INET6 && ly_uv6mod != NULL) {
					SRP_LOG_DBG("destination-prefix = %s", prefix_buffer);
					lyd_new_leaf(ly_node, ly_uv6mod, "destination-prefix", prefix_buffer);
				}

				// route-metadata/source-protocol
				SRP_LOG_DBG("source-protocol = %s", ROUTE->metadata.source_protocol);
				lyd_new_path(ly_node, ly_ctx, "source-protocol", ROUTE->metadata.source_protocol, LYD_ANYDATA_CONSTSTRING, 0);

				// route-metadata/active
				if (ROUTE->metadata.active == 1) {
					SRP_LOG_DBG("active = %d", ROUTE->metadata.active);
					lyd_new_path(ly_node, ly_ctx, "active", NULL, LYD_ANYDATA_CONSTSTRING, 0);
				}
			}
		}
	}

	if (error != 0) {
		goto error_out;
	}

	goto out;

error_out:
	SRP_LOG_ERR("unable to return routes for routing tables");
	error = SR_ERR_CALLBACK_FAILED;

out:
	rib_list_free(&ribs);
	nl_socket_free(socket);
	nl_cache_free(cache);
	nl_cache_free(link_cache);
	return error;
}

static int routing_rpc_active_route_cb(sr_session_ctx_t *session, const char *xpath, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
	int error = SR_ERR_OK;
	SRP_LOG_DBG("xpath for RPC: %s", xpath);
	return error;
}

static int routing_load_data(sr_session_ctx_t *session)
{
	int error = 0;
	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *routing_container_node = NULL;

	ly_ctx = sr_get_context(sr_session_get_connection(session));
	if (ly_ctx == NULL) {
		SRP_LOG_ERRMSG("unable to get ly_ctx variable... exiting immediately");
		goto error_out;
	}

	// create the routing container node and pass it to other functions which will add to the given node
	routing_container_node = lyd_new_path(NULL, ly_ctx, ROUTING_CONTAINER_YANG_PATH, NULL, LYD_ANYDATA_CONSTSTRING, 0);
	if (routing_container_node == NULL) {
		SRP_LOG_ERRMSG("unable to create the routing container ly node");
		goto error_out;
	}

	error = routing_load_ribs(session, routing_container_node);
	if (error) {
		SRP_LOG_ERR("routing_load_ribs failed : %d", error);
		goto error_out;
	}

	error = routing_load_control_plane_protocols(session, routing_container_node);
	if (error) {
		SRP_LOG_ERR("routing_load_control_plane_protocols failed : %d", error);
		goto error_out;
	}

	// after all nodes have been added with no error -> edit the values and apply changes
	error = sr_edit_batch(session, routing_container_node, "merge");
	if (error != SR_ERR_OK) {
		SRP_LOG_ERR("sr_edit_batch error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}
	error = sr_apply_changes(session, 0, 0);
	if (error != 0) {
		SRP_LOG_ERR("sr_apply_changes error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	goto out;
error_out:
	SRP_LOG_ERRMSG("unable to load initial data");
out:
	if (routing_container_node) {
		lyd_free(routing_container_node);
	}
	return error;
}

static int routing_load_ribs(sr_session_ctx_t *session, struct lyd_node *routing_container_node)
{
	// error handling
	int error = 0;
	int nl_err = 0;

	// libyang
	struct lyd_node *ribs_node = NULL, *rib_node = NULL, *added_node = NULL;
	const struct ly_ctx *ly_ctx = NULL;
	const struct lys_module *ly_uv4mod = NULL, *ly_uv6mod = NULL;

	// libnl
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rib_list ribs = {0};

	// temp buffers
	char list_buffer[PATH_MAX] = {0};

	ly_ctx = sr_get_context(sr_session_get_connection(session));
	if (ly_ctx) {
		ly_uv4mod = ly_ctx_get_module(ly_ctx, "ietf-ipv4-unicast-routing", "2018-03-13", 0);
		ly_uv6mod = ly_ctx_get_module(ly_ctx, "ietf-ipv6-unicast-routing", "2018-03-13", 0);
	} else {
		SRP_LOG_ERR("unable to load external modules... exiting");
		error = -1;
		goto error_out;
	}

	rib_list_init(&ribs);

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("unable to init nl_sock struct...");
		goto error_out;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		SRP_LOG_ERR("nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_alloc_cache(socket, AF_UNSPEC, 0, &cache);
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_table_names("/etc/iproute2/rt_tables");
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_protocol_names("/etc/iproute2/rt_protos");
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	error = routing_collect_ribs(cache, &ribs);
	if (error != 0) {
		goto error_out;
	}

	ribs_node = lyd_new_path(routing_container_node, ly_ctx, ROUTING_RIBS_CONTAINER_YANG_PATH, NULL, LYD_ANYDATA_CONSTSTRING, 0);
	if (ribs_node == NULL) {
		SRP_LOG_ERR("unable to create rib list container...");
		goto error_out;
	}

	// all RIBs loaded - add them to the initial config
	struct rib *iter = NULL;
	for (int i = 0; i < ribs.size; i++) {
		iter = &ribs.list[i];
		SRP_LOG_DBG("adding table %s to the list", iter->name);

		// write the current adding table into the buffer
		snprintf(list_buffer, sizeof(list_buffer), "%s[name='%s-%s']", ROUTING_RIB_LIST_YANG_PATH, iter->address_family == AF_INET ? "ipv4" : "ipv6", iter->name);
		rib_node = lyd_new_path(ribs_node, ly_ctx, list_buffer, NULL, LYD_ANYDATA_CONSTSTRING, 0);
		if (rib_node == NULL) {
			SRP_LOG_ERR("unable to add new list...");
			goto error_out;
		}

		// address family
		added_node = lyd_new_path(rib_node, ly_ctx, "address-family", iter->address_family == AF_INET ? "ipv4" : "ipv6", LYD_ANYDATA_CONSTSTRING, 0);
		if (added_node == NULL) {
			SRP_LOG_ERR("unable to add 'address-family' node to the tree");
			goto error_out;
		}

		// description
		if (iter->description[0] != 0) {
			added_node = lyd_new_path(rib_node, ly_ctx, "description", iter->description, LYD_ANYDATA_STRING, 0);
			if (added_node == NULL) {
				SRP_LOG_ERR("unable to add 'description' node to the rib");
				goto error_out;
			}
		}
	}

	goto out;
error_out:
	SRP_LOG_ERR("error loading initial data");
	error = -1;

out:
	rib_list_free(&ribs);
	nl_socket_free(socket);
	nl_cache_free(cache);

	return error;
}

static int routing_collect_ribs(struct nl_cache *routes_cache, struct rib_list *ribs)
{
	int error = 0;
	struct rtnl_route *route = NULL;
	char table_buffer[32] = {0};
	char name_buffer[32 + 5] = {0};

	route = (struct rtnl_route *) nl_cache_get_first(routes_cache);

	while (route != NULL) {
		// fetch table name
		const int table_id = (int) rtnl_route_get_table(route);
		const uint8_t af = rtnl_route_get_family(route);
		rtnl_route_table2str(table_id, table_buffer, sizeof(table_buffer));
		snprintf(name_buffer, sizeof(name_buffer), "%s-%s", af == AF_INET ? "ipv4" : "ipv6", table_buffer);

		// add the table to the list and set properties
		rib_list_add(ribs, table_buffer, af);

		// default table is main (same as iproute2) - for both ipv4 and ipv6 addresses
		if (strncmp(table_buffer, "main", sizeof("main") - 1) == 0) {
			rib_list_set_default(ribs, table_buffer, af, 1);
		}

		route = (struct rtnl_route *) nl_cache_get_next((struct nl_object *) route);
	}

	// after the list is finished -> build descriptions for all ribs
	error = routing_build_rib_descriptions(ribs);
	if (error != 0) {
		goto error_out;
	}

	goto out;

error_out:
	SRP_LOG_ERR("error collecting RIBs: %d", error);

out:
	return error;
}

static int routing_collect_routes(struct nl_cache *routes_cache, struct rib_list *ribs)
{
	int error = 0;
	struct rtnl_route *route = NULL;
	struct route tmp_route = {0};
	char table_buffer[32] = {0};
	struct rib *tmp_rib = NULL;

	error = routing_collect_ribs(routes_cache, ribs);
	if (error != 0) {
		goto error_out;
	}

	// gather all routes for each table
	route = (struct rtnl_route *) nl_cache_get_first(routes_cache);
	while (route != NULL) {
		// fetch table name
		const int table_id = (int) rtnl_route_get_table(route);
		const uint8_t af = rtnl_route_get_family(route);
		rtnl_route_table2str(table_id, table_buffer, sizeof(table_buffer));

		// get the current RIB of the route
		tmp_rib = rib_list_get(ribs, table_buffer, af);
		if (tmp_rib == NULL) {
			error = -1;
			goto error_out;
		}

		// fill the route with info and add to the hash of the current RIB
		route_init(&tmp_route);
		route_set_preference(&tmp_route, rtnl_route_get_priority(route));

		// next-hop container -> TODO: see what about special type
		const int NEXTHOP_COUNT = rtnl_route_get_nnexthops(route);
		if (NEXTHOP_COUNT == 1) {
			struct rtnl_nexthop *nh = rtnl_route_nexthop_n(route, 0);
			route_next_hop_set_simple(&tmp_route.next_hop, rtnl_route_nh_get_ifindex(nh), rtnl_route_nh_get_gateway(nh));
		} else {
			rtnl_route_foreach_nexthop(route, foreach_nexthop, &tmp_route.next_hop);
		}

		// route-metadata/source-protocol
		if (rtnl_route_get_protocol(route) == RTPROT_STATIC) {
			route_set_source_protocol(&tmp_route, "ietf-routing:static");
		} else {
			route_set_source_protocol(&tmp_route, "ietf-routing:direct");
		}

		// add the created route to the hash by a destination address
		route_list_hash_add(&tmp_rib->routes, rtnl_route_get_dst(route), &tmp_route);

		// last-updated -> TODO: implement later
		route_free(&tmp_route);
		route = (struct rtnl_route *) nl_cache_get_next((struct nl_object *) route);
	}

	for (int i = 0; i < ribs->size; i++) {
		const struct route_list_hash *routes_hash = &ribs->list[i].routes;
		if (routes_hash->size) {
			// iterate every "hash" and set the active value for the preferred route
			for (int j = 0; j < routes_hash->size; j++) {
				struct route_list *ls = &routes_hash->list_route[j];
				if (ls->size > 0) {
					struct route *pref = &ls->list[0];
					for (int k = 1; k < ls->size; k++) {
						struct route *ptr = &ls->list[k];
						if (ptr->preference < pref->preference) {
							pref = ptr;
						}
					}
					pref->metadata.active = 1;
				}
			}
		}
	}

error_out:
	return error;
}

static int routing_load_control_plane_protocols(sr_session_ctx_t *session, struct lyd_node *routing_container_node)
{
	// error handling
	int error = 0;
	int nl_err = 0;

	// libyang
	const struct lys_module *ly_uv4mod = NULL, *ly_uv6mod = NULL;
	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *cpp_container_node = NULL;
	struct lyd_node *cpp_list_node = NULL;
	struct lyd_node *static_routes_container_node = NULL;
	struct lyd_node *tmp_node = NULL;
	struct lyd_node *ipv4_container_node = NULL, *ipv6_container_node = NULL;
	struct lyd_node *route_node = NULL;
	struct lyd_node *nh_node = NULL, *nh_list_node = NULL;

	// libnl
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct nl_cache *link_cache = NULL;
	struct rtnl_route *route = NULL;

	// temp buffers
	char list_buffer[PATH_MAX] = {0};
	char ip_buffer[INET6_ADDRSTRLEN] = {0};
	char prefix_buffer[INET6_ADDRSTRLEN + 1 + 3] = {0};
	char route_path_buffer[PATH_MAX] = {0};

	// control-plane-protocol structs
	struct control_plane_protocol cpp_map[ROUTING_PROTOS_COUNT] = {0};
	const struct control_plane_protocol *STATIC_PROTO = &cpp_map[RTPROT_STATIC];
	const struct route_list_hash *ipv4_static_routes = &STATIC_PROTO->ipv4_static.routes;
	const struct route_list_hash *ipv6_static_routes = &STATIC_PROTO->ipv6_static.routes;
	struct route tmp_route = {0};

	ly_ctx = sr_get_context(sr_session_get_connection(session));
	if (ly_ctx) {
		ly_uv4mod = ly_ctx_get_module(ly_ctx, "ietf-ipv4-unicast-routing", "2018-03-13", 0);
		ly_uv6mod = ly_ctx_get_module(ly_ctx, "ietf-ipv6-unicast-routing", "2018-03-13", 0);
	} else {
		SRP_LOG_ERR("unable to load external modules... exiting");
		error = -1;
		goto error_out;
	}

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERRMSG("unable to init nl_sock struct...");
		goto error_out;
	}

	nl_err = nl_connect(socket, NETLINK_ROUTE);
	if (nl_err != 0) {
		SRP_LOG_ERR("nl_connect failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_alloc_cache(socket, AF_UNSPEC, 0, &cache);
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_link_alloc_cache(socket, AF_UNSPEC, &link_cache);
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_link_alloc_cache failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_table_names("/etc/iproute2/rt_tables");
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	nl_err = rtnl_route_read_protocol_names("/etc/iproute2/rt_protos");
	if (nl_err != 0) {
		SRP_LOG_ERR("rtnl_route_read_table_names failed (%d): %s", nl_err, nl_geterror(nl_err));
		goto error_out;
	}

	// build protocols map array
	error = routing_build_protos_map(cpp_map);
	if (error) {
		SRP_LOG_ERR("unable to build protocols mapping array: %d", error);
		goto error_out;
	}

	route_list_hash_init((struct route_list_hash *) ipv4_static_routes);
	route_list_hash_init((struct route_list_hash *) ipv6_static_routes);

	// find all routes added statically and add them to the CPPs static-routes container
	route = (struct rtnl_route *) nl_cache_get_first(cache);
	while (route != NULL) {
		const int PROTO = rtnl_route_get_protocol(route);

		if (PROTO == RTPROT_STATIC) {
			const int AF = rtnl_route_get_family(route);
			// fill the route with info and add to the hash of the current RIB
			route_init(&tmp_route);
			route_set_preference(&tmp_route, rtnl_route_get_priority(route));

			// next-hop container -> TODO: see what about special type
			const int NEXTHOP_COUNT = rtnl_route_get_nnexthops(route);
			if (NEXTHOP_COUNT == 1) {
				struct rtnl_nexthop *nh = rtnl_route_nexthop_n(route, 0);
				route_next_hop_set_simple(&tmp_route.next_hop, rtnl_route_nh_get_ifindex(nh), rtnl_route_nh_get_gateway(nh));
			} else {
				rtnl_route_foreach_nexthop(route, foreach_nexthop, &tmp_route.next_hop);
			}

			// route-metadata/source-protocol
			route_set_source_protocol(&tmp_route, "ietf-routing:static");

			// add the route to the protocol's container
			if (AF == AF_INET) {
				// v4
				route_list_hash_add((struct route_list_hash *) ipv4_static_routes, rtnl_route_get_dst(route), &tmp_route);
			} else if (AF == AF_INET6) {
				// v6
				route_list_hash_add((struct route_list_hash *) ipv6_static_routes, rtnl_route_get_dst(route), &tmp_route);
			}

			route_free(&tmp_route);
		}
		route = (struct rtnl_route *) nl_cache_get_next((struct nl_object *) route);
	}

	cpp_container_node = lyd_new_path(routing_container_node, ly_ctx, ROUTING_CONTROL_PLANE_PROTOCOLS_CONTAINER_YANG_PATH, NULL, LYD_ANYDATA_STRING, 0);
	if (cpp_container_node == NULL) {
		SRP_LOG_ERR("unable to create control plane protocols container...");
		goto error_out;
	}

	// control plane protocols container created - add all protocols to the inner list

	// add the data to the datastore
	for (int i = 0; i < ROUTING_PROTOS_COUNT; i++) {
		const struct control_plane_protocol *proto = &cpp_map[i];
		if (proto->initialized) {
			// write proto path - type + name are added automatically when creating the list node
			snprintf(list_buffer, sizeof(list_buffer), "%s[type=\"%s\"][name=\"%s\"]", ROUTING_CONTROL_PLANE_PROTOCOL_LIST_YANG_PATH, proto->type, proto->name);
			cpp_list_node = lyd_new_path(cpp_container_node, ly_ctx, list_buffer, NULL, LYD_ANYDATA_CONSTSTRING, 0);
			if (cpp_list_node == NULL) {
				SRP_LOG_ERR("unable to add new control plane protocol %s", proto->name);
				goto error_out;
			}

			// description
			tmp_node = lyd_new_path(cpp_list_node, ly_ctx, "description", proto->description, LYD_ANYDATA_CONSTSTRING, 0);
			if (tmp_node == NULL) {
				SRP_LOG_ERR("unable to add 'description' node for the control plane protocol %s...", proto->name);
				goto error_out;
			}

			// static protocol -> static-routes
			if (strncmp(proto->name, "static", sizeof("static") - 1) == 0) {
				static_routes_container_node = lyd_new_path(cpp_list_node, ly_ctx, "static-routes", NULL, LYD_ANYDATA_STRING, 0);
				if (static_routes_container_node == NULL) {
					SRP_LOG_ERR("unable to add static-routes container node... exiting");
					goto error_out;
				}

				// for the static protocol add ipv4 and ipv6 static routes - each destination prefix => one route => one nexthop + description
				ipv4_container_node = lyd_new(static_routes_container_node, ly_uv4mod, "ipv4");
				if (ipv4_container_node == NULL) {
					SRP_LOG_ERR("unable to add ipv4 container node... exiting");
					goto error_out;
				}
				ipv6_container_node = lyd_new(static_routes_container_node, ly_uv6mod, "ipv6");
				if (ipv6_container_node == NULL) {
					SRP_LOG_ERR("unable to add ipv6 container node... exiting");
					goto error_out;
				}

				// ipv4
				for (int j = 0; j < ipv4_static_routes->size; j++) {
					const struct nl_addr *DST_PREFIX = ipv4_static_routes->list_addr[j];
					const struct route *ROUTE = &ipv4_static_routes->list_route[j].list[0];
					const union route_next_hop_value *NEXTHOP = &ROUTE->next_hop.value;

					// configure prefix
					nl_addr2str(DST_PREFIX, ip_buffer, sizeof(ip_buffer));
					if (strchr(ip_buffer, '/') == NULL) {
						snprintf(prefix_buffer, sizeof(prefix_buffer), "%s/%d", ip_buffer, nl_addr_get_prefixlen(DST_PREFIX));
					} else {
						snprintf(prefix_buffer, sizeof(prefix_buffer), "%s", ip_buffer);
					}

					// create new list node
					snprintf(route_path_buffer, sizeof(route_path_buffer), "route[destination-prefix=\"%s\"]", prefix_buffer);
					route_node = lyd_new_path(ipv4_container_node, ly_ctx, route_path_buffer, NULL, LYD_ANYDATA_STRING, 0);
					if (route_node == NULL) {
						SRP_LOG_ERR("unable to create a new route node for the list %s", route_path_buffer);
						goto error_out;
					}

					// description
					if (ROUTE->metadata.description != NULL) {
						tmp_node = lyd_new_leaf(route_node, ly_uv4mod, "description", ROUTE->metadata.description);
						if (tmp_node == NULL) {
							SRP_LOG_ERR("unable to add description leaf to the %s node", route_path_buffer);
							goto error_out;
						}
					}

					// next-hop container
					nh_node = lyd_new(route_node, ly_uv4mod, "next-hop");
					if (nh_node == NULL) {
						SRP_LOG_ERR("unable to create new next-hop container for the node %s", route_path_buffer);
						goto error_out;
					}
					switch (ROUTE->next_hop.kind) {
						case route_next_hop_kind_none:
							break;
						case route_next_hop_kind_simple: {
							// next-hop-address
							if (NEXTHOP->simple.addr) {
								nl_addr2str(NEXTHOP->simple.addr, ip_buffer, sizeof(ip_buffer));
								tmp_node = lyd_new_leaf(nh_node, ly_uv4mod, "next-hop-address", ip_buffer);
								if (tmp_node == NULL) {
									SRP_LOG_ERR("unable to create next-hop-address leaf for the node %s", route_path_buffer);
									goto error_out;
								}
							}
							break;
						}
						case route_next_hop_kind_special:
							break;
						case route_next_hop_kind_list: {
							const struct route_next_hop_list *NEXTHOP_LIST = &ROUTE->next_hop.value.list;
							nh_list_node = lyd_new_anydata(nh_node, ly_uv4mod, "next-hop-list/next-hop", NULL, LYD_ANYDATA_STRING);
							if (nh_list_node == NULL) {
								SRP_LOG_ERR("unable to create new next-hop-list/next-hop list for the node %s", route_path_buffer);
								goto error_out;
							}
							for (int k = 0; k < NEXTHOP_LIST->size; k++) {
								// next-hop-address
								if (NEXTHOP_LIST->list[k].addr) {
									nl_addr2str(NEXTHOP_LIST->list[k].addr, ip_buffer, sizeof(ip_buffer));
									tmp_node = lyd_new_leaf(nh_list_node, ly_uv4mod, "next-hop-address", ip_buffer);
									if (tmp_node == NULL) {
										SRP_LOG_ERR("unable to create next-hop-address leaf in the list for route %s", route_path_buffer);
										goto error_out;
									}
								}
							}
							break;
						}
					}
				}

				// ipv6
				for (int j = 0; j < ipv6_static_routes->size; j++) {
					const struct nl_addr *DST_PREFIX = ipv6_static_routes->list_addr[j];
					const struct route *ROUTE = &ipv6_static_routes->list_route[j].list[0];
					const union route_next_hop_value *NEXTHOP = &ROUTE->next_hop.value;

					// configure prefix
					nl_addr2str(DST_PREFIX, ip_buffer, sizeof(ip_buffer));
					if (strchr(ip_buffer, '/') == NULL) {
						snprintf(prefix_buffer, sizeof(prefix_buffer), "%s/%d", ip_buffer, nl_addr_get_prefixlen(DST_PREFIX));
					} else {
						snprintf(prefix_buffer, sizeof(prefix_buffer), "%s", ip_buffer);
					}

					// create new list node
					snprintf(route_path_buffer, sizeof(route_path_buffer), "route[destination-prefix=\"%s\"]", prefix_buffer);
					route_node = lyd_new_path(ipv6_container_node, ly_ctx, route_path_buffer, NULL, LYD_ANYDATA_STRING, 0);
					if (route_node == NULL) {
						SRP_LOG_ERR("unable to create a new route node for the list %s", route_path_buffer);
						goto error_out;
					}

					// description
					if (ROUTE->metadata.description != NULL) {
						tmp_node = lyd_new_leaf(route_node, ly_uv6mod, "description", ROUTE->metadata.description);
						if (tmp_node == NULL) {
							SRP_LOG_ERR("unable to add description leaf to the %s node", route_path_buffer);
							goto error_out;
						}
					}

					// next-hop container
					nh_node = lyd_new(route_node, ly_uv6mod, "next-hop");
					if (nh_node == NULL) {
						SRP_LOG_ERR("unable to create new next-hop container for the node %s", route_path_buffer);
						goto error_out;
					}
					switch (ROUTE->next_hop.kind) {
						case route_next_hop_kind_none:
							break;
						case route_next_hop_kind_simple: {
							// next-hop-address
							if (NEXTHOP->simple.addr) {
								nl_addr2str(NEXTHOP->simple.addr, ip_buffer, sizeof(ip_buffer));
								tmp_node = lyd_new_leaf(nh_node, ly_uv6mod, "next-hop-address", ip_buffer);
								if (tmp_node == NULL) {
									SRP_LOG_ERR("unable to create next-hop-address leaf for the node %s", route_path_buffer);
									goto error_out;
								}
							}
							break;
						}
						case route_next_hop_kind_special:
							break;
						case route_next_hop_kind_list: {
							const struct route_next_hop_list *NEXTHOP_LIST = &ROUTE->next_hop.value.list;
							nh_list_node = lyd_new_anydata(nh_node, ly_uv6mod, "next-hop-list/next-hop", NULL, LYD_ANYDATA_STRING);
							if (nh_list_node == NULL) {
								SRP_LOG_ERR("unable to create new next-hop-list/next-hop list for the node %s", route_path_buffer);
								goto error_out;
							}
							for (int k = 0; k < NEXTHOP_LIST->size; k++) {
								// next-hop-address
								if (NEXTHOP_LIST->list[k].addr) {
									nl_addr2str(NEXTHOP_LIST->list[k].addr, ip_buffer, sizeof(ip_buffer));
									tmp_node = lyd_new_leaf(nh_list_node, ly_uv6mod, "next-hop-address", ip_buffer);
									if (tmp_node == NULL) {
										SRP_LOG_ERR("unable to create next-hop-address leaf in the list for route %s", route_path_buffer);
										goto error_out;
									}
								}
							}
							break;
						}
					}
				}
			}
		}
	}

	goto out;

error_out:
	SRP_LOG_ERR("error loading initial data for control-plane-procotols container");
	error = 1;
out:
	// free all control plane protocol structs
	for (int i = 0; i < ROUTING_PROTOS_COUNT; i++) {
		control_plane_protocol_free(&cpp_map[i]);
	}
	nl_socket_free(socket);
	nl_cache_free(cache);
	nl_cache_free(link_cache);
	return error;
}

static int routing_build_rib_descriptions(struct rib_list *ribs)
{
	int error = 0;

	const char *data_dir = NULL;
	struct rib_description_pair default_descriptions[] = {
		{"ipv4-main", "main routing table - normal routing table containing all non-policy routes (ipv4 only)"},
		{"ipv6-main", "main routing table - normal routing table containing all non-policy routes (ipv6 only)"},
		{"ipv4-default", "default routing table - empty and reserved for post-processing if previous default rules did not select the packet (ipv4 only)"},
		{"ipv6-default", "default routing table - empty and reserved for post-processing if previous default rules did not select the packet (ipv6 only)"},
		{"ipv4-local", "local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv4 only)"},
		{"ipv6-local", "local routing table - maintained by the kernel, containing high priority control routes for local and broadcast addresses (ipv6 only)"},
	};
	struct {
		struct rib_description_pair *list;
		int size;
	} rib_descriptions = {0};

	// file stream
	FILE *fptr = NULL;

	// buffers and temporary values
	char path_buffer[PATH_MAX] = {0};
	char line_buffer[PATH_MAX] = {0};
	char name_buffer[32 + 5] = {0};
	struct rib_description_pair tmp_description = {0};

	// get the data_dir variable using getenv
	data_dir = getenv(ROUTING_PLUGIN_DATA_DIR);
	if (data_dir == NULL) {
		SRP_LOG_ERR("unable to use environment variable for the plugin data dir: %s", ROUTING_PLUGIN_DATA_DIR);
		goto error_out;
	}

	// create file path and see if it exists
	snprintf(path_buffer, sizeof(path_buffer), "%s%s%s", data_dir, (data_dir[strlen(data_dir) - 1] == '/') ? "" : "/", ROUTING_RIBS_DESCRIPTIONS_MAP_FNAME);
	SRP_LOG_DBG("RIBs description map file path: %s", path_buffer);

	if (access(path_buffer, F_OK) != 0) {
		// file doesn't exist - use default values and create the file for future use
		fptr = fopen(path_buffer, "w");
		if (fptr == NULL) {
			SRP_LOG_ERR("unable to open %s file for writing", path_buffer);
			goto error_out;
		}

		for (unsigned i = 0; i < sizeof(default_descriptions) / sizeof(default_descriptions[0]); i++) {
			rib_descriptions.list = xrealloc(rib_descriptions.list, sizeof(struct rib_description_pair) * (unsigned) (rib_descriptions.size + 1));
			memcpy(&rib_descriptions.list[rib_descriptions.size], &default_descriptions[i], sizeof(struct rib_description_pair));
			rib_descriptions.size += 1;

			fprintf(fptr, "%s\t\"%s\"\n", default_descriptions[i].name, default_descriptions[i].description);
		}
	} else {
		// file exists - use those values
		fptr = fopen(path_buffer, "r");
		if (fptr == NULL) {
			SRP_LOG_ERR("unable to open file %s", path_buffer);
			goto error_out;
		}

		while (fgets(line_buffer, sizeof(line_buffer), fptr) != NULL) {
			const int read_n = sscanf(line_buffer, "%s \"%256[^\"]\"", tmp_description.name, tmp_description.description);
			if (read_n == 2) {
				// add the description to the list
				rib_descriptions.list = xrealloc(rib_descriptions.list, sizeof(struct rib_description_pair) * (unsigned) (rib_descriptions.size + 1));
				memcpy(&rib_descriptions.list[rib_descriptions.size], &tmp_description, sizeof(struct rib_description_pair));
				rib_descriptions.size += 1;
			} else {
				SRP_LOG_ERR("unable to correctly read 2 values from the RIBs map file");
				goto error_out;
			}
		}
	}

	// now iterate over each rib and set its description
	for (int i = 0; i < ribs->size; i++) {
		const struct rib *RIB = &ribs->list[i];
		const int TABLE_ID = rtnl_route_str2table(RIB->name);

		// for default, local and main add prefixes, for other use given names
		if (routing_is_rib_known(TABLE_ID)) {
			snprintf(name_buffer, sizeof(name_buffer), "%s-%s", RIB->address_family == AF_INET ? "ipv4" : "ipv6", RIB->name);
		} else {
			snprintf(name_buffer, sizeof(name_buffer), "%s", RIB->name);
		}

		const size_t NAME_LEN = strlen(name_buffer);

		// set the description for each AF
		for (int j = 0; j < rib_descriptions.size; j++) {
			const struct rib_description_pair *PAIR = &rib_descriptions.list[j];
			if (strncmp(name_buffer, PAIR->name, NAME_LEN) == 0) {
				memcpy(((struct rib *) RIB)->description, PAIR->description, sizeof(PAIR->description));
				break;
			}
		}
	}

	goto out;

error_out:
	SRP_LOG_ERR("unable to build RIB descriptions");
	error = -1;

out:
	if (fptr != NULL) {
		fclose(fptr);
	}
	FREE_SAFE(rib_descriptions.list);

	return error;
}

static inline int routing_is_rib_known(int table)
{
	return table == RT_TABLE_DEFAULT || table == RT_TABLE_LOCAL || table == RT_TABLE_MAIN;
}

static int routing_build_protos_map(struct control_plane_protocol map[ROUTING_PROTOS_COUNT])
{
	int error = 0;

	// temp variables and buffers
	char *data_dir = NULL;
	char path_buffer[PATH_MAX] = {0};
	char line_buffer[PATH_MAX] = {0};
	int tmp_type = 0;
	char type_buffer[100] = {0};
	char desc_buffer[256] = {0};
	FILE *fptr = NULL;

	data_dir = getenv(ROUTING_PLUGIN_DATA_DIR);
	if (data_dir == NULL) {
		SRP_LOG_ERR("unable to use environment variable for the plugin data dir: %s", ROUTING_PLUGIN_DATA_DIR);
		goto error_out;
	}
	snprintf(path_buffer, sizeof(path_buffer), "%s%s%s", data_dir, (data_dir[strlen(data_dir) - 1] == '/' ? "" : "/"), ROUTING_PROTOS_MAP_FNAME);
	fptr = fopen(path_buffer, "r");
	if (fptr == NULL) {
		SRP_LOG_DBG("protocols map file doesnt exist - using only already known proto types and descriptions");
		// file doesn't exist -> build the initial map and write it to the file
		const char *known_types_map[ROUTING_PROTOS_COUNT] = {
			[RTPROT_UNSPEC] = "ietf-routing:direct",
			[RTPROT_REDIRECT] = "ietf-routing:direct",
			[RTPROT_KERNEL] = "ietf-routing:direct",
			[RTPROT_BOOT] = "ietf-routing:direct",
			[RTPROT_STATIC] = "ietf-routing:static",
			[RTPROT_GATED] = "ietf-routing:direct",
			[RTPROT_RA] = "ietf-routing:direct",
			[RTPROT_MRT] = "ietf-routing:direct",
			[RTPROT_ZEBRA] = "ietf-routing:direct",
			[RTPROT_BIRD] = "ietf-routing:direct",
			[RTPROT_DNROUTED] = "ietf-routing:direct",
			[RTPROT_XORP] = "ietf-routing:direct",
			[RTPROT_NTK] = "ietf-routing:direct",
			[RTPROT_DHCP] = "ietf-routing:direct",
			[RTPROT_MROUTED] = "ietf-routing:direct",
			[RTPROT_BABEL] = "ietf-routing:direct",
			[RTPROT_BGP] = "ietf-routing:direct",
			[RTPROT_ISIS] = "ietf-routing:direct",
			[RTPROT_OSPF] = "ietf-routing:direct",
			[RTPROT_RIP] = "ietf-routing:direct",
			[RTPROT_EIGRP] = "ietf-routing:direct",
		};

		const char *known_descr_map[ROUTING_PROTOS_COUNT] = {
			[RTPROT_UNSPEC] = "unspecified protocol",
			[RTPROT_REDIRECT] = "redirect protocol",
			[RTPROT_KERNEL] = "kernel protocol",
			[RTPROT_BOOT] = "boot protocol",
			[RTPROT_STATIC] = "static protocol",
			[RTPROT_GATED] = "gated protocol",
			[RTPROT_RA] = "ra protocol",
			[RTPROT_MRT] = "mrt protocol",
			[RTPROT_ZEBRA] = "zebra protocol",
			[RTPROT_BIRD] = "bird protocol",
			[RTPROT_DNROUTED] = "dnrouted protocol",
			[RTPROT_XORP] = "xorp protocol",
			[RTPROT_NTK] = "ntk protocol",
			[RTPROT_DHCP] = "dhcp protocol",
			[RTPROT_MROUTED] = "mrouted protocol",
			[RTPROT_BABEL] = "babel protocol",
			[RTPROT_BGP] = "bgp protocol",
			[RTPROT_ISIS] = "isis protocol",
			[RTPROT_OSPF] = "ospf protocol",
			[RTPROT_RIP] = "rip protocol",
			[RTPROT_EIGRP] = "eigrp protocol",
		};

		fptr = fopen(path_buffer, "w");
		if (fptr == NULL) {
			SRP_LOG_ERR("unable to open %s file for writing", ROUTING_PROTOS_MAP_FNAME);
			goto error_out;
		}

		// file opened - write protocol types into the file
		for (int i = 0; i < ROUTING_PROTOS_COUNT; i++) {
			if (routing_is_proto_type_known(i)) {
				rtnl_route_proto2str(i, map[i].name, sizeof(map[i].name));
				map[i].type = xstrdup(known_types_map[i]);
				map[i].description = xstrdup(known_descr_map[i]);
				map[i].initialized = 1;
				fprintf(fptr, "%d\t%s\t\"%s\"\n", i, map[i].type, map[i].description);
			}
		}
	} else {
		SRP_LOG_DBG("protocols map file exists - using map values");
		// file exists -> use map file values
		while (fgets(line_buffer, sizeof(line_buffer), fptr) != NULL) {
			const int read_n = sscanf(line_buffer, "%d %s \"%256[^\"]\"", &tmp_type, type_buffer, desc_buffer);
			if (read_n == 3) {
				if (tmp_type >= 0 && tmp_type <= ROUTING_PROTOS_COUNT) {
					rtnl_route_proto2str(tmp_type, map[tmp_type].name, sizeof(map[tmp_type].name));
					map[tmp_type].type = xstrdup(type_buffer);
					map[tmp_type].description = xstrdup(desc_buffer);
					map[tmp_type].initialized = 1;
				} else {
					SRP_LOG_ERR("invalid protocol type found in the protocol types map file: %d", tmp_type);
					goto error_out;
				}
			} else {
				SRP_LOG_ERR("unable to properly read the protocol types map file format; read %d values", read_n);
				goto error_out;
			}
		}
	}
	goto out;

error_out:
	error = -1;
out:
	if (fptr) {
		fclose(fptr);
	}
	return error;
}

static inline int routing_is_proto_type_known(int type)
{
	return type == RTPROT_UNSPEC ||
		   type == RTPROT_REDIRECT ||
		   type == RTPROT_KERNEL ||
		   type == RTPROT_BOOT ||
		   type == RTPROT_STATIC ||
		   type == RTPROT_GATED ||
		   type == RTPROT_RA ||
		   type == RTPROT_MRT ||
		   type == RTPROT_ZEBRA ||
		   type == RTPROT_BIRD ||
		   type == RTPROT_DNROUTED ||
		   type == RTPROT_XORP ||
		   type == RTPROT_NTK ||
		   type == RTPROT_DHCP ||
		   type == RTPROT_MROUTED ||
		   type == RTPROT_BABEL ||
		   type == RTPROT_BGP ||
		   type == RTPROT_ISIS ||
		   type == RTPROT_OSPF ||
		   type == RTPROT_RIP ||
		   type == RTPROT_EIGRP;
}

static bool routing_running_datastore_is_empty(void)
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
