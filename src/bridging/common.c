#include "common.h"

#include <string.h>

static void convert_mac_address(char *addr, char find, char replace);

void mac_address_ly_to_nl(char *addr)
{
	convert_mac_address(addr, '-', ':');
}

void mac_address_nl_to_ly(char *addr)
{
	convert_mac_address(addr, ':', '-');
}

static void convert_mac_address(char *addr, char find, char replace)
{
	char *tmp_ptr = addr;
	while ((tmp_ptr = strchr(tmp_ptr, find))) {
		*tmp_ptr = replace;
	}
}