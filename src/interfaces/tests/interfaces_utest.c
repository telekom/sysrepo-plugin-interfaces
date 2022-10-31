#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* xmalloc, FREE_SAFE */
#include "utils/memory.h"

/* load api */
#include "plugin/api/interfaces/load.h"

/* interfaces hash table state */
#include "plugin/data/interfaces/interface_state.h"

/* interfaces interface linked list */
#include "plugin/data/interfaces/interface/linked_list.h"

/* init functionality */
static int setup(void **state);
static int teardown(void **state);

/* tests */

/** interface hash table state **/
static void test_hash_new_correct(void **state);

/* load */
static void test_correct_load_interface(void **state);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_correct_load_interface),
		cmocka_unit_test(test_hash_new_correct),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}

static int setup(void **state)
{
	interfaces_ctx_t *ctx = xmalloc(sizeof(interfaces_ctx_t));
	if (!ctx) {
		return -1;
	}

	*ctx = (interfaces_ctx_t){0};
	*state = ctx;

	return 0;
}

static int teardown(void **state)
{
	if (*state) {
		FREE_SAFE(*state);
	}

	return 0;
}

static void test_correct_load_interface(void **state)
{
    interfaces_ctx_t *ctx = *state;
    int rc = 0;

    assert_int_equal(rc, 0);
}

static void test_hash_new_correct(void **state)
{
    interfaces_ctx_t *ctx = *state;
    int rc = 0;

    interfaces_interface_state_hash_element_t *if_root; 

    if_root = interfaces_interface_state_hash_new();

    assert_null(if_root);

    assert_int_equal(rc, 0);
}

