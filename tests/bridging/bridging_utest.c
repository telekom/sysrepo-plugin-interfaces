#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <bridging/common.h>

static void test_correct_mac_ly_to_nl(void **state);
static void test_correct_mac_nl_to_ly(void **state);

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
	char mac_address[] = "2c-54-91-88-c9-e3";
	const char *expected = "2c:54:91:88:c9:e3";

	mac_address_ly_to_nl(mac_address);

	assert_string_equal(mac_address, expected);
}

static void test_correct_mac_nl_to_ly(void **state)
{
	char mac_address[] = "00:25:96:12:34:56";
	const char expected[] = "00-25-96-12-34-56";

	mac_address_nl_to_ly(mac_address);

	assert_string_equal(mac_address, expected);
}