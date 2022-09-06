#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* load api */
#include "plugin/api/interfaces/load.h"

/* init functionality */
static int setup(void **state);
static int teardown(void **state);

/* tests */

/* load */
static void test_correct_load_interface(void **state);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_correct_load_interface),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}

static int setup(void **state)
{
	interfaces_ctx_t *ctx = malloc(sizeof(interfaces_ctx_t));
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
		free(*state);
	}

	return 0;
}

static void test_correct_load_interface(void **state)
{
    interfaces_ctx_t *ctx = *state;
    int rc = 0;

    assert_int_equal(rc, 0);
}

