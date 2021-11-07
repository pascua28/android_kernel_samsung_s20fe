// SPDX-License-Identifier: GPL-2.0
/*
 * TODO: Add test description.
 */

#include <kunit/test.h>
#include <kunit/mock.h>

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
/* NOTE: Target running TC must be in the #ifndef CONFIG_UML */

#include "../abox.h"
static struct abox_data *p_abox_data;

static void abox_shm_test_of_abox_shm_init(struct test *test)
{
	abox_shm_init(p_abox_data->shm_addr, p_abox_data->shm_size);
	EXPECT_EQ(test, shm_base, p_abox_data->shm_addr);
	EXPECT_EQ(test, shm_size, p_abox_data->shm_size);
}

static void abox_shm_test_of_abox_shm_readb_writeb(struct test *test)
{
	u8 val;
	unsigned int offset = 0;

	val = abox_shm_readb(offset);
	abox_shm_writeb(val, offset);
	EXPECT_EQ(test, val, abox_shm_readb(offset));
}

static void abox_shm_test_of_abox_shm_readb_writew(struct test *test)
{
	u16 val;
	unsigned int offset = 0;

	val = abox_shm_readw(offset);
	abox_shm_writew(val, offset);
	EXPECT_EQ(test, val, abox_shm_readw(offset));
}

static void abox_shm_test_of_abox_shm_readb_writel(struct test *test)
{
	u32 val;
	unsigned int offset = 0;

	val = abox_shm_readl(offset);
	abox_shm_writel(val, offset);
	EXPECT_EQ(test, val, abox_shm_readl(offset));
}

static void abox_shm_test_of_abox_shm_readb_writeq(struct test *test)
{
	u64 val;
	unsigned int offset = 0;

	val = abox_shm_readq(offset);
	abox_shm_writeq(val, offset);
	EXPECT_EQ(test, val, abox_shm_readq(offset));
}
#endif

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int abox_shm_test_init(struct test *test)
{
	p_abox_data = abox_get_abox_data();
	if (!p_abox_data)
		FAIL(test, "Fail to get abox data");
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void abox_shm_test_exit(struct test *test)
{

}

/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case abox_shm_test_cases[] = {
	/*
	 * This is a helper to create a test case object from a test case
	 * function; its exact function is not important to understand how to
	 * use KUnit, just know that this is how you associate test cases with a
	 * test module.
	 */
#if !defined(CONFIG_UML)
	/* NOTE: Target running TC */
	TEST_CASE(abox_shm_test_of_abox_shm_init),
	TEST_CASE(abox_shm_test_of_abox_shm_readb_writeb),
	TEST_CASE(abox_shm_test_of_abox_shm_readb_writew),
	TEST_CASE(abox_shm_test_of_abox_shm_readb_writel),
	TEST_CASE(abox_shm_test_of_abox_shm_readb_writeq),
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
static struct test_module abox_shm_test_module = {
	.name = "abox_shm_test",
	.init = abox_shm_test_init,
	.exit = abox_shm_test_exit,
	.test_cases = abox_shm_test_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(abox_shm_test_module);
