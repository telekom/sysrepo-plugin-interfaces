#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static void test_correct_routing(void **state);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_correct_routing),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

static void test_correct_routing(void **state)
{
}