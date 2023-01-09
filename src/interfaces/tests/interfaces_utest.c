#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <cmocka.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* load api */
#include "plugin/api/interfaces/load.h"

/* interfaces hash table state */
#include "plugin/data/interfaces/interface/ipv4/address.h"
#include "plugin/data/interfaces/interface/ipv6/address.h"
#include "plugin/data/interfaces/interface_state.h"

/* interfaces interface linked list */
#include "plugin/data/interfaces/interface/linked_list.h"
#include "plugin/types.h"

/* init functionality */
static int setup(void** state);
static int teardown(void** state);

/* tests */

/** interface hash table state **/
static void test_state_hash_new_correct(void** state);
static void test_state_hash_element_new_correct(void** state);
static void test_state_hash_add_correct(void** state);
static void test_state_hash_add_incorrect(void** state);
static void test_state_hash_get_correct(void** state);
static void test_state_hash_get_incorrect(void** state);
static void test_state_hash_set_name_correct(void** state);
static void test_state_hash_set_name_incorrect(void** state);

/** interface list state **/
static void test_interface_list_new_correct(void** state);
static void test_interface_list_add_element_correct(void** state);
/*** ipv4 ***/
static void test_interface_list_new_ipv4_address_correct(void** state);
static void test_interface_list_new_ipv4_neighbor_correct(void** state);
static void test_interface_list_element_new_ipv4_address_correct(void** state);
static void test_interface_list_element_new_ipv4_neighbor_correct(void** state);
static void test_interface_ipv4_address_netmask2prefix_correct(void** state);
static void test_interface_ipv4_address_netmask2prefix_incorrect(void** state);

static void test_interface_ipv4_address_set_ip_correct(void** state);
static void test_interface_ipv4_address_set_prefix_length_correct(void** state);
static void test_interface_ipv4_address_set_netmask_correct(void** state);

/*** ipv6 ***/
static void test_interface_list_new_ipv6_address_correct(void** state);
static void test_interface_list_new_ipv6_neighbor_correct(void** state);
static void test_interface_list_element_new_ipv6_address_correct(void** state);
static void test_interface_list_element_new_ipv6_neighbor_correct(void** state);

static void test_interface_ipv6_address_set_ip_correct(void** state);
static void test_interface_ipv6_address_set_prefix_length_correct(void** state);

/** load **/
static void test_correct_load_interface(void** state);

int main(void)
{
    const struct CMUnitTest tests[] = {
        /** interface hash table state **/
        cmocka_unit_test(test_correct_load_interface),
        cmocka_unit_test(test_state_hash_new_correct),
        cmocka_unit_test(test_state_hash_element_new_correct),
        cmocka_unit_test(test_state_hash_add_correct),
        cmocka_unit_test(test_state_hash_add_incorrect),
        cmocka_unit_test(test_state_hash_get_correct),
        cmocka_unit_test(test_state_hash_get_incorrect),
        cmocka_unit_test(test_state_hash_set_name_correct),
        cmocka_unit_test(test_state_hash_set_name_incorrect),
        /** interface list state **/
        cmocka_unit_test(test_interface_list_new_correct),
        cmocka_unit_test(test_interface_list_add_element_correct),
        /*** ipv4 ***/
        cmocka_unit_test(test_interface_list_new_ipv4_address_correct),
        cmocka_unit_test(test_interface_list_new_ipv4_neighbor_correct),
        cmocka_unit_test(test_interface_list_element_new_ipv4_address_correct),
        cmocka_unit_test(test_interface_list_element_new_ipv4_neighbor_correct),
        cmocka_unit_test(test_interface_ipv4_address_netmask2prefix_correct),
        cmocka_unit_test(test_interface_ipv4_address_netmask2prefix_incorrect),
        cmocka_unit_test(test_interface_ipv4_address_set_ip_correct),
        cmocka_unit_test(test_interface_ipv4_address_set_prefix_length_correct),
        cmocka_unit_test(test_interface_ipv4_address_set_netmask_correct),
        /*** ipv6 ***/
        cmocka_unit_test(test_interface_list_new_ipv6_address_correct),
        cmocka_unit_test(test_interface_list_new_ipv6_neighbor_correct),
        cmocka_unit_test(test_interface_list_element_new_ipv6_address_correct),
        cmocka_unit_test(test_interface_list_element_new_ipv6_neighbor_correct),
        cmocka_unit_test(test_interface_ipv6_address_set_ip_correct),
        cmocka_unit_test(test_interface_ipv6_address_set_prefix_length_correct),
    };

    return cmocka_run_group_tests(tests, setup, teardown);
}

static int setup(void** state)
{
    interfaces_ctx_t* ctx = malloc(sizeof(interfaces_ctx_t));
    if (!ctx) {
        return -1;
    }

    *ctx = (interfaces_ctx_t) { 0 };
    *state = ctx;

    return 0;
}

static int teardown(void** state)
{
    if (*state) {
        free(*state);
    }

    return 0;
}

static void test_correct_load_interface(void** state)
{
    (void)state;

    int rc = 0;

    assert_int_equal(rc, 0);
}

static void test_state_hash_new_correct(void** state)
{
    (void)state;

    interfaces_interface_state_hash_element_t* state_hash;

    state_hash = interfaces_interface_state_hash_new();
    assert_null(state_hash);
}

static void test_state_hash_element_new_correct(void** state)
{
    (void)state;

    interfaces_interface_state_hash_element_t* new_elem;

    new_elem = interfaces_interface_state_hash_element_new();
    assert_non_null(new_elem);

    interfaces_interface_state_hash_element_free(&new_elem);
    assert_null(new_elem);
}

static void test_state_hash_add_correct(void** state)
{
    (void)state;

    int rc = 0;

    interfaces_interface_state_hash_element_t *state_hash, *new_elem;

    state_hash = interfaces_interface_state_hash_new();
    assert_null(state_hash);

    new_elem = interfaces_interface_state_hash_element_new();

    interfaces_interface_state_hash_element_set_name(&new_elem, "FOO");
    assert_int_equal(rc, 0);

    rc = interfaces_interface_state_hash_add(&state_hash, new_elem);
    assert_int_equal(rc, 0);

    interfaces_interface_state_hash_free(&state_hash);
}

static void test_state_hash_add_incorrect(void** state)
{
    (void)state;

    int rc = 0;

    interfaces_interface_state_hash_element_t *state_hash, *new_elem;

    state_hash = interfaces_interface_state_hash_new();
    assert_null(state_hash);

    new_elem = interfaces_interface_state_hash_element_new();

    interfaces_interface_state_hash_element_set_name(&new_elem, "FOO");
    assert_int_equal(rc, 0);

    rc = interfaces_interface_state_hash_add(&state_hash, new_elem);
    assert_int_equal(rc, 0);

    /* try adding the same element */
    rc = interfaces_interface_state_hash_add(&state_hash, new_elem);
    assert_int_not_equal(rc, 0);

    interfaces_interface_state_hash_free(&state_hash);
}

static void test_state_hash_get_correct(void** state)
{
    (void)state;

    int rc = 0;
    const char* name = "FOO";

    interfaces_interface_state_hash_element_t *state_hash, *new_elem, *found;

    state_hash = interfaces_interface_state_hash_new();
    assert_null(state_hash);

    new_elem = interfaces_interface_state_hash_element_new();

    interfaces_interface_state_hash_element_set_name(&new_elem, name);
    assert_int_equal(rc, 0);

    rc = interfaces_interface_state_hash_add(&state_hash, new_elem);
    assert_int_equal(rc, 0);

    found = interfaces_interface_state_hash_get(state_hash, name);
    assert_non_null(found);

    interfaces_interface_state_hash_free(&state_hash);
}

static void test_state_hash_get_incorrect(void** state)
{
    (void)state;

    int rc = 0;
    const char* names[] = { "FOO", "BAR" };

    interfaces_interface_state_hash_element_t *state_hash, *new_elem, *found;

    state_hash = interfaces_interface_state_hash_new();
    assert_null(state_hash);

    new_elem = interfaces_interface_state_hash_element_new();

    interfaces_interface_state_hash_element_set_name(&new_elem, names[0]);
    assert_int_equal(rc, 0);

    rc = interfaces_interface_state_hash_add(&state_hash, new_elem);
    assert_int_equal(rc, 0);

    found = interfaces_interface_state_hash_get(state_hash, names[1]);
    assert_null(found);

    interfaces_interface_state_hash_free(&state_hash);
}

static void test_state_hash_set_name_correct(void** state)
{
    (void)state;

    int rc = 0;

    interfaces_interface_state_hash_element_t* new_elem;

    new_elem = interfaces_interface_state_hash_element_new();

    rc = interfaces_interface_state_hash_element_set_name(&new_elem, "FOO");
    assert_int_equal(rc, 0);
    assert_non_null(new_elem->state.name);

    interfaces_interface_state_hash_element_free(&new_elem);
}

static void test_state_hash_set_name_incorrect(void** state)
{
    (void)state;

    int rc = 0;

    interfaces_interface_state_hash_element_t* new_elem;

    new_elem = interfaces_interface_state_hash_element_new();

    rc = interfaces_interface_state_hash_element_set_name(&new_elem, NULL);
    assert_int_equal(rc, 0);
    assert_null(new_elem->state.name);

    interfaces_interface_state_hash_element_free(&new_elem);
}

static void test_interface_list_new_correct(void** state)
{
    (void)state;
    interfaces_interface_ipv4_address_element_t* address_ipv4;
    interfaces_interface_ipv4_neighbor_element_t* neighbor_ipv4;

    interfaces_interface_ipv6_address_element_t* address_ipv6;
    interfaces_interface_ipv6_neighbor_element_t* neighbor_ipv6;

    INTERFACES_INTERFACE_LIST_NEW(address_ipv4);
    assert_null(address_ipv4);

    INTERFACES_INTERFACE_LIST_NEW(neighbor_ipv4);
    assert_null(neighbor_ipv4);

    INTERFACES_INTERFACE_LIST_NEW(address_ipv6);
    assert_null(address_ipv6);

    INTERFACES_INTERFACE_LIST_NEW(neighbor_ipv6);
    assert_null(neighbor_ipv6);
}

static void test_interface_list_add_element_correct(void** state)
{
    (void)state;
    interfaces_interface_ipv4_address_element_t* address_ipv4;
    interfaces_interface_ipv4_address_element_t* address_elem_ipv4;

    interfaces_interface_ipv4_neighbor_element_t* neighbor_ipv4;
    interfaces_interface_ipv4_neighbor_element_t* neighbor_elem_ipv4;

    interfaces_interface_ipv6_address_element_t* address_ipv6;
    interfaces_interface_ipv6_address_element_t* address_elem_ipv6;

    interfaces_interface_ipv6_neighbor_element_t* neighbor_ipv6;
    interfaces_interface_ipv6_neighbor_element_t* neighbor_elem_ipv6;

    INTERFACES_INTERFACE_LIST_NEW(address_ipv4);
    assert_null(address_ipv4);

    INTERFACES_INTERFACE_LIST_NEW(neighbor_ipv4);
    assert_null(neighbor_ipv4);

    INTERFACES_INTERFACE_LIST_NEW(address_ipv6);
    assert_null(address_ipv6);

    INTERFACES_INTERFACE_LIST_NEW(neighbor_ipv6);
    assert_null(neighbor_ipv6);

    address_elem_ipv4 = interfaces_interface_ipv4_address_new();
    assert_null(address_elem_ipv4);
    address_elem_ipv4 = interfaces_interface_ipv4_address_element_new();
    assert_non_null(address_elem_ipv4);
    INTERFACES_INTERFACE_LIST_ADD_ELEMENT(address_ipv4, address_elem_ipv4);
    assert_non_null(address_ipv4);
    INTERFACES_INTERFACE_LIST_FREE(address_ipv4);
    assert_null(address_ipv4);

    neighbor_elem_ipv4 = interfaces_interface_ipv4_neighbor_new();
    assert_null(neighbor_elem_ipv4);
    neighbor_elem_ipv4 = interfaces_interface_ipv4_neighbor_element_new();
    assert_non_null(neighbor_elem_ipv4);
    INTERFACES_INTERFACE_LIST_ADD_ELEMENT(neighbor_ipv4, neighbor_elem_ipv4);
    assert_non_null(neighbor_ipv4);
    INTERFACES_INTERFACE_LIST_FREE(neighbor_ipv4);
    assert_null(neighbor_ipv4);

    address_elem_ipv6 = interfaces_interface_ipv6_address_new();
    assert_null(address_elem_ipv6);
    address_elem_ipv6 = interfaces_interface_ipv6_address_element_new();
    assert_non_null(address_elem_ipv6);
    INTERFACES_INTERFACE_LIST_ADD_ELEMENT(address_ipv6, address_elem_ipv6);
    assert_non_null(address_ipv6);
    INTERFACES_INTERFACE_LIST_FREE(address_ipv6);
    assert_null(address_ipv6);

    neighbor_elem_ipv6 = interfaces_interface_ipv6_neighbor_new();
    assert_null(neighbor_elem_ipv6);
    neighbor_elem_ipv6 = interfaces_interface_ipv6_neighbor_element_new();
    assert_non_null(neighbor_elem_ipv6);
    INTERFACES_INTERFACE_LIST_ADD_ELEMENT(neighbor_ipv6, neighbor_elem_ipv6);
    assert_non_null(neighbor_ipv6);
    INTERFACES_INTERFACE_LIST_FREE(neighbor_ipv6);
    assert_null(neighbor_ipv6);
}

static void test_interface_list_new_ipv4_address_correct(void** state)
{
    (void)state;

    interfaces_interface_ipv4_address_element_t* address;

    address = interfaces_interface_ipv4_address_new();
    assert_null(address);
}

static void test_interface_list_new_ipv4_neighbor_correct(void** state)
{
    (void)state;

    interfaces_interface_ipv4_neighbor_element_t* neighbor;

    neighbor = interfaces_interface_ipv4_neighbor_new();
    assert_null(neighbor);
}

static void test_interface_list_element_new_ipv4_address_correct(void** state)
{
    (void)state;

    interfaces_interface_ipv4_address_element_t* address;

    address = interfaces_interface_ipv4_address_new();
    assert_null(address);

    address = interfaces_interface_ipv4_address_element_new();
    assert_non_null(address);

    interfaces_interface_ipv4_address_element_free(&address);
    assert_null(address);
}

static void test_interface_list_element_new_ipv4_neighbor_correct(void** state)
{
    (void)state;

    interfaces_interface_ipv4_neighbor_element_t* neighbor;

    neighbor = interfaces_interface_ipv4_neighbor_new();
    assert_null(neighbor);

    neighbor = interfaces_interface_ipv4_neighbor_element_new();
    assert_non_null(neighbor);

    interfaces_interface_ipv4_neighbor_element_free(&neighbor);
    assert_null(neighbor);
}

static void test_interface_ipv4_address_netmask2prefix_correct(void** state)
{
    (void)state;

    int rc = -1;
    uint8_t prefix_length = 0;
    const char* netmask = "255.255.255.0";

    rc = interfaces_interface_ipv4_address_netmask2prefix(netmask, &prefix_length);
    assert_int_equal(rc, 0);
    assert_int_equal(prefix_length, 24);
}

static void test_interface_ipv4_address_netmask2prefix_incorrect(void** state)
{
    (void)state;

    int rc = 0;
    uint8_t prefix_length = 0;
    const char* netmask = "FOOBAR";

    rc = interfaces_interface_ipv4_address_netmask2prefix(netmask, &prefix_length);
    assert_int_not_equal(rc, 0);
}

static void test_interface_ipv4_address_set_ip_correct(void** state)
{
    (void)state;
    const char* ip = "127.0.0.1";

    interfaces_interface_ipv4_address_element_t* address;

    address = interfaces_interface_ipv4_address_new();
    assert_null(address);

    address = interfaces_interface_ipv4_address_element_new();
    assert_non_null(address);

    interfaces_interface_ipv4_address_element_set_ip(&address, ip);
    assert_string_equal(address->address.ip, ip);
}

static void test_interface_ipv4_address_set_prefix_length_correct(void** state)
{
    (void)state;
    uint8_t prefix_length = 16;

    interfaces_interface_ipv4_address_element_t* address;

    address = interfaces_interface_ipv4_address_new();
    assert_null(address);

    address = interfaces_interface_ipv4_address_element_new();
    assert_non_null(address);

    interfaces_interface_ipv4_address_element_set_prefix_length(&address, prefix_length);
    assert_int_equal(address->address.subnet.prefix_length, prefix_length);
    assert_int_equal(address->address.subnet_type, interfaces_interface_ipv4_address_subnet_prefix_length);
}

static void test_interface_ipv4_address_set_netmask_correct(void** state)
{
    (void)state;
    int rc = 0;
    const char* netmask = "255.255.0.0";

    interfaces_interface_ipv4_address_element_t* address;

    address = interfaces_interface_ipv4_address_new();
    assert_null(address);

    address = interfaces_interface_ipv4_address_element_new();
    assert_non_null(address);

    rc = interfaces_interface_ipv4_address_element_set_netmask(&address, netmask);
    assert_int_equal(rc, 0);
    assert_string_equal(address->address.subnet.netmask, netmask);
    assert_int_equal(address->address.subnet_type, interfaces_interface_ipv4_address_subnet_netmask);
}

static void test_interface_list_new_ipv6_address_correct(void** state)
{
    (void)state;

    interfaces_interface_ipv6_address_element_t* address;

    address = interfaces_interface_ipv6_address_new();
    assert_null(address);
}

static void test_interface_list_new_ipv6_neighbor_correct(void** state)
{
    (void)state;

    interfaces_interface_ipv6_neighbor_element_t* neighbor;

    neighbor = interfaces_interface_ipv6_neighbor_new();
    assert_null(neighbor);
}

static void test_interface_list_element_new_ipv6_address_correct(void** state)
{
    (void)state;

    interfaces_interface_ipv6_address_element_t* address;

    address = interfaces_interface_ipv6_address_new();
    assert_null(address);

    address = interfaces_interface_ipv6_address_element_new();
    assert_non_null(address);

    interfaces_interface_ipv6_address_element_free(&address);
    assert_null(address);
}

static void test_interface_list_element_new_ipv6_neighbor_correct(void** state)
{
    (void)state;

    interfaces_interface_ipv6_neighbor_element_t* neighbor;

    neighbor = interfaces_interface_ipv6_neighbor_new();
    assert_null(neighbor);

    neighbor = interfaces_interface_ipv6_neighbor_element_new();
    assert_non_null(neighbor);

    interfaces_interface_ipv6_neighbor_element_free(&neighbor);
    assert_null(neighbor);
}

static void test_interface_ipv6_address_set_ip_correct(void** state)
{
    (void)state;
    const char* ip = "0:0:0:0:0:0:0:1";

    interfaces_interface_ipv6_address_element_t* address;

    address = interfaces_interface_ipv6_address_new();
    assert_null(address);

    address = interfaces_interface_ipv6_address_element_new();
    assert_non_null(address);

    interfaces_interface_ipv6_address_element_set_ip(&address, ip);
    assert_string_equal(address->address.ip, ip);
}

static void test_interface_ipv6_address_set_prefix_length_correct(void** state)
{
    (void)state;
    uint8_t prefix_length = 64;

    interfaces_interface_ipv6_address_element_t* address;

    address = interfaces_interface_ipv6_address_new();
    assert_null(address);

    address = interfaces_interface_ipv6_address_element_new();
    assert_non_null(address);

    interfaces_interface_ipv6_address_element_set_prefix_length(&address, prefix_length);
    assert_int_equal(address->address.prefix_length, prefix_length);
}
