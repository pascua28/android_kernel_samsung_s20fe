// SPDX-License-Identifier: GPL-2.0
/*
 * TODO: Add test description.
 */

#include <kunit/test.h>
#include <kunit/mock.h>
#include "../include/sec_battery.h"
#include "../include/sec_step_charging.h"

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

void sec_bat_reset_step_charging(struct sec_battery_info *battery);
void sec_bat_exit_step_charging(struct sec_battery_info *battery);
void sec_bat_set_aging_info_step_charging(struct sec_battery_info *battery);


#if !defined(CONFIG_UML)
/* NOTE: Target running TC must be in the #ifndef CONFIG_UML */
static void sec_step_charging_test_start(struct test *test)
{

}
#endif

/* NOTE: UML TC */
static void sec_step_charging_test_bar(struct test *test)
{
	/* Test cases for UML */
}

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int sec_step_charging_test_init(struct test *test)
{
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void sec_step_charging_test_exit(struct test *test)
{
}

static void kunit_sec_bat_reset_step_charging(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);
	int backup_step_charging_status = battery->step_charging_status;
#if defined(CONFIG_DIRECT_CHARGING)
	bool backup_dc_float_voltage_set = battery->dc_float_voltage_set;
#endif

	test_info(test, "START %s test\n", __func__);

	sec_bat_reset_step_charging(battery);

	EXPECT_EQ(test, battery->step_charging_status, -1);
#if defined(CONFIG_DIRECT_CHARGING)
	EXPECT_EQ(test, battery->dc_float_voltage_set, false);
#endif

	battery->step_charging_status = backup_step_charging_status;
#if defined(CONFIG_DIRECT_CHARGING)
	battery->dc_float_voltage_set = backup_dc_float_voltage_set;
#endif
}
#if 0
static void kunit_sec_bat_exit_step_charging(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);
	union power_supply_propval val;
	unsigned int backup_step_charging_float_voltage;
	int backup_step_charging_status = battery->step_charging_status;
#if defined(CONFIG_DIRECT_CHARGING)
	bool backup_dc_float_voltage_set = battery->dc_float_voltage_set;
#endif

	test_info(test, "START %s test\n", __func__);

	if (!battery->step_charging_type)
		return;

	backup_step_charging_float_voltage
		= battery->pdata->step_charging_float_voltage[battery->step_charging_step-1];

	if (battery->step_charging_type & STEP_CHARGING_CONDITION_CHARGE_POWER) {
		if (battery->max_charge_power < battery->step_charging_charge_power) {
			EXPECT_TRUE(test, battery->pdata->charging_current);
			EXPECT_TRUE(test, battery->pdata->step_charging_current);
			EXPECT_TRUE(test, battery->pdata->step_charging_float_voltage);
			EXPECT_GE(test, battery->cable_type, 0);
			EXPECT_GE(test, battery->step_charging_step-1, 0);

			sec_bat_exit_step_charging(battery);

			psy_do_property(battery->pdata->charger_name, get,
				POWER_SUPPLY_PROP_VOLTAGE_MAX, val);
			EXPECT_EQ(test, battery->pdata->step_charging_float_voltage[battery->step_charging_step-1], val.intval);
			EXPECT_EQ(test, battery->step_charging_status, -1);
#if defined(CONFIG_DIRECT_CHARGING)
			EXPECT_EQ(test, battery->dc_float_voltage_set, false);
#endif

			battery->step_charging_status = backup_step_charging_status;
#if defined(CONFIG_DIRECT_CHARGING)
			battery->dc_float_voltage_set = backup_dc_float_voltage_set;
#endif		
			if ((battery->step_charging_type & STEP_CHARGING_CONDITION_FLOAT_VOLTAGE) &&
				(battery->swelling_mode == SWELLING_MODE_NONE)) {
				val.intval = battery->pdata->step_charging_float_voltage[battery->step_charging_step-1]
					= backup_step_charging_float_voltage;
				psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_VOLTAGE_MAX, val);
			}
		}
	}
}
#endif
/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case sec_step_charging_test_cases[] = {
	/*
	 * This is a helper to create a test case object from a test case
	 * function; its exact function is not important to understand how to
	 * use KUnit, just know that this is how you associate test cases with a
	 * test module.
	 */
#if !defined(CONFIG_UML)
	/* NOTE: Target running TC */
	TEST_CASE(sec_step_charging_test_start),

	TEST_CASE(kunit_sec_bat_reset_step_charging),
	//TEST_CASE(kunit_sec_bat_exit_step_charging),
#endif
	/* NOTE: UML TC */
	TEST_CASE(sec_step_charging_test_bar),
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
static struct test_module sec_step_charging_test_module = {
	.name = "sec_step_charging_test",
	.init = sec_step_charging_test_init,
	.exit = sec_step_charging_test_exit,
	.test_cases = sec_step_charging_test_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(sec_step_charging_test_module);
