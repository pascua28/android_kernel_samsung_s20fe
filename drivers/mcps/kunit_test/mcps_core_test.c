// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Samsung Electronics.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/moduleparam.h>

#include <net/ip.h>
#include <linux/inet.h>

#include <kunit/test.h>
#include <kunit/mock.h>

#include "../mcps_device.h"
#include "../mcps_sauron.h"
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
extern void init_sauron(struct sauron *sauron);
extern struct eye *pick_heavy(struct sauron *sauron, int cpu);
extern struct eye *pick_light(struct sauron *sauron, int cpu);
extern void __add_flow(struct sauron *sauron, struct eye *eye);
extern void __delete_flow(struct sauron *sauron, struct eye *eye);
extern void update_eye(struct sauron *sauron, struct eye *eye);
static struct sauron *mock_sauron = NULL;
/* NOTE: Target running TC must be in the #ifndef CONFIG_UML */
static void kunit_pick_flow_when_null_return_null(struct test *test)
{
	//Arrange(Given)
	int cpu = 0;
	struct eye *heavy_eye = NULL;
	struct eye *light_eye = NULL;
	mock_sauron = (struct sauron *)kzalloc(sizeof(struct sauron), GFP_KERNEL);
	if (mock_sauron == NULL) {
		return;
	}
	init_sauron(mock_sauron);

	//Act (When)
	for_each_possible_cpu(cpu) {
		heavy_eye = pick_heavy(mock_sauron, 0);
		light_eye = pick_light(mock_sauron, 0);

		//Assert(Then)
		EXPECT_TRUE(test, (heavy_eye == NULL));
		EXPECT_TRUE(test, (light_eye == NULL));

		if (heavy_eye != NULL) {
			kfree(heavy_eye);
		}

		if (light_eye != NULL) {
			kfree(light_eye);
		}
	}

	kfree(mock_sauron);
}

static void kunit_check_sauron_flow_cnt_by_cpus_when_add_and_delete_unmonitored_flow(struct test *test)
{
	//Arrange(Given)
	int cpu = 0;
	struct eye *mock_eye = (struct eye *)kzalloc(sizeof(struct eye), GFP_KERNEL);
	if (!mock_eye) {
		return;
	}

	mock_sauron = (struct sauron *)kzalloc(sizeof(struct sauron), GFP_KERNEL);
	if (mock_sauron == NULL) {
		return;
	}
	init_sauron(mock_sauron);

	mock_eye->cpu = (u32)0;
	mock_eye->hash = (u32)1234;
	mock_eye->t_stamp = mock_eye->t_capture = jiffies;
	mock_eye->policy = EYE_POLICY_FAST;

	for_each_possible_cpu(cpu) {
		//Act (When)
		mock_eye->cpu = (u32)cpu;

		sauron_lock(mock_sauron);
		__add_flow(mock_sauron, mock_eye);
		sauron_unlock(mock_sauron);

		//Assert(Then)
		EXPECT_EQ(test, mock_sauron->flow_cnt_by_cpus[cpu], 1);

		//Act (When)
		sauron_lock(mock_sauron);
		__delete_flow(mock_sauron, mock_eye);
		sauron_unlock(mock_sauron);

		//Assert(Then)
		EXPECT_EQ(test, mock_sauron->flow_cnt_by_cpus[cpu], 0);
	}

	kfree(mock_eye);
	kfree(mock_sauron);
}

static void kunit_monitor_flow_when_update_eye(struct test *test)
{
	//Arrange(Given)
	struct eye *heavy_eye = NULL;
	struct eye *light_eye = NULL;
	struct eye *mock_eye = (struct eye *)kzalloc(sizeof(struct eye), GFP_KERNEL);
	if (!mock_eye) {
		return;
	}

	mock_sauron = (struct sauron *)kzalloc(sizeof(struct sauron), GFP_KERNEL);
	if (mock_sauron == NULL) {
		return;
	}
	init_sauron(mock_sauron);

	mock_eye->cpu = (u32)0;
	mock_eye->hash = (u32)1234;

	mock_eye->t_stamp = mock_eye->t_capture = jiffies;
	mock_eye->policy = EYE_POLICY_FAST;

	//Act (When)
	sauron_lock(mock_sauron);
	__add_flow(mock_sauron, mock_eye);
	sauron_unlock(mock_sauron);

	update_eye(mock_sauron, mock_eye);
	heavy_eye = pick_heavy(mock_sauron, 0);
	light_eye = pick_light(mock_sauron, 0);

	//Assert(Then)
	EXPECT_EQ(test, mock_sauron->flow_cnt_by_cpus[0], 1);
	EXPECT_EQ(test, mock_sauron->target_flow_cnt_by_cpus[0], 0);
	EXPECT_TRUE(test, (heavy_eye == NULL));
	EXPECT_TRUE(test, (light_eye == NULL));

	//Arrange(Given)
	mock_eye->t_capture = mock_eye->t_stamp - (HZ/2) - 1;
	mock_eye->capture = 0;
	mock_eye->value = 1000;

	//Act (When)
	update_eye(mock_sauron, mock_eye);
	heavy_eye = pick_heavy(mock_sauron, 0);
	light_eye = pick_light(mock_sauron, 0);

	//Assert(Then)
	EXPECT_EQ(test, mock_sauron->flow_cnt_by_cpus[0], 1);
	EXPECT_EQ(test, mock_sauron->target_flow_cnt_by_cpus[0], 1);
	EXPECT_EQ(test, mock_eye->monitored, 1);
	EXPECT_TRUE(test, (heavy_eye != NULL));
	EXPECT_TRUE(test, (light_eye != NULL));

	//Arrange(Given)
	mock_eye->t_stamp = jiffies;
	mock_eye->t_capture = mock_eye->t_stamp - 1;
	mock_eye->capture = 1000;
	mock_eye->value = 1001;

	//Act (When)
	update_eye(mock_sauron, mock_eye);
	heavy_eye = pick_heavy(mock_sauron, 0);
	light_eye = pick_light(mock_sauron, 0);

	//Assert(Then)
	EXPECT_EQ(test, mock_sauron->flow_cnt_by_cpus[0], 1);
	EXPECT_EQ(test, mock_sauron->target_flow_cnt_by_cpus[0], 1);
	EXPECT_EQ(test, mock_eye->monitored, 1);
	EXPECT_TRUE(test, (heavy_eye != NULL));
	EXPECT_TRUE(test, (light_eye != NULL));

	//Arrange(Given)
	mock_eye->t_stamp = jiffies;
	mock_eye->t_capture = mock_eye->t_stamp - (HZ/2) - 1;
	mock_eye->capture = 1001;
	mock_eye->value = 1002;

	//Act (When)
	update_eye(mock_sauron, mock_eye);
	heavy_eye = pick_heavy(mock_sauron, 0);
	light_eye = pick_light(mock_sauron, 0);

	//Assert(Then)
	EXPECT_EQ(test, mock_sauron->flow_cnt_by_cpus[0], 1);
	EXPECT_EQ(test, mock_sauron->target_flow_cnt_by_cpus[0], 0);
	EXPECT_EQ(test, mock_eye->monitored, 0);
	EXPECT_TRUE(test, (heavy_eye == NULL));
	EXPECT_TRUE(test, (light_eye == NULL));

	kfree(mock_eye);
	kfree(mock_sauron);
}
#endif

/* NOTE: UML TC */
static void test_mcps_core_bar(struct test *test)
{
	/* Test cases for UML */
	return;
}

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int test_mcps_core_init(struct test *test)
{
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void test_mcps_core_exit(struct test *test)
{

}

/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case test_mcps_core_cases[] = {
		/*
		 * This is a helper to create a test case object from a test case
		 * function; its exact function is not important to understand how to
		 * use KUnit, just know that this is how you associate test cases with a
		 * test module.
		 */
#if !defined(CONFIG_UML)
		/* NOTE: Target running TC */
		TEST_CASE(kunit_pick_flow_when_null_return_null),
		TEST_CASE(kunit_check_sauron_flow_cnt_by_cpus_when_add_and_delete_unmonitored_flow),
		TEST_CASE(kunit_monitor_flow_when_update_eye),
#endif
		/* NOTE: UML TC */
		TEST_CASE(test_mcps_core_bar),
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
static struct test_module test_mcps_core_module = {
		.name = "test_mcps_core",
		.init = test_mcps_core_init,
		.exit = test_mcps_core_exit,
		.test_cases = test_mcps_core_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(test_mcps_core_module);
