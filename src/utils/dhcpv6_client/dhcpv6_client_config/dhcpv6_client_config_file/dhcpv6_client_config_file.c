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

#include "dhcpv6_client_config_file.h"

extern int dhcpv6_client_list_add_entries(config_data_list_t *ccl, char *if_name, char *opt, char **opt_vals);

#define OPT_NUM 7

// supported dhcpv6 client configuration options
const char *config_opt_keywords[OPT_NUM] = {
	"send dhcp6.client-id",
	"request dhcp6.oro",
	"request dhcp6.rapid-commit",
	"send user-class",
	"send dhcp6.reconf-accept",
	"send dhcp6.vendor-class",
	"request dhcp6.vendor-opts"
};

int dhcpv6_client_config_file_read(config_data_list_t *ccl)
{
	// TODO: implement smarter dhclient.conf parser
	int error = 0;
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	char if_name[IFNAMSIZ] = {0}; //  IFNAMSIZ max length of interface name
	char *opt_line = NULL; // line containing relevant option
	char *opt = NULL; // just the option part, without the option values
	char *opt_vals[MAX_ORO_OPTS] = {0}; // option values; MAX_ORO_OPTS is used because it's enough to store all options with multiple values

	// check if file exists
	if (access(DHCLIENT_CONFIG_FILE, F_OK) != 0) {
		// if it doesn't, just return from this function like everything is normal
		SRP_LOG_INF("dhclient.conf file doesn't exist, skipping parsing at init");
		return 0;
	}

	fp = fopen(DHCLIENT_CONFIG_FILE, "r");

	if (fp == NULL) {
		SRP_LOG_ERR("fopen failed");
		goto error_out;
	}

	// iterate line by line through dhclient.conf
	while ((read = getline(&line, &len, fp)) != -1) {
		// TODO: remove multiple whitespaces from line
		// TODO: remove multiple newlines inside option strings

		// check if interface keyword in line
		if (strstr(line, "interface") != NULL) {
			// find "interface" keyword and get name of interface
			error = sscanf(line, "interface \"%16[^\"]\"", if_name); // IFNAMSIZ = 16
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}
		}

		// check for relevant config option keywords
		dhcpv6_client_config_file_check_entries(line, &opt_line, &opt);

		// check if we found a relevant option
		if (opt != NULL) {
			// parse the config option
			dhcpv6_client_config_file_parse_entries(opt_line, opt, opt_vals);
		}

		error = dhcpv6_client_list_add_entries(ccl, if_name, opt, opt_vals);
		if (error != 0) {
			SRP_LOG_ERR("dhcpv6_client_list_add_entries error");
			goto error_out;
		}

		if (opt != NULL) {
			FREE_SAFE(opt);
		}

		if (opt_line != NULL) {
			FREE_SAFE(opt_line);
		}

		for (int i = 0; i < MAX_ORO_OPTS; i++) {
			if (opt_vals[i] != NULL) {
				FREE_SAFE(opt_vals[i]);
			}
		}
	}

	fclose(fp);

	if (line != NULL) {
		FREE_SAFE(line);
	}

	return 0;

error_out:
	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (opt != NULL) {
		FREE_SAFE(opt);
	}

	if (opt_line != NULL) {
		FREE_SAFE(opt_line);
	}

	for (int i = 0; i < MAX_ORO_OPTS; i++) {
		if (opt_vals[i] != NULL) {
			FREE_SAFE(opt_vals[i]);
		}
	}

	return -1;
}

void dhcpv6_client_config_file_check_entries(char *line, char **opt_line, char **opt)
{
	for (int i = 0; i < OPT_NUM; i++) {
		// check if relevant option in line
		if (strstr(line, config_opt_keywords[i]) != NULL) {
			// save the whole line
			*opt_line = xstrdup(line);

			// just save the option part without values
			*opt = xstrdup(config_opt_keywords[i]);

			break;
		}
	}
}

int dhcpv6_client_config_file_parse_entries(char *opt_line, char *opt, char **opt_vals)
{
	int error = 0;
	//char tmp_ent_num[10] = {0}; // max num of chars for uint32
	//char tmp_vnd_id[10] = {0};
	char tmp_str[130] = {0}; // max duid len = 130, we'll use it for max user and vendor class strings as well...

	// strip it
	opt_line = strstrip(opt_line);

	if (strcmp(opt, "send dhcp6.client-id") == 0) {
		error = sscanf(opt_line, "send dhcp6.client-id \"%s", tmp_str);
		if (error == EOF) {
			SRP_LOG_ERR("sscanf error");
			goto error_out;
		}

		opt_vals[0] = xstrdup(tmp_str);

	} /*else if (strcmp(opt, "request dhcp6.oro") == 0) {
		// TODO: handle user class if string is multiple words (parse string between "")
		char *token = NULL;
		int cnt = 0;

		token = strtok(opt_line, " ,");

		do {
			// skip first two tokens since it will always be "send 'some_option'"
			// we are only intersted in the part after 'some_option'
			if (cnt > 1) {
				opt_vals[cnt-2] = xstrdup(token);
			}

			token = strtok(NULL, " ,");
			cnt++;
		} while (token != NULL); 


	}*/ else if (strcmp(opt, "send user-class") == 0) {
			error = sscanf(opt_line, "send user-class \"%130[^\"]\"", tmp_str);
		if (error == EOF) {
			SRP_LOG_ERR("sscanf error");
			goto error_out;
		}

		opt_vals[0] = xstrdup(tmp_str);

	} /*else if (strcmp(opt, "send dhcp6.vendor-class") == 0) {
		error = sscanf(opt_line, "send dhcp6.vendor-class %s %s \"%130[^\"]\"", tmp_ent_num, tmp_vnd_id, tmp_str);
		if (error == EOF) {
			SRP_LOG_ERR("sscanf error");
			goto error_out;
		}

		opt_vals[0] = xstrdup(tmp_ent_num);
		opt_vals[1] = xstrdup(tmp_vnd_id);
		opt_vals[2] = xstrdup(tmp_str);
	}*/

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_write_list(config_data_list_t ccl)
{
	int error = 0;
	bool if_del = false;

	for (uint32_t i = 0; i < MAX_IF_NUM; i++) {
		if (ccl.configs[i].if_name == NULL) { // skip deleted interfaces
			continue;
		}

		// first check if any interfaces from the list are already in the config file
		error = dhcpv6_client_config_file_check(ccl.configs[i].if_name, &if_del);
		if (error != 0) {
			SRP_LOG_ERR("dhcpv6_client_config_file_check error");
			goto error_out;
		}

		if (if_del == true || ccl.configs[i].del == true) { // also remove an interface if it scheduled for deletion
			// if so, these entries will be removed
			// because once this function is called all up-to-date data will be inside the list already
			// and then we can add the interfaces anew by calling dhcpv6_client_config_file_write for each interface
			error = dhcpv6_client_config_file_remove_entry(ccl.configs[i].if_name);
			if (error != 0) {
				SRP_LOG_ERR("dhcpv6_client_config_file_remove_entry error");
				goto error_out;
			}
		}

		// now write the entries for this specific interface in the config file
		// only if it's not scheduled for deletion...
		if (ccl.configs[i].del == false) {
			error = dhcpv6_client_config_file_write(ccl.configs[i]);
			if (error != 0) {
				SRP_LOG_ERR("dhcpv6_client_config_file_write error");
				goto error_out;
			}
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_check(char *if_name, bool *del)
{
	int error = 0;
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	char tmp_if_name[IFNAMSIZ] = {0}; //  IFNAMSIZ max length of interface name

	// check if file exists
	if (access(DHCLIENT_CONFIG_FILE, F_OK) != 0) {
		// if it doesn't, just return from this function like everything is normal
		SRP_LOG_INF("dhclient.conf file doesn't exist yet, skipping check if if_name in config");

		// there is nothing to delete yet...
		*del = false;
		return 0;
	}

	fp = fopen(DHCLIENT_CONFIG_FILE, "r");
	if (fp == NULL) {
		SRP_LOG_ERR("fopen failed");
		goto error_out;
	}

	// iterate line by line through dhclient.conf
	while ((read = getline(&line, &len, fp)) != -1) {
		// check if interface keyword in line
		if (strstr(line, "interface") != NULL) {
			// find "interface" keyword and get name of interface
			error = sscanf(line, "interface \"%16[^\"]\"", tmp_if_name); // IFNAMSIZ = 16
			if (error == EOF) {
				SRP_LOG_ERR("sscanf error");
				goto error_out;
			}
		}

		if (strlen(tmp_if_name) > 0 && strcmp(tmp_if_name, if_name) == 0) {
			// the interface name was found
			*del = true;
			break;
		} else {
			*del = false;
		}
	}

	FREE_SAFE(line);
	fclose(fp);

	return 0;

error_out:
	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (fp != NULL) {
		FREE_SAFE(fp);
	}

	return -1;
}

int dhcpv6_client_config_file_remove_entry(char *if_name)
{
	int error = 0;
	FILE *fp = NULL;
	FILE *tmp_fp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	char tmp_if_name[IFNAMSIZ] = {0}; //  IFNAMSIZ max length of interface name
	bool skip = false;

	tmp_fp = fopen(TMP_DHCLIENT_CONFIG_FILE, "w");
	if (tmp_fp == NULL) {
		SRP_LOG_ERR("fopen failed");
		goto error_out;
	}

	fp = fopen(DHCLIENT_CONFIG_FILE, "r");
	if (fp == NULL) {
		SRP_LOG_ERR("fopen failed");
		goto error_out;
	}

	// iterate line by line through dhclient.conf
	while ((read = getline(&line, &len, fp)) != -1) {
		// find "interface" keyword and get name of interface
		error = sscanf(line, "interface \"%16[^\"]\"", tmp_if_name); // IFNAMSIZ = 16
		if (error == EOF) {
			SRP_LOG_ERR("sscanf error");
			goto error_out;
		}

		if (strlen(tmp_if_name) > 0 && strcmp(tmp_if_name, if_name) == 0) {
			// interface found, now skip every line until "}" is reached
			skip = true;
		} else if (strstr(line, "}") == 0) {
			// if "}" is found, stop skipping lines
			skip = false;
		}

		if (skip == false) {
			fputs(line, tmp_fp); // place the line in a tmp file
		}
	}

	FREE_SAFE(line);
	fclose(tmp_fp);
	fclose(fp);

	// create a backup of the original config file
	error = rename(DHCLIENT_CONFIG_FILE, BAK_DHCLIENT_CONFIG_FILE);
	if (error != 0) {
		SRP_LOG_ERR("rename failed");
		goto error_out;
	}

	// rename the temp file to the original file
	error = rename(TMP_DHCLIENT_CONFIG_FILE, DHCLIENT_CONFIG_FILE);
	if (error != 0) {
		SRP_LOG_ERR("rename failed");
		goto error_out;
	}

	return 0;

error_out:
	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (tmp_fp != NULL) {
		FREE_SAFE(tmp_fp);
	}

	if (fp != NULL) {
		FREE_SAFE(fp);
	}

	return -1;
}

int dhcpv6_client_config_file_write(dhcpv6_client_config_t client_config)
{
	int error = 0;
	FILE * fp = NULL;

	// open dhclient.conf file
	fp = fopen (DHCLIENT_CONFIG_FILE, "a+");
	if (fp == NULL) {
		SRP_LOG_ERR("fopen failed");
		goto error_out;
	}

	// add the interface e.g.: interface "enp0s31f6" {
	error = dhcpv6_client_config_file_write_if_nam(client_config.if_name, fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_if_nam error");
		goto error_out;
	}

	// add DUID of interface
	error = dhcpv6_client_config_file_write_duid(client_config.duid, fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_duid error");
		goto error_out;
	}

	// add ORO e.g.: request dhcp6.oro // currently list is not supported 5, 6, 7
	error = dhcpv6_client_config_file_write_oro(client_config.config_opts.oro, fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_oro error");
		goto error_out;
	}

	// add rapid commit
	error = dhcpv6_client_config_file_write_rpd_cmt(client_config.config_opts.rapid_commit_opt, fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_rpd_cmt error");
		goto error_out;
	}

	// add user class, e.g.: send option user-class "test_user_class"
	error = dhcpv6_client_config_file_write_usr_cls(client_config.config_opts.user_class_opt, fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_usr_cls error");
		goto error_out;
	}

	// add reconf accept
	error = dhcpv6_client_config_file_write_rcn_acpt(client_config.config_opts.reconfigure_accept_opt, fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_rcn_acpt error");
		goto error_out;
	}

	// add vendor class opt
	/*error = dhcpv6_client_config_file_write_vnd_cls_opt(client_config.config_opts.vendor_class_opt, fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_vnd_cls_opt error");
		goto error_out;
	} */

	// add vendor specific opt
	error = dhcpv6_client_config_file_write_vnd_spec_opt(client_config.config_opts.vendor_specific_info_opts, fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_vnd_spec_opt error");
		goto error_out;
	}

	// add closing bracket for this interface
	error = dhcpv6_client_config_file_write_closing_brckt(fp);
	if (error != 0) {
		SRP_LOG_ERR("dhcpv6_client_config_file_write_closing_brckt error");
		goto error_out;
	}

	fclose(fp);

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_write_if_nam(char *if_name, FILE *fp)
{
	int error = 0;

	error = fprintf(fp, "interface \"%s\" {\n", if_name);
	if (error < 0) {
		goto error_out;
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_write_duid(char *duid, FILE *fp)
{
	int error = 0;

	if (duid != 0) {
		error = fprintf(fp, "\tsend dhcp6.client-id \"%s\";\n", duid);
		if (error < 0) {
			goto error_out;
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_write_oro(oro_options_t oro, FILE *fp)
 {
	int error = 0;
	//char *tmp_opts[MAX_ORO_OPTS] = {0};
	//uint32_t cnt = 0;

	if (oro.count > 0) {
		error = fprintf(fp, "\trequest dhcp6.oro;\n");
		if (error < 0) {
			goto error_out;
		}
	}

	/*// get all oro opt values
	for (uint32_t i = 0; i < MAX_ORO_OPTS; i++) {
		char *oro_val = oro.oro_options[i];

		if (oro_val != NULL) {
			tmp_opts[cnt] = xstrdup(oro_val);
			cnt++;
		}
	}

	for (uint32_t i = 0; i < cnt-1; i++) {
		error = fprintf(fp, "%s, ", tmp_opts[i]);
		if (error < 0) {
			goto error_out;
		}

		FREE_SAFE(tmp_opts[i]);
	}

	error = fprintf(fp, "%s;\n", tmp_opts[cnt-1]);
	if (error < 0) {
		goto error_out;
	}

	FREE_SAFE(tmp_opts[cnt-1]);
	*/

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_write_rpd_cmt(bool rapid_commit_opt, FILE *fp)
 {
	int error = 0;

	if (rapid_commit_opt == true) {
		error = fprintf(fp, "\trequest dhcp6.rapid-commit;\n");
		if (error < 0) {
			goto error_out;
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_write_usr_cls(user_class_option_t user_class, FILE *fp)
 {
	int error = 0;

	if (user_class.count > 0) {
		for (uint32_t i = 0; i < user_class.count; i++) {
			char *usr_class_dat = user_class.user_class_data_instances[i].user_class_data;

			error = fprintf(fp, "\tsend user-class \"%s\";\n", usr_class_dat);
			if (error < 0) {
				goto error_out;
			}
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_write_rcn_acpt(bool reconfigure_accept_opt, FILE *fp)
{
	int error = 0;

	if (reconfigure_accept_opt == true) {
		error = fprintf(fp, "\tsend dhcp6.reconf-accept;\n");
		if (error < 0) {
			goto error_out;
		}
	}

	return 0;

error_out:
	return -1;
}

/* dhcpv6_client_conf_save_vnd_cls_opt():
 *	only example of ipv6 dhclient.conf found on web:
 *		https://github.com/DentonGentry/gfiber-buildroot/blob/1eb1cd8ad3f60e8c8bf9ed6cd2b9477787a5b86c/fs/skeleton/etc/dhclient6.conf
 *
 *	from above example:
 *	"option dhcp6.vendor-class code 16 = {integer 32, integer 16, string};
 *	 send dhcp6.vendor-class 3561 12 "dslforum.org";
 *	 also request dhcp6.fqdn, dhcp6.sntp-servers, dhcp6.vendor-opts;"
 *
 *	based on this, the following applies in our case:
 *
 *		send dhcp6.vendor-class <enterprise-number> <vendor-class-data-id> <vendor-class-data>;
 *
 */
 /* dhcpv6_client_config_file_write_vnd_cls_opt(vendor_class_option_t
int dhcpv6_client_config_file_write_vnd_cls_opt(vendor_class_option_t vendor_class_opt, FILE *fp)
{
	int error = 0;

	if (vendor_class_opt.count > 0) {
		for (uint32_t i = 0; i < vendor_class_opt.count; i++) {
			vendor_class_option_instance_t vendor_instances = vendor_class_opt.vendor_class_option_instances[i];
			uint32_t enterprise_num = vendor_instances.enterprise_number;

			for (uint32_t j = 0; j < vendor_instances.count; j++) {
				vendor_class_t vendor_class = vendor_instances.vendor_class[j];
				uint8_t vendor_class_data_id = vendor_class.vendor_class_data_id;
				char *vendor_class_data = vendor_class.vendor_class_data;

				if (vendor_class_data != NULL) {
					error = fprintf(fp, "\tsend dhcp6.vendor-class %u %d \"%s\";\n", enterprise_num, vendor_class_data_id, vendor_class_data);
					if (error < 0) {
						goto error_out;
					}
				}
			}
		}
	}

	return 0;

error_out:
	return -1;
}
*/

/* dhcpv6_client_conf_save_vnd_spec_opt():
 * e.g. taken from dhcpd6.conf:
 * 	option dhcp6.vendor-opts
 *		00:00:11:8b: # enterprise-number 4491
 *			00:3d:00:20: # sub-option-code [61] len [32]
 *				fd:00:de:ad:00:01:00:00:00:00:00:00:00:00:00:01:  # sub-option-data
 *				07:d1:00:00:00:00:00:00:00:00:00:00:00:00:00:02:
 *
 *			00:22:00:20: # code [34] len [32]
 *				fd:00:de:ad:00:01:00:00:00:00:00:00:00:00:00:01:
 *				08:d1:00:00:00:00:00:00:00:00:00:00:00:00:00:02;
 *
 *	this example is for the server config file, and not the client config file...
 *
 *	In dhclient.conf we only use the following:
 *		request dhcp6.vendor-opts;
 */
int dhcpv6_client_config_file_write_vnd_spec_opt(vendor_specific_information_options_t vendor_opts, FILE *fp)
{
	int error = 0;

	if (vendor_opts.count > 0) {
		// TODO: check if client can send above example, if so implemented the example above
		//	   	 for now just request the vendor-opts from server
		error = fprintf(fp, "\trequest dhcp6.vendor-opts;\n");
		if (error < 0) {
			goto error_out;
		}
	}

	return 0;

error_out:
	return -1;
}

int dhcpv6_client_config_file_write_closing_brckt(FILE *fp)
{
	int error = 0;

	error = fprintf(fp, "}\n\n");
	if (error < 0) {
		goto error_out;
	}

	return 0;

error_out:
	return -1;
}

// TODO: move to utils.c
char *strstrip(char *s)
{
	// stolen from Linux kernel and modified to remove " and ; from s
	size_t size = 0;
	char *end = 0;
	int double_quote = 34; // " ascii integer
	int semicolon = 59; // ; ascii integer

	// check if double quote is at start of string
	if (s[0] == double_quote) {
		s++;
	}

	size = strlen(s);

	if (!size) {
		return s;
	}

	end = s + size - 1;
	while ((end >= s && isspace(*end)) || *end == double_quote || *end == semicolon) { // also remove double quote and ";"
		end--;
	}

	*(end + 1) = '\0';

	while (*s && isspace(*s)) {
		s++;
	}

	return s;
}