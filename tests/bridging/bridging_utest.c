#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <cmocka.h>

#include "plugin/common.h"

static void test_correct_mac_ly_to_nl(void **state);
static void test_correct_mac_nl_to_ly(void **state);

struct mac_address_pair {
	char mac[18];
	char expected[18];
};

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_correct_mac_ly_to_nl),
		cmocka_unit_test(test_correct_mac_nl_to_ly),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

static void test_correct_mac_ly_to_nl(void **state)
{
	struct mac_address_pair pairs[] = {
		{
			"2c-54-91-88-c9-e3",
			"2c:54:91:88:c9:e3",
		},
		{
			"2d-53-81-a8-c1-e2",
			"2d:53:81:a8:c1:e2",
		},
		{
			"2d-23-61-a9-c1-e2",
			"2d:23:61:a9:c1:e2",
		},
	};

	for (size_t i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++) {
		// convert
		mac_address_ly_to_nl(pairs[i].mac);

		// check
		assert_string_equal(pairs[i].mac, pairs[i].expected);
	}
}

static void test_correct_mac_nl_to_ly(void **state)
{
	struct mac_address_pair pairs[] = {
		{
			"2c:54:91:88:c9:e3",
			"2c-54-91-88-c9-e3",
		},
		{
			"2d:53:81:a8:c1:e2",
			"2d-53-81-a8-c1-e2",
		},
		{
			"2d:23:61:a9:c1:e2",
			"2d-23-61-a9-c1-e2",
		},
	};

	for (size_t i = 0; i < sizeof(pairs) / sizeof(pairs[0]); i++) {
		// convert
		mac_address_nl_to_ly(pairs[i].mac);

		// check
		assert_string_equal(pairs[i].mac, pairs[i].expected);
	}
}