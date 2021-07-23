#ifndef DHCPV6_CLIENT_LIST_ONCE_H
#define DHCPV6_CLIENT_LIST_ONCE_H

#include "../../memory.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ORO_OPTS 10

#define MAX_USR_CLASS_INSTANCES 10

/* vendor class currently not supported in dhclient
#define MAX_VENDOR_CLASS_INSTANCES 10
#define MAX_VENDOR_CLASSES 10
*/

#define MAX_VENDOR_SPEC_INFO_INSTANCES 10
#define MAX_VENDOR_OPTS 10

#define MAX_IA_OPTS 10
#define MAX_PD_OPTS 10

#define MAX_IF_NUM 10

typedef struct client_config_opts_s client_config_opts_t;

typedef struct oro_options_s oro_options_t;

typedef struct status_code_option_s status_code_option_t;

typedef struct user_class_option_s user_class_option_t;
typedef struct user_class_data_instance_s user_class_data_instance_t;

/* vendor class currently not supported in dhclient
typedef struct vendor_class_option_s vendor_class_option_t;
typedef struct vendor_class_option_instance_s vendor_class_option_instance_t;
typedef struct vendor_class_s vendor_class_t;
*/

typedef struct vendor_specific_information_options_s vendor_specific_information_options_t;
typedef struct vendor_specific_information_option_s vendor_specific_information_option_t;
typedef struct vendor_option_data_s vendor_option_data_t;

typedef struct ia_na_opts_s ia_na_opts_t;
typedef struct ia_pd_opts_s ia_pd_opts_t;

typedef struct dhcpv6_client_config_s dhcpv6_client_config_t;

typedef struct config_data_list_s config_data_list_t;

struct status_code_option_s {
	uint32_t status_code;
	char *status_message;
};

struct ia_na_opts_s {
	uint32_t ia_id;
	uint32_t ia_na_options; // not used, for future use
};

struct ia_pd_opts_s {
	uint32_t ia_id;
	uint32_t ia_pd_options; // not used, for future use
};

struct oro_options_s {
	/* sending of oro is not currently supported by the dhclient
		we can only request them
		TODO: start disccussion on ISC mailing list because this does not make sense
		the client should be able to send the oro options
	char *oro_options[MAX_ORO_OPTS]; */
	uint32_t count;
};

struct user_class_data_instance_s {
	uint8_t user_class_data_id; // currently not used
	char *user_class_data;
};

struct user_class_option_s {
	user_class_data_instance_t user_class_data_instances[MAX_USR_CLASS_INSTANCES];
	uint32_t count;
};

/*
	// vendor class currently not supported in dhclient 
struct vendor_class_s {
	uint8_t vendor_class_data_id;
	char *vendor_class_data;
};

struct vendor_class_option_instance_s {
	uint32_t enterprise_number;
	vendor_class_t vendor_class[MAX_VENDOR_CLASSES];
	uint32_t count;
};

struct vendor_class_option_s {
	vendor_class_option_instance_t vendor_class_option_instances[MAX_VENDOR_CLASS_INSTANCES];
	uint32_t count;
};
*/

/* currently dhclient only supports requesting of the vendor-opts and not sending
struct vendor_option_data_s {
    uint16_t sub_opt_code;
    char *sub_opt_data;
};

struct vendor_specific_information_option_s {
	
	uint32_t enterprise_number;
	vendor_option_data_t vendor_option_data[MAX_VENDOR_OPTS];
	uint32_t count;
};
*/

struct vendor_specific_information_options_s {
	//vendor_specific_information_option_t vendor_specific_info_opt[MAX_VENDOR_SPEC_INFO_INSTANCES];
	uint32_t count;
};

struct client_config_opts_s {
	oro_options_t oro;
	status_code_option_t status_code_opt; /* currently not used */
	bool rapid_commit_opt;
	user_class_option_t user_class_opt;
	//vendor_class_option_t vendor_class_opt; /* currently not supported in dhclient */
	vendor_specific_information_options_t vendor_specific_info_opts;
	bool reconfigure_accept_opt;
};

struct dhcpv6_client_config_s {
	char *if_name;
	char *enabled;
	char *duid;
	client_config_opts_t config_opts;
	ia_na_opts_t ia_na[MAX_IA_OPTS];
	ia_pd_opts_t ia_pd[MAX_PD_OPTS];
	bool del;
};

struct config_data_list_s {
	dhcpv6_client_config_t configs[MAX_IF_NUM];
	uint8_t count;
};

int dhcpv6_client_list_add_entries(config_data_list_t *ccl, char *if_name, char *opt, char **opt_vals);
int dhcpv6_client_list_add_interface(config_data_list_t *ccl, char *if_name);
void dhcpv6_client_list_set_if_name(dhcpv6_client_config_t *client_config, char *if_name);
int dhcpv6_client_list_add_duid(config_data_list_t *ccl, char *if_name, char *duid);
int dhcpv6_client_list_add_oro(config_data_list_t *ccl, char *if_name, char *oro_opt);
int dhcpv6_client_list_add_rpd_cmt(config_data_list_t *ccl, char *if_name);
int dhcpv6_client_list_add_usr_cls(config_data_list_t *ccl, char *if_name, char *usr_cls);
int dhcpv6_client_list_add_rcn_acpt(config_data_list_t *ccl, char *if_name);
//int dhcpv6_client_list_add_vnd_cls_opt(config_data_list_t *ccl, char *if_name, char *ent_num, char *vnd_cls_id, char *vnd_cls_data);
//void dhcpv6_client_list_add_vnd_cls(dhcpv6_client_config_t *client_config, uint32_t inst_pos, uint8_t vendor_class_data_id, char *vnd_cls_data);
int dhcpv6_client_list_add_vnd_spec_opt(config_data_list_t *ccl, char *if_name);

int dhcpv6_client_list_set_del_if(config_data_list_t *ccl, char *if_name);
int dhcpv6_client_list_check_remove_interface(config_data_list_t *ccl);
int dhcpv6_client_list_remove_interface(config_data_list_t *ccl, char *if_name);
int dhcpv6_client_list_remove_duid(config_data_list_t *ccl, char *if_name, char *duid);
int dhcpv6_client_list_remove_oro(config_data_list_t *ccl, char *if_name, char *oro_opt);
int dhcpv6_client_list_remove_rpd_cmt(config_data_list_t *ccl, char *if_name);
int dhcpv6_client_list_remove_usr_cls(config_data_list_t *ccl, char *if_name, char *usr_cls);
int dhcpv6_client_list_remove_rcn_acpt(config_data_list_t *ccl, char *if_name);
int dhcpv6_client_list_remove_vnd_cls_opt(config_data_list_t *ccl, char *if_name, char *ent_num);
void dhcpv6_client_list_remove_vnd_cls(dhcpv6_client_config_t *client_config, uint32_t inst_pos);
//int dhcpv6_client_list_remove_vnd_cls_data(config_data_list_t *ccl, char *if_name, char *ent_num, char *vnd_cls_id, char *vnd_cls_data);
//int dhcpv6_client_list_remove_vnd_cls_id(config_data_list_t *ccl, char *if_name, char *ent_num, char *vnd_cls_id, char *vnd_cls_data);
int dhcpv6_client_list_remove_vnd_spec_opt(config_data_list_t *ccl, char *if_name);

//int dhcpv6_client_list_modify_oro(config_data_list_t *ccl, char *if_name, char *prev_oro_opt, char *oro_opt);
int dhcpv6_client_list_modify_usr_cls(config_data_list_t *ccl, char *if_name, char *prev_usr_cls, char *new_usr_cls);
//int dhcpv6_client_list_modify_vnd_cls_data(config_data_list_t *ccl, char *if_name, char *ent_num, char *vnd_cls_id, char *prev_vnd_cls_data, char *vnd_cls_data);

void dhcpv6_client_list_free(config_data_list_t *ccl);
void dhcpv6_client_list_free_config(dhcpv6_client_config_t *client_config);

dhcpv6_client_config_t *dhcpv6_client_list_get_client_config(config_data_list_t *ccl, char *if_name);

#endif /* DHCPV6_CLIENT_LIST_ONCE_H */