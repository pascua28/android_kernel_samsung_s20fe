// SPDX-License-Identifier: GPL-2.0
/*
 * Kunit test for s2dos05 regulator driver
 */

#include <kunit/test.h>
#include <kunit/mock.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/s2dos05.h>

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
static void s2dos05_test_foo(struct test *test)
{
	/*
	 * This is an EXPECTATION; it is how KUnit tests things. When you want
	 * to test a piece of code, you set some expectations about what the
	 * code should do. KUnit then runs the test and verifies that the code's
	 * behavior matched what was expected.
	 */
	EXPECT_EQ(test, 1, 1);
}
#endif

static void s2dos05_test_bar(struct test *test)
{
	/* Test cases for UML */
	EXPECT_EQ(test, 1, 1);
	return;
}

extern int s2m_ssd_convert_uA_to_reg_val(bool enable, int min_uA, int max_uA,
						u8 *val, u8 *mask);

static void s2dos05_test_set_elvss_short_detection1(struct test *test)
{
	bool enable;
	u8 val, mask;
	int ret;

	enable = true;
	ret = s2m_ssd_convert_uA_to_reg_val(enable, 4000, 4000, &val, &mask);
	EXPECT_EQ(test, ret, 0);
	EXPECT_EQ(test, val, (!enable << 3 | 1 << 5));
	EXPECT_EQ(test, mask, S2DOS05_ELVSS_SSD_EN_MASK | S2DOS05_ELVSS_SEL_SSD_MASK);

	enable = false;
	ret = s2m_ssd_convert_uA_to_reg_val(enable, 6000, 6000, &val, &mask);
	EXPECT_EQ(test, ret, 0);
	EXPECT_EQ(test, val, (!enable << 3 | 0 << 5));
	EXPECT_EQ(test, mask, S2DOS05_ELVSS_SSD_EN_MASK | S2DOS05_ELVSS_SEL_SSD_MASK);

	enable = true;
	ret = s2m_ssd_convert_uA_to_reg_val(enable, 8000, 8000, &val, &mask);
	EXPECT_EQ(test, ret, 0);
	EXPECT_EQ(test, val, (!enable << 3 | 3 << 5));
	EXPECT_EQ(test, mask, S2DOS05_ELVSS_SSD_EN_MASK | S2DOS05_ELVSS_SEL_SSD_MASK);

	enable = true;
	ret = s2m_ssd_convert_uA_to_reg_val(enable, 0, 0, &val, &mask);
	EXPECT_EQ(test, ret, -EINVAL);

	enable = true;
	ret = s2m_ssd_convert_uA_to_reg_val(enable, 2000, 4000, &val, &mask);
	EXPECT_EQ(test, ret, 0);
	EXPECT_EQ(test, val, (!enable << 3 | 0 << 5));
	EXPECT_EQ(test, mask, S2DOS05_ELVSS_SSD_EN_MASK | S2DOS05_ELVSS_SEL_SSD_MASK);

	enable = true;
	ret = s2m_ssd_convert_uA_to_reg_val(enable, 1000, 5000, &val, &mask);
	EXPECT_EQ(test, ret, 0);
	EXPECT_EQ(test, val, (!enable << 3 | 0 << 5));
	EXPECT_EQ(test, mask, S2DOS05_ELVSS_SSD_EN_MASK | S2DOS05_ELVSS_SEL_SSD_MASK);

	enable = true;
	ret = s2m_ssd_convert_uA_to_reg_val(enable, 3000, 3500, &val, &mask);
	EXPECT_EQ(test, ret, -EINVAL);

	return;
}

extern int s2m_set_elvss_short_detection(struct regulator_dev *rdev,
						bool enable, int lv_uA);
extern int s2m_get_elvss_ssd_current_limit(struct regulator_dev *rdev);
extern int s2m_set_elvss_ssd_current_limit(struct regulator_dev *rdev,
						int min_uA, int max_uA);

#if !defined(CONFIG_UML)
static inline void s2dos05_test_init_regmap(void) { }

static void s2dos05_test_set_elvss_short_detection2(struct test *test)
{
	struct regulator *elvss_rgt; 
	int old_config;

	elvss_rgt = regulator_get(NULL, "elvss");

	if (IS_ERR(elvss_rgt)) {
		FAIL(test, "regulator_get failed\n");
		return;
	}

	old_config = regulator_get_current_limit(elvss_rgt);
	if (old_config < 0) {
		SUCCEED(test);
		return;
	}

	/* Check regulator_set_short_detection function */
	EXPECT_EQ(test, -EINVAL, regulator_set_short_detection(elvss_rgt, 1, 1000));

	EXPECT_EQ(test, 0, regulator_set_short_detection(elvss_rgt, 1, 2000));
	EXPECT_EQ(test, 2000, regulator_get_current_limit(elvss_rgt));

	EXPECT_EQ(test, -EINVAL, regulator_set_short_detection(elvss_rgt, 1, 2050));

	EXPECT_EQ(test, 0, regulator_set_short_detection(elvss_rgt, 1, 6000));
	EXPECT_EQ(test, 6000, regulator_get_current_limit(elvss_rgt));

	EXPECT_EQ(test, 0, regulator_set_short_detection(elvss_rgt, 0, 8000));
	EXPECT_EQ(test, 0, regulator_get_current_limit(elvss_rgt));

	EXPECT_EQ(test, -EINVAL, regulator_set_short_detection(elvss_rgt, 1, 0));
	EXPECT_EQ(test, 0, regulator_get_current_limit(elvss_rgt));

	/* Check regulator_set_current_limit function */
	EXPECT_EQ(test, 0, regulator_set_current_limit(elvss_rgt, 1000, 4000));
	EXPECT_EQ(test, 2000, regulator_get_current_limit(elvss_rgt));

	EXPECT_EQ(test, 0, regulator_set_current_limit(elvss_rgt, 0, 0));
	EXPECT_EQ(test, 0, regulator_get_current_limit(elvss_rgt));

	EXPECT_EQ(test, 0, regulator_set_current_limit(elvss_rgt, 0, 8000));
	EXPECT_EQ(test, 2000, regulator_get_current_limit(elvss_rgt));

	EXPECT_EQ(test, 0, regulator_set_current_limit(elvss_rgt, 3000, 6000));
	EXPECT_EQ(test, 4000, regulator_get_current_limit(elvss_rgt));

	EXPECT_EQ(test, -EINVAL, regulator_set_current_limit(elvss_rgt, 9000, 9000));

	/* Restore old configuration */
	EXPECT_EQ(test, 0, regulator_set_current_limit(elvss_rgt, old_config, old_config));
	EXPECT_EQ(test, old_config, regulator_get_current_limit(elvss_rgt));

	regulator_put(elvss_rgt);
}
#else
static unsigned char s2dos05_regmap[S2DOS05_REG_DEVICE_ID_PGM];

static void s2dos05_test_init_regmap(void)
{
	int i;

	for (i = 0; i < S2DOS05_REG_DEVICE_ID_PGM; i++)
		s2dos05_regmap[i] = 0;
}

DEFINE_FUNCTION_MOCK(s2dos05_read_reg, RETURNS(int),
		PARAMS(struct i2c_client *, u8, u8 *));
int REAL_ID(s2dos05_read_reg)(struct i2c_client *i2c, u8 reg, u8 *dest)
{
	*dest = s2dos05_regmap[reg];
	return 0;
}
DEFINE_INVOKABLE(s2dos05_read_reg, RETURNS(int), ASSIGN,
		PARAMS(struct i2c_client *, u8, u8 *));

DEFINE_FUNCTION_MOCK(s2dos05_update_reg, RETURNS(int),
		PARAMS(struct i2c_client *, u8, u8, u8));
int REAL_ID(s2dos05_update_reg)(struct i2c_client *i2c, u8 reg, u8 val, u8 mask)
{
	u8 old_val, new_val;

	old_val = s2dos05_regmap[reg] & 0xff;
	new_val = (val & mask) | (old_val & (~mask));
	s2dos05_regmap[reg] = new_val;

	return 0;
}
DEFINE_INVOKABLE(s2dos05_update_reg, RETURNS(int), ASSIGN,
		PARAMS(struct i2c_client *, u8, u8, u8));

/* NOTE: UML TC */
struct s2dos05_data {
	struct s2dos05_dev *iodev;
	int num_regulators;
	struct regulator_dev *rdev[S2DOS05_REGULATOR_MAX];
	int opmode[S2DOS05_REGULATOR_MAX];
#ifdef CONFIG_DRV_SAMSUNG_PMIC
	u8 read_addr;
	u8 read_val;
	struct device *dev;
#endif
#ifdef CONFIG_SEC_PM
#ifdef CONFIG_SEC_FACTORY
	struct notifier_block fb_block;
#endif /* CONFIG_SEC_FACTORY */
#endif /* CONFIG_SEC_PM */
};

static void s2dos05_test_set_elvss_short_detection2(struct test *test)
{
	struct regulator_dev rdev;
	struct s2dos05_data info;
	struct s2dos05_dev iodev;
	struct mock_expectation *h_read;
	struct mock_expectation *h_update;

	h_read = EXPECT_CALL(s2dos05_read_reg(any(test), any(test), any(test)));
	h_read->action = INVOKE_REAL(test, s2dos05_read_reg);
	AtMost(INT_MAX, h_read);

	h_update = EXPECT_CALL(s2dos05_update_reg(any(test), any(test),
				any(test), any(test)));
	h_update->action = INVOKE_REAL(test, s2dos05_update_reg);
	AtMost(INT_MAX, h_update);

	rdev.reg_data = &info;
	info.iodev = &iodev;

	EXPECT_EQ(test, 0, s2m_set_elvss_short_detection(&rdev, 1, 4000));
	EXPECT_EQ(test, 4000, s2m_get_elvss_ssd_current_limit(&rdev));

	EXPECT_EQ(test, -EINVAL, s2m_set_elvss_short_detection(&rdev, 1, 1000));

	EXPECT_EQ(test, 0, s2m_set_elvss_short_detection(&rdev, 1, 2000));
	EXPECT_EQ(test, 2000, s2m_get_elvss_ssd_current_limit(&rdev));

	EXPECT_EQ(test, 0, s2m_set_elvss_short_detection(&rdev, 0, 8000));
	EXPECT_EQ(test, 0, s2m_get_elvss_ssd_current_limit(&rdev));

	EXPECT_EQ(test, -EINVAL, s2m_set_elvss_short_detection(&rdev, 1, 0));

	/*******************************************************/

	EXPECT_EQ(test, 0, s2m_set_elvss_ssd_current_limit(&rdev, 1000, 4000));
	EXPECT_EQ(test, 2000, s2m_get_elvss_ssd_current_limit(&rdev));

	EXPECT_EQ(test, 0, s2m_set_elvss_ssd_current_limit(&rdev, 0, 0));
	EXPECT_EQ(test, 0, s2m_get_elvss_ssd_current_limit(&rdev));

	EXPECT_EQ(test, 0, s2m_set_elvss_ssd_current_limit(&rdev, 2000, 6000));
	EXPECT_EQ(test, 2000, s2m_get_elvss_ssd_current_limit(&rdev));

	EXPECT_EQ(test, -EINVAL, s2m_set_elvss_ssd_current_limit(&rdev, 3000, 3010));

	EXPECT_EQ(test, 0, s2m_set_elvss_ssd_current_limit(&rdev, 5000, 6000));
	EXPECT_EQ(test, 6000, s2m_get_elvss_ssd_current_limit(&rdev));
}
#endif /* !CONFIG_UML */

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int s2dos05_test_init(struct test *test)
{
	s2dos05_test_init_regmap();
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void s2dos05_test_exit(struct test *test)
{
}

/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case s2dos05_test_cases[] = {
	/*
	 * This is a helper to create a test case object from a test case
	 * function; its exact function is not important to understand how to
	 * use KUnit, just know that this is how you associate test cases with a
	 * test module.
	 */
#if !defined(CONFIG_UML)
	/* NOTE: Target running TC */
	TEST_CASE(s2dos05_test_foo),
#endif
	TEST_CASE(s2dos05_test_bar),
	TEST_CASE(s2dos05_test_set_elvss_short_detection1),
	TEST_CASE(s2dos05_test_set_elvss_short_detection2),
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
static struct test_module s2dos05_test_module = {
	.name = "s2dos05_test",
	.init = s2dos05_test_init,
	.exit = s2dos05_test_exit,
	.test_cases = s2dos05_test_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(s2dos05_test_module);
