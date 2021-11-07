// SPDX-License-Identifier: GPL-2.0
/*
 * TODO: Add test description.
 */

#include <kunit/test.h>
#include <kunit/mock.h>
#include <linux/device.h>
#include <linux/mfd/cs35l41/core.h>
#include <linux/mfd/cs35l41/registers.h>
#include <linux/mfd/cs35l41/calibration.h>
#include <linux/mfd/cs35l41/big_data.h>
#include <linux/mfd/cs35l41/power.h>
#include <linux/regmap.h>
#include <linux/typecheck.h>

/*
 * This is the most fundamental element of KUnit, the test case. A test case
 * makes a set EXPECTATIONs and ASSERTIONs about the behavior of some code; if
 * any expectations or assertions are not met, the test fails; otherwise, the
 * test passes.
 *
 * In KUnit, a test case is just a function with the signature
 * `void (*)(struct test *)`. `struct test` is a context object that stores
 * information about the current test.
 */

#if !defined(CONFIG_UML)
static void cs35l41_test_pwr_start(struct test *test)
{
	const char *suffix_left = "_l";
	const char *suffix_right = "_r";
	const char *suffix_wrong = "_x";

	test_info(test, "%s: start", __func__);
	cirrus_pwr_start(suffix_left);
	cirrus_pwr_start(suffix_right);
	cirrus_pwr_start(suffix_wrong);
	test_info(test, "%s: end", __func__);
}

static void cs35l41_test_pwr_stop(struct test *test)
{
	const char *suffix_left = "_l";
	const char *suffix_right = "_r";
	const char *suffix_wrong = "_x";

	test_info(test, "%s: start", __func__);
	cirrus_pwr_stop(suffix_left);
	cirrus_pwr_stop(suffix_right);
	cirrus_pwr_stop(suffix_wrong);
	test_info(test, "%s: end", __func__);
}

static void cs35l41_test_pwr_set(struct test *test)
{
	const char *suffix_left = "_l";
	const char *suffix_right = "_r";
	const char *suffix_wrong = "_x";
	int ret;

	test_info(test, "%s: start", __func__);
	ret = cirrus_pwr_set_params(false, suffix_wrong, 0, 0);
	EXPECT_EQ(test, ret, 0);
	ret = cirrus_pwr_set_params(false, suffix_right, 0, 0);
	EXPECT_EQ(test, ret, 0);
	ret = cirrus_pwr_set_params(false, suffix_left, 0, 0);
	EXPECT_EQ(test, ret, 0);
	test_info(test, "%s: end", __func__);
}

static void cs35l41_test_pwr_add(struct test *test)
{
	const char *suffix_left = "_l";
	const char *suffix_right = "_r";
	const char *suffix_wrong = "_x";
	int ret;

	test_info(test, "%s: start", __func__);
	ret = cirrus_pwr_amp_add(NULL, suffix_wrong, NULL);
	EXPECT_EQ(test, ret, 0);

	ret = cirrus_pwr_amp_add(NULL, suffix_right, NULL);
	EXPECT_EQ(test, ret, 0);

	ret = cirrus_pwr_amp_add(NULL, suffix_left, NULL);
	EXPECT_EQ(test, ret, 0);
	test_info(test, "%s: end", __func__);
}
#endif

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int cirrus_pwr_test_init(struct test *test)
{
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void cirrus_pwr_test_exit(struct test *test)
{
}

/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case cirrus_pwr_test_cases[] = {
	/*
	 * This is a helper to create a test case object from a test case
	 * function; its exact function is not important to understand how to
	 * use KUnit, just know that this is how you associate test cases with a
	 * test module.
	 */
#if !defined(CONFIG_UML)
	TEST_CASE(cs35l41_test_pwr_start),
	TEST_CASE(cs35l41_test_pwr_stop),
	TEST_CASE(cs35l41_test_pwr_set),
	TEST_CASE(cs35l41_test_pwr_add),
#endif
	{},
};

/*
 * This defines a suite or grouping of tests.
 *
 * Test cases are defined as belonging to the suite by adding them to
 * `test_cases`.
 *
 * Often it is desirable to run some function which will set up things which
 * will be used by every test; this is accomplished with an `init` function
 * which runs before each test case is invoked. Similarly, an `exit` function
 * may be specified which runs after every test case and can be used to for
 * cleanup. For clarity, running tests in a test module would behave as follows:
 *
 * module.init(test);
 * module.test_case[0](test);
 * module.exit(test);
 * module.init(test);
 * module.test_case[1](test);
 * module.exit(test);
 * ...;
 */
static struct test_module cirrus_pwr_test_module = {
	.name = "cirrus_pwr_test",
	.init = cirrus_pwr_test_init,
	.exit = cirrus_pwr_test_exit,
	.test_cases = cirrus_pwr_test_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(cirrus_pwr_test_module);
