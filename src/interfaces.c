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
#include <utils/memory.h>
#include <string.h>
#include <netlink/socket.h>
#include <netlink/route/link.h>
#include <netlink/route/addr.h>
#include <netlink/route/tc.h>
#include <netlink/route/qdisc.h>
#include <netlink/netlink.h>
#include <libyang/libyang.h>
#include <linux/if.h>
#include <pthread.h>
#include <unistd.h>
#include <utils/if_state.h>
#include <time.h>
#include <sys/sysinfo.h>

#define LD_MAX_LINKS 100 // TODO: check this

typedef struct {
	char *name;
	char *description;
	char *type;
	char *enabled;
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
	"dummy",
};

#define BASE_YANG_MODEL "ietf-interfaces"

#define SYSREPOCFG_EMPTY_CHECK_COMMAND "sysrepocfg -X -d running -m " BASE_YANG_MODEL

// config data
#define INTERFACES_YANG_MODEL "/" BASE_YANG_MODEL ":interfaces"
#define INTERFACE_LIST_YANG_PATH INTERFACES_YANG_MODEL "/interface"

// other #defines
#define MAC_ADDR_MAX_LENGTH 17
#define DATETIME_BUF_SIZE 30

// callbacks
static int interfaces_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
static int interfaces_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

// helper functions
static bool system_running_datastore_is_empty_check(void);
static int load_data(sr_session_ctx_t *session);
static char *interfaces_xpath_get(const struct lyd_node *node);
int set_config_value(const char *xpath, const char *value);
int add_link_info(link_data_list_t *ld);

void link_init(link_data_t *l);
void link_set_name(link_data_t *l, char *name);
void link_data_list_init(link_data_list_t *ld);
int link_data_list_add(link_data_list_t *ld, char *name);
int link_data_list_set_description(link_data_list_t *ld, char *name, char *description);
int link_data_list_set_type(link_data_list_t *ld, char *name, char *type);
int link_data_list_set_enabled(link_data_list_t *ld, char *name, char *enabled);
void link_data_list_free(link_data_list_t *ld);
void link_data_free(link_data_t *l);

static int get_system_boot_time(char boot_datetime[]);

// function to start all threads for each interface
int init_state_changes(void);

// callback function for a thread to track state changes on a specific interface (ifindex passed using void* data param)
void *manager_thread_cb(void *data);
void cache_change_cb(struct nl_cache *cache, struct nl_object *obj, int val, void *arg);

// static list of interface states for tracking state changes using threads
static if_state_list_t if_state_changes;

// link manager used for cacheing links info constantly
static struct nl_cache_mngr *link_manager = NULL;
static struct nl_cache *link_cache = NULL;

volatile int exit_application = 0;

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *startup_session = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	*private_data = NULL;

	link_data_list_init(&link_data_list);

	SRP_LOG_INFMSG("start session to startup datastore");

	if_state_list_init(&if_state_changes);

	error = init_state_changes();
	if (error != 0) {
		SRP_LOG_ERR("Error occurred while initializing threads to track interface changes... exiting");
		goto out;
	}

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
	if_state_list_free(&if_state_changes);
	nl_cache_mngr_free(link_manager);

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
					// TODO: add deletetion of interface or interface data
				}
			}
			FREE_SAFE(node_xpath);
			node_value = NULL;
		}
		add_link_info(&link_data_list);
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
		error = link_data_list_set_description(&link_data_list, interface_node_name, (char *) value);
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
		error = link_data_list_set_enabled(&link_data_list, interface_node_name, (char *) value);
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

int add_link_info(link_data_list_t *ld)
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

		struct rtnl_link *old = rtnl_link_get_by_name(cache, name);
		struct rtnl_link *request = rtnl_link_alloc();

		// desc
		if (description != 0) {
			rtnl_link_set_ifalias(request, description);
		}

		// type
		if (type != 0) {
			error = rtnl_link_set_type(request, type);
			if (error < 0) {
				SRP_LOG_ERR("rtnl_link_set_type error (%d): %s", error, nl_geterror(error));
				goto out;
			}
		}

		// enabled
		if (enabled != 0) {
			if (strcmp(enabled, "true") == 0) {
				// set the interface to UP
				rtnl_link_set_flags(request, (unsigned int) rtnl_link_str2flags("up"));
				rtnl_link_set_operstate(request, IF_OPER_UP);
			} else {
				// set the interface to DOWN
				rtnl_link_unset_flags(request, (unsigned int) rtnl_link_str2flags("up"));
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
	l->name = 0;
	l->description = 0;
	l->type = 0;
	l->enabled = 0;
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
		if (strcmp(ld->links[i].name, name) == 0) {
			name_found = true;
			break;
		}
	}

	if (!name_found) {
		link_set_name(&ld->links[ld->count], name);
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
	struct nl_addr *addr = NULL;
	struct rtnl_tc *tc = NULL;
	struct rtnl_qdisc *qdisc = NULL;

	int32_t tmp_if_index = 0;
	uint64_t tmp_len = 0;
	struct rtnl_link *tmp_link = NULL;

	char tmp_buffer[PATH_MAX] = {0};
	char xpath_buffer[PATH_MAX] = {0};
	char interface_path_buffer[PATH_MAX] = {0};

	if_state_t *tmp_ifs = NULL;

	struct {
		char *name;
		char *description;
		char *type;
		char *enabled;
		char *link_up_down_trap_enable;
		char *admin_status;
		const char *oper_status;
		struct tm *last_change;
		int32_t if_index;
		char *phys_address;
		struct {
			char **data;
			uint32_t count;
		} higher_layer_if;
		struct {
			char **data;
			uint32_t count;
		} lower_layer_if;
		uint64_t speed;
		struct {
			char *discontinuity_time;
			uint64_t in_octets;
			uint64_t in_unicast_pkts;
			uint64_t in_broadcast_pkts;
			uint64_t in_multicast_pkts;
			uint32_t in_discards;
			uint32_t in_errors;
			uint32_t in_unknown_protos;
			uint64_t out_octets;
			uint64_t out_unicast_pkts;
			uint64_t out_broadcast_pkts;
			uint64_t out_multicast_pkts;
			uint32_t out_discards;
			uint32_t out_errors;
		} statistics;
	} interface_data = {0};

	const char *OPER_STRING_MAP[] = {
		[IF_OPER_UNKNOWN] = "unknown",
		[IF_OPER_NOTPRESENT] = "not-present",
		[IF_OPER_DOWN] = "down",
		[IF_OPER_LOWERLAYERDOWN] = "lower-layer-down",
		[IF_OPER_TESTING] = "testing",
		[IF_OPER_DORMANT] = "dormant",
		[IF_OPER_UP] = "up",
	};

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
	qdisc = rtnl_qdisc_alloc();

	while (link != NULL) {
		// get tc and set the link
		tc = TC_CAST(qdisc);
		rtnl_tc_set_link(tc, link);

		interface_data.name = rtnl_link_get_name(link);
		// interface_data.description = ?
		interface_data.type = rtnl_link_get_type(link);
		interface_data.enabled = rtnl_link_get_operstate(link) == IF_OPER_UP ? "enabled" : "disabled";
		// interface_data.link_up_down_trap_enable = ?
		// interface_data.admin_status = ?
		interface_data.oper_status = OPER_STRING_MAP[rtnl_link_get_operstate(link)];
		interface_data.if_index = rtnl_link_get_ifindex(link);

		// last-change field
		tmp_ifs = if_state_list_get_by_if_idx(&if_state_changes, interface_data.if_index);
		interface_data.last_change = (tmp_ifs->last_change != 0) ? localtime(&tmp_ifs->last_change) : NULL;

		// mac address
		addr = rtnl_link_get_addr(link);
		interface_data.phys_address = xmalloc(sizeof(char) * (MAC_ADDR_MAX_LENGTH + 1));
		nl_addr2str(addr, interface_data.phys_address, MAC_ADDR_MAX_LENGTH);
		interface_data.phys_address[MAC_ADDR_MAX_LENGTH] = 0;

		// higher-layer-if
		tmp_if_index = rtnl_link_get_master(link);
		while (tmp_if_index) {
			tmp_link = rtnl_link_get(cache, tmp_if_index);

			// append name to the list
			tmp_len = strlen(rtnl_link_get_name(tmp_link));
			interface_data.higher_layer_if.count++;
			interface_data.higher_layer_if.data = xrealloc(interface_data.higher_layer_if.data, sizeof(char *) * (interface_data.higher_layer_if.count));
			interface_data.higher_layer_if.data[interface_data.higher_layer_if.count - 1] = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(interface_data.higher_layer_if.data[interface_data.higher_layer_if.count - 1], rtnl_link_get_name(tmp_link), tmp_len);
			interface_data.higher_layer_if.data[interface_data.higher_layer_if.count - 1][tmp_len] = 0;

			tmp_if_index = rtnl_link_get_master(tmp_link);
		}

		// interface_data.lower_layer_if = ?
		interface_data.speed = rtnl_tc_get_stat(tc, RTNL_TC_RATE_BPS);

		// stats:
		char system_boot_time[DATETIME_BUF_SIZE] = {0};
		error = get_system_boot_time(system_boot_time);
		if (error) {
			SRP_LOG_ERR("get_system_boot_time error: %s", strerror(errno));
			goto error_out;
		}
		interface_data.statistics.discontinuity_time = system_boot_time;
		interface_data.statistics.in_octets = rtnl_link_get_stat(link, RTNL_LINK_IP6_INOCTETS);

		// to discuss
		interface_data.statistics.in_unicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_RX_PACKETS) - rtnl_link_get_stat(link, RTNL_LINK_IP6_INMCASTPKTS);
		// only this option involves broadcast packets => IP6 SNMP
		interface_data.statistics.in_broadcast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_INBCASTPKTS);

		interface_data.statistics.in_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_INMCASTPKTS);
		interface_data.statistics.in_discards = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_IP6_INDISCARDS);
		interface_data.statistics.in_errors = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_TX_ERRORS);
		interface_data.statistics.in_unknown_protos = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_IP6_INUNKNOWNPROTOS);

		interface_data.statistics.out_unicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_TX_PACKETS) - rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTMCASTPKTS);
		interface_data.statistics.out_broadcast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTBCASTPKTS);
		interface_data.statistics.out_multicast_pkts = rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTMCASTPKTS);
		interface_data.statistics.out_discards = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_IP6_OUTDISCARDS);
		interface_data.statistics.out_errors = (uint32_t) rtnl_link_get_stat(link, RTNL_LINK_RX_ERRORS);

		snprintf(interface_path_buffer, sizeof(interface_path_buffer) / sizeof(char), "%s[name=\"%s\"]", INTERFACE_LIST_YANG_PATH, rtnl_link_get_name(link));

		// name
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/name", interface_path_buffer);
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.name);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.name, LYD_ANYDATA_CONSTSTRING, 0);

		// type
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/type", interface_path_buffer);
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.type);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.type, LYD_ANYDATA_CONSTSTRING, 0);

		// oper-status
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/oper-status", interface_path_buffer);
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.oper_status);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, (char *) interface_data.oper_status, LYD_ANYDATA_CONSTSTRING, 0);

		// last-change -> only if changed at one point
		if (interface_data.last_change != NULL) {
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/last-change", interface_path_buffer);
			SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.type);
			strftime(tmp_buffer, sizeof tmp_buffer, "%FT%TZ", interface_data.last_change);
			lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);
		}

		// if-index
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/if-index", interface_path_buffer);
		SRP_LOG_DBG("%s = %d", xpath_buffer, interface_data.if_index);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.if_index);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// phys-address
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/phys-address", interface_path_buffer);
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.phys_address);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.phys_address, LYD_ANYDATA_CONSTSTRING, 0);

		// speed
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/speed", interface_path_buffer);
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.speed);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.speed);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// higher-layer-if
		for (uint64_t i = 0; i < interface_data.higher_layer_if.count; i++) {
			snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/higher-layer-if", interface_path_buffer);
			SRP_LOG_DBG("%s += %s", xpath_buffer, interface_data.higher_layer_if.data[i]);
			lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.higher_layer_if.data[i], LYD_ANYDATA_CONSTSTRING, 0);
		}

		// stats:
		// discontinuity-time
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/discontinuity-time", interface_path_buffer);
		SRP_LOG_DBG("%s = %s", xpath_buffer, interface_data.statistics.discontinuity_time);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, interface_data.statistics.discontinuity_time, LYD_ANYDATA_CONSTSTRING, 0);

		// in-octets
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-octets", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.in_octets);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// in-unicast-pkts
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-unicast-pkts", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.in_unicast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// in-broadcast-pkts
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-broadcast-pkts", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.in_broadcast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// in-multicast-pkts
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-multicast-pkts", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.in_multicast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// in-discards
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-discards", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.in_discards);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// in-errors
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-errors", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.in_errors);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// in-unknown-protos
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/in-unknown-protos", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.in_unknown_protos);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// out-octets
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-octets", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.out_octets);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// out-unicast-pkts
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-unicast-pkts", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.out_unicast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// out-broadcast-pkts
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-broadcast-pkts", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.out_broadcast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// out-multicast-pkts
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-multicast-pkts", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%lu", interface_data.statistics.out_multicast_pkts);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// out-discards
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-discards", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.out_discards);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// out-errors
		snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/statistics/out-errors", interface_path_buffer);
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%u", interface_data.statistics.out_errors);
		lyd_new_path(*parent, ly_ctx, xpath_buffer, tmp_buffer, LYD_ANYDATA_CONSTSTRING, 0);

		// free all allocated data
		free(interface_data.phys_address);

		// continue to next link node
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	rtnl_qdisc_put(qdisc);
	nl_cache_free(cache);

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	nl_socket_free(socket);
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int get_system_boot_time(char boot_datetime[])
{
	time_t now = 0;
	struct tm *ts = {0};
	struct sysinfo s_info = {0};
	time_t uptime_seconds = 0;

	now = time(NULL);

	ts = localtime(&now);
	if (ts == NULL)
		return -1;

	if (sysinfo(&s_info) != 0)
		return -1;

	uptime_seconds = s_info.uptime;

	time_t diff = now - uptime_seconds;

	ts = localtime(&diff);
	if (ts == NULL)
		return -1;

	/* must satisfy constraint (type yang:date-and-time):
		"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[\+\-]\d{2}:\d{2})"
		TODO: Add support for:
			- 2021-02-09T06:02:39.234+01:00
			- 2021-02-09T06:02:39.234Z
			- 2021-02-09T06:02:39+11:11
	*/

	strftime(boot_datetime, DATETIME_BUF_SIZE, "%FT%TZ", ts);

	return 0;
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

int init_state_changes(void)
{
	int error = 0;
	struct nl_sock *socket = NULL;
	struct nl_cache *cache = NULL;
	struct rtnl_link *link = NULL;
	if_state_t *tmp_st = NULL;
	pthread_attr_t attr;

	uint if_cnt = 0;

	struct {
		pthread_t *data;
		uint count;
	} thread_ls;

	pthread_t manager_thread;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, 1);

	socket = nl_socket_alloc();
	if (socket == NULL) {
		SRP_LOG_ERR("nl_socket_alloc error: invalid socket");
		return -1;
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
		++if_cnt;
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	// allocate a list to contain if_cnt number of interface states
	if_state_list_alloc(&if_state_changes, if_cnt);

	thread_ls.data = (pthread_t *) malloc(sizeof(pthread_t) * if_cnt);
	thread_ls.count = if_cnt;

	link = (struct rtnl_link *) nl_cache_get_first(cache);
	if_cnt = 0;

	while (link != NULL) {
		tmp_st = if_state_list_get(&if_state_changes, if_cnt);
		if (tmp_st) {
			tmp_st->state = rtnl_link_get_operstate(link);
			tmp_st->if_idx = rtnl_link_get_ifindex(link);
		}

		++if_cnt;
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}

	error = nl_cache_mngr_alloc(NULL, NETLINK_ROUTE, 0, &link_manager);
	if (error != 0) {
		SRP_LOG_ERR("nl_cache_mngr_alloc failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	error = nl_cache_mngr_add(link_manager, "route/link", cache_change_cb, NULL, &link_cache);
	if (error != 0) {
		SRP_LOG_ERR("nl_cache_mngr_add failed (%d): %s", error, nl_geterror(error));
		goto error_out;
	}

	pthread_create(&manager_thread, NULL, manager_thread_cb, 0);

	pthread_detach(manager_thread);

error_out:

	// clear libnl data
	nl_cache_free(cache);
	nl_socket_free(socket);

	// free tmp struct
	if (thread_ls.count) {
		free(thread_ls.data);
	}
	return error;
}

void cache_change_cb(struct nl_cache *cache, struct nl_object *obj, int val, void *arg)
{
	struct rtnl_link *link = NULL;
	int if_idx = 0;
	uint8_t tmp_state = 0;
	if_state_t *tmp_st = NULL;

	SRP_LOG_DBG("entered cb function for a link manager");

	link = (struct rtnl_link *) nl_cache_get_first(cache);

	while (link != NULL) {
		if_idx = rtnl_link_get_ifindex(link);
		tmp_st = if_state_list_get_by_if_idx(&if_state_changes, if_idx);
		tmp_state = rtnl_link_get_operstate(link);

		if (tmp_state != tmp_st->state) {
			SRP_LOG_DBG("Interface %d changed operstate from %d to %d", if_idx, tmp_st->state, tmp_state);
			tmp_st->state = tmp_state;
			tmp_st->last_change = time(0);
		}
		link = (struct rtnl_link *) nl_cache_get_next((struct nl_object *) link);
	}
}

void *manager_thread_cb(void *data)
{
	do {
		nl_cache_mngr_data_ready(link_manager);
	} while (true && exit_application == 0);

	return NULL;
}

#ifndef PLUGIN
#include <signal.h>
#include <unistd.h>

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

	pthread_exit(0);

	return error ? -1 : 0;
}

static void sigint_handler(__attribute__((unused)) int signum)
{
	SRP_LOG_INFMSG("Sigint called, exiting...");
	exit_application = 1;
}

#endif
