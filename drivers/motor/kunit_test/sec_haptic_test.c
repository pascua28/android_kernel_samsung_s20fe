// SPDX-License-Identifier: GPL-2.0
/*
 * TODO: Add test description.
 */
#include <linux/errno.h>
#include <kunit/test.h>
#include <kunit/mock.h>
#include <kunit/kunit_manager.h>

#include <linux/motor/sec_haptic.h>

struct sec_haptic_drvdata rdata;

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

static void sec_haptic_set_frequency_test(struct test *test)
{
	struct sec_haptic_drvdata *ddata = &rdata;

	test_info(test,"single frequency case\n");
	rdata.multi_frequency = 0;
	/* valid freq 1200~3500*/
	EXPECT_EQ(test,sec_haptic_set_frequency(ddata,1200),0);
	EXPECT_EQ(test,sec_haptic_set_frequency(ddata,2000),0);
	EXPECT_EQ(test,sec_haptic_set_frequency(ddata,3000),0);
	EXPECT_EQ(test,sec_haptic_set_frequency(ddata,3500),0);
	/* invalid freq */
	EXPECT_EQ(test,sec_haptic_set_frequency(ddata,-10),-EINVAL);
	EXPECT_EQ(test,sec_haptic_set_frequency(ddata,1000),-EINVAL);
	EXPECT_EQ(test,sec_haptic_set_frequency(ddata,4000),-EINVAL);

	return;
}

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int sec_haptic_test_init(struct test *test)
{
	rdata.ratio = 75;
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void sec_haptic_test_exit(struct test *test)
{
}

/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case sec_haptic_test_cases[] = {
	/*
	 * This is a helper to create a test case object from a test case
	 * function; its exact function is not important to understand how to
	 * use KUnit, just know that this is how you associate test cases with a
	 * test module.
	 */

	/* NOTE: UML TC */
	TEST_CASE(sec_haptic_set_frequency_test),
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
static struct test_module sec_haptic_test_module = {
	.name = "sec_haptic_test",
	.init = sec_haptic_test_init,
	.exit = sec_haptic_test_exit,
	.test_cases = sec_haptic_test_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(sec_haptic_test_module);
