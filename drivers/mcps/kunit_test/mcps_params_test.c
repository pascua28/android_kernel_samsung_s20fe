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

#if defined(CONFIG_MCPS_ICGB) && \
	!defined(CONFIG_KLAT) && \
	(defined(CONFIG_SOC_EXYNOS9830) || defined(CONFIG_SOC_EXYNOS9820))
extern int check_mcps_in_addr(__be32 addr);
extern int check_mcps_in6_addr(struct in6_addr *addr);
extern int set_mcps_in_addr(const char *buf, const struct kernel_param *kp);
extern int get_mcps_in_addr(char *buf, const struct kernel_param *kp);
extern int set_mcps_in6_addr(const char *buf, const struct kernel_param *kp);
extern int get_mcps_in6_addr(char *buf, const struct kernel_param *kp);
extern __be32 __mcps_in_addr[8];
extern struct in6_addr __mcps_in6_addr[8];

static __be32 mock__mcps_in_addr[9];
static struct in6_addr mock__mcps_in6_addr[9];
#endif

static char *buffer;
static struct sauron *mock_sauron;
extern int set_mcps_flush(const char *buf, const struct kernel_param *kp);
extern int get_mcps_flush(char *buf, const struct kernel_param *kp);
extern int set_mcps_newflow_cpu(const char *val, const struct kernel_param *kp);
extern int get_mcps_newflow_cpu(char *buffer, const struct kernel_param *kp);
extern int set_mcps_dynamic_cpu(const char *val, const struct kernel_param *kp);
extern int get_mcps_dynamic_cpu(char *buffer, const struct kernel_param *kp);
extern int set_mcps_arps_cpu(const char *val, const struct kernel_param *kp);
extern int get_mcps_arps_cpu(char *buffer, const struct kernel_param *kp);
extern int set_mcps_arps_config(const char *val, const struct kernel_param *kp);
extern int get_mcps_arps_config(char *buffer, const struct kernel_param *kp);

extern int get_mcps_heavy_flow(char *buffer, const struct kernel_param *kp);
extern int get_mcps_light_flow(char *buffer, const struct kernel_param *kp);
extern int get_mcps_enqueued(char *buffer, const struct kernel_param *kp);
extern int get_mcps_processed(char *buffer, const struct kernel_param *kp);
extern int get_mcps_dropped(char *buffer, const struct kernel_param *kp);
extern int get_mcps_ignored(char *buffer, const struct kernel_param *kp);
extern int get_mcps_distributed(char *buffer, const struct kernel_param *kp);
extern int get_mcps_sauron_target_flow(char *buffer, const struct kernel_param *kp);
extern int get_mcps_sauron_flow(char *buffer, const struct kernel_param *kp);

extern int create_and_init_arps_config(struct mcps_config *mcps);
extern int release_arps_config(struct mcps_config *mcps);
extern void init_sauron(struct sauron *sauron);

#if !defined(CONFIG_UML)
/* NOTE: Target running TC must be in the #ifndef CONFIG_UML */
#if defined(CONFIG_MCPS_ICGB) && \
	!defined(CONFIG_KLAT) && \
	(defined(CONFIG_SOC_EXYNOS9830) || defined(CONFIG_SOC_EXYNOS9820))
static void kunit_check_mcps_in_addr(struct test *test)
{
	int i = 0;
	int results[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	//Arrange(Given)
	__mcps_in_addr[0] = in_aton("1.1.1.0");
	__mcps_in_addr[1] = in_aton("1.1.1.1");
	__mcps_in_addr[2] = in_aton("1.1.1.2");
	__mcps_in_addr[3] = in_aton("1.1.1.3");
	__mcps_in_addr[4] = in_aton("1.1.1.4");
	__mcps_in_addr[5] = in_aton("1.1.1.5");
	__mcps_in_addr[6] = in_aton("1.1.1.6");
	__mcps_in_addr[7] = in_aton("1.1.1.7");

	mock__mcps_in_addr[0] = in_aton("1.1.1.0"); // true
	mock__mcps_in_addr[1] = in_aton("1.1.1.1"); // true
	mock__mcps_in_addr[2] = in_aton("1.1.1.2"); // true
	mock__mcps_in_addr[3] = in_aton("1.1.1.3"); // true
	mock__mcps_in_addr[4] = in_aton("1.1.1.4"); // true
	mock__mcps_in_addr[5] = in_aton("1.1.1.5"); // true
	mock__mcps_in_addr[6] = in_aton("1.1.1.6"); // true
	mock__mcps_in_addr[7] = in_aton("1.1.1.7"); // true
	mock__mcps_in_addr[8] = in_aton("1.1.1.8"); // false

	//Act(when)
	for (i = 0; i < 9; i++) {
		results[i] = check_mcps_in_addr(mock__mcps_in_addr[i]);
	}

	//Assert(Then)
	EXPECT_TRUE(test, results[0]);
	EXPECT_TRUE(test, results[1]);
	EXPECT_TRUE(test, results[2]);
	EXPECT_TRUE(test, results[3]);
	EXPECT_TRUE(test, results[4]);
	EXPECT_TRUE(test, results[5]);
	EXPECT_TRUE(test, results[6]);
	EXPECT_TRUE(test, results[7]);
	EXPECT_FALSE(test, results[8]);
}

static void kunit_check_mcps_in6_addr(struct test *test)
{
	int i = 0;
	int results[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	struct in6_addr val[9];

	//Arrange(Given)
	in6_pton("1:1:1:1:1:1:1:0", -1, val[0].s6_addr, -1, NULL);
	in6_pton("1:1:1:1:1:1:1:1", -1, val[1].s6_addr, -1, NULL);
	in6_pton("1:1:1:1:1:1:1:2", -1, val[2].s6_addr, -1, NULL);
	in6_pton("1:1:1:1:1:1:1:3", -1, val[3].s6_addr, -1, NULL);
	in6_pton("1:1:1:1:1:1:1:4", -1, val[4].s6_addr, -1, NULL);
	in6_pton("1:1:1:1:1:1:1:5", -1, val[5].s6_addr, -1, NULL);
	in6_pton("1:1:1:1:1:1:1:6", -1, val[6].s6_addr, -1, NULL);
	in6_pton("1:1:1:1:1:1:1:7", -1, val[7].s6_addr, -1, NULL);
	in6_pton("1:1:1:1:1:1:1:8", -1, val[8].s6_addr, -1, NULL);

	__mcps_in6_addr[0] = val[0];
	__mcps_in6_addr[1] = val[1];
	__mcps_in6_addr[2] = val[2];
	__mcps_in6_addr[3] = val[3];
	__mcps_in6_addr[4] = val[4];
	__mcps_in6_addr[5] = val[5];
	__mcps_in6_addr[6] = val[6];
	__mcps_in6_addr[7] = val[7];

	mock__mcps_in6_addr[0] = val[0]; // true
	mock__mcps_in6_addr[1] = val[1]; // true
	mock__mcps_in6_addr[2] = val[2]; // true
	mock__mcps_in6_addr[3] = val[3]; // true
	mock__mcps_in6_addr[4] = val[4]; // true
	mock__mcps_in6_addr[5] = val[5]; // true
	mock__mcps_in6_addr[6] = val[6]; // true
	mock__mcps_in6_addr[7] = val[7]; // true
	mock__mcps_in6_addr[8] = val[8]; // false

	//Act(when)
	for (i = 0; i < 9; i++) {
		results[i] = check_mcps_in6_addr(&mock__mcps_in6_addr[i]);
	}

	//Assert(Then)
	EXPECT_TRUE(test, results[0]);
	EXPECT_TRUE(test, results[1]);
	EXPECT_TRUE(test, results[2]);
	EXPECT_TRUE(test, results[3]);
	EXPECT_TRUE(test, results[4]);
	EXPECT_TRUE(test, results[5]);
	EXPECT_TRUE(test, results[6]);
	EXPECT_TRUE(test, results[7]);
	EXPECT_FALSE(test, results[8]);
}

static void kunit_mcps_in_addr(struct test *test)
{
	if (buffer == NULL) {
		return;
	}
	//Arrange(Given)
	set_mcps_in_addr("0@1.1.1.0", NULL);
	set_mcps_in_addr("1@1.1.1.1", NULL);
	set_mcps_in_addr("2@1.1.1.2", NULL);
	set_mcps_in_addr("3@1.1.1.3", NULL);
	set_mcps_in_addr("4@1.1.1.4", NULL);
	set_mcps_in_addr("5@1.1.1.5", NULL);
	set_mcps_in_addr("6@1.1.1.6", NULL);
	set_mcps_in_addr("7@1.1.1.7", NULL);

	//Act(when)
	set_mcps_in_addr("010.10.10.0", NULL);
	set_mcps_in_addr("one@10.10.10.1", NULL);
	set_mcps_in_addr("2@", NULL);
	set_mcps_in_addr("-3@10.10.10.3", NULL);
	set_mcps_in_addr("4@10.10.10.4", NULL);
	set_mcps_in_addr("5@10.10.10.5", NULL);
	set_mcps_in_addr("6@10.10.10.6", NULL);
	set_mcps_in_addr("7@10.10.10.7", NULL);
	set_mcps_in_addr("8@10.10.10.8", NULL);

	get_mcps_in_addr(buffer, NULL);

	//Assert(Then)
	EXPECT_STREQ(test, buffer, "1.1.1.0\n1.1.1.1\n1.1.1.2\n1.1.1.3\n10.10.10.4\n10.10.10.5\n10.10.10.6\n10.10.10.7\n");
}

static void kunit_mcps_in6_addr(struct test *test)
{
	if (buffer == NULL) {
		return;
	}
	//Arrange(Given)
	set_mcps_in6_addr("0@1:1:1:1:1:1:1:0", NULL);
	set_mcps_in6_addr("1@1:1:1:1:1:1:1:1", NULL);
	set_mcps_in6_addr("2@1:1:1:1:1:1:1:2", NULL);
	set_mcps_in6_addr("3@1:1:1:1:1:1:1:3", NULL);
	set_mcps_in6_addr("4@1:1:1:1:1:1:1:4", NULL);
	set_mcps_in6_addr("5@1:1:1:1:1:1:1:5", NULL);
	set_mcps_in6_addr("6@1:1:1:1:1:1:1:6", NULL);
	set_mcps_in6_addr("7@1:1:1:1:1:1:1:7", NULL);

	//Act(when)
	set_mcps_in6_addr("010:10:10:10:10:10:10:0", NULL);
	set_mcps_in6_addr("o@10:10:10:10:10:10:10:1", NULL);
	set_mcps_in6_addr("2@", NULL);
	set_mcps_in6_addr("-3@10:10:10:10:10:10:10:3", NULL);
	set_mcps_in6_addr("4@10:10:10:10:10:10:10:4", NULL);
	set_mcps_in6_addr("5@10:10:10:10:10:10:10:5", NULL);
	set_mcps_in6_addr("6@10:10:10:10:10:10:10:6", NULL);
	set_mcps_in6_addr("7@10:10:10:10:10:10:10:7", NULL);
	set_mcps_in6_addr("8@10:10:10:10:10:10:10:8", NULL);

	get_mcps_in6_addr(buffer, NULL);

	//Assert(Then)
	EXPECT_STREQ(test, buffer, "0001:0001:0001:0001:0001:0001:0001:0000\n0001:0001:0001:0001:0001:0001:0001:0001\n0001:0001:0001:0001:0001:0001:0001:0002\n0001:0001:0001:0001:0001:0001:0001:0003\n0010:0010:0010:0010:0010:0010:0010:0004\n0010:0010:0010:0010:0010:0010:0010:0005\n0010:0010:0010:0010:0010:0010:0010:0006\n0010:0010:0010:0010:0010:0010:0010:0007\n");
}
#endif
static void kunit_mcps_flush(struct test *test)
{
	//Arrange(Given)
	int ret0, ret1 = ret0 = 1;

	//Act(when)
	ret0 = set_mcps_flush("hello", NULL);
	ret1 = get_mcps_flush(NULL, NULL);

	//Assert(Then)
	EXPECT_EQ(test, ret0, 0);
	EXPECT_EQ(test, ret1, 0);
}

static void kunit_set_mcps_mask_cpu_when_input_nothing_do_not_change_any(struct test *test)
{
	if (buffer == NULL) {
		return;
	}

	//Arrange(Given)
	set_mcps_dynamic_cpu("FF", NULL);
	set_mcps_newflow_cpu("FF", NULL);
	set_mcps_arps_cpu("FF", NULL);

	//Act(when)
	set_mcps_dynamic_cpu("", NULL);
	set_mcps_newflow_cpu("", NULL);
	set_mcps_arps_cpu("", NULL);

	//Assert(Then)
	get_mcps_dynamic_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "ff\n[8|4|4|2]\n");
	get_mcps_newflow_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "ff\n[8|4|4|2]\n");
	get_mcps_arps_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "ff\n[8|4|4|2]\n");
}

static void kunit_set_mcps_mask_cpu_when_input_wrong_mask_set_empty_and_null(struct test *test)
{
	if (buffer == NULL) {
		return;
	}

	//Arrange(Given)
	set_mcps_dynamic_cpu("FF", NULL);
	set_mcps_newflow_cpu("FF", NULL);
	set_mcps_arps_cpu("FF", NULL);

	//Act(when)
	set_mcps_dynamic_cpu("GG", NULL);
	set_mcps_newflow_cpu("gg", NULL);
	set_mcps_arps_cpu("@#", NULL);

	//Assert(Then)
	get_mcps_dynamic_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "0\n[0|0|0|0]\n");
	get_mcps_newflow_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "0\n[0|0|0|0]\n");
	get_mcps_arps_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "0\n[0|0|0|0]\n");
}

static void kunit_set_mcps_mask_cpu_when_input_correct_mask_set_successful(struct test *test)
{
	if (buffer == NULL) {
		return;
	}

	//Arrange(Given)
	set_mcps_dynamic_cpu("FF", NULL);
	set_mcps_newflow_cpu("FF", NULL);
	set_mcps_arps_cpu("FF", NULL);

	//Act(when)
	set_mcps_dynamic_cpu("F0", NULL);
	set_mcps_newflow_cpu("0F", NULL);
	set_mcps_arps_cpu("56", NULL);

	//Assert(Then)
	get_mcps_dynamic_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "f0\n[4|0|4|2]\n");
	get_mcps_newflow_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "0f\n[4|4|0|0]\n");
	get_mcps_arps_cpu(buffer, NULL);
	EXPECT_STREQ(test, buffer, "56\n[4|2|2|1]\n");
}

static void kunit_mcps_arps_config(struct test *test)
{
	if (buffer == NULL) {
		return;
	}
	//Arrange(Given)

	//Act (When)
	set_mcps_arps_config("1 1 1 1 1", NULL);

	//Assert(Then)
	get_mcps_arps_config(buffer, NULL);
	EXPECT_STREQ(test, buffer, "1 1 1 1 1 \n");

	//Act (When)
	set_mcps_arps_config("2 2 2 2 2 2", NULL);

	//Assert(Then)
	get_mcps_arps_config(buffer, NULL);
	EXPECT_STREQ(test, buffer, "2 2 2 2 2 \n");

	//Act (When)
	set_mcps_arps_config("-1 3 3 3 3", NULL);

	//Assert(Then)
	get_mcps_arps_config(buffer, NULL);
	EXPECT_STREQ(test, buffer, "2 2 2 2 2 \n");

	//Act (When)
	set_mcps_arps_config("4    4    4    4       4", NULL);

	//Assert(Then)
	get_mcps_arps_config(buffer, NULL);
	EXPECT_STREQ(test, buffer, "2 2 2 2 2 \n");

	//Act (When)
	set_mcps_arps_config("5555555555555 55 5 5 5", NULL);

	//Assert(Then)
	get_mcps_arps_config(buffer, NULL);
	EXPECT_STREQ(test, buffer, "2 2 2 2 2 \n");

	//Act (When)
	set_mcps_arps_config("6 6 6 6", NULL);

	//Assert(Then)
	get_mcps_arps_config(buffer, NULL);
	EXPECT_STREQ(test, buffer, "6 6 6 6 0 \n");

	//Act (When)
	set_mcps_arps_config("s e v e n", NULL);

	//Assert(Then)
	get_mcps_arps_config(buffer, NULL);
	EXPECT_STREQ(test, buffer, "6 6 6 6 0 \n");

	//Act (When)
	set_mcps_arps_config("", NULL);

	//Assert(Then)
	get_mcps_arps_config(buffer, NULL);
	EXPECT_STREQ(test, buffer, "6 6 6 6 0 \n");
}

static void kunit_get_mcps_stats(struct test *test)
{
	int len = 0;
	if (buffer == NULL) {
		return;
	}

	//Arrange(Given)
	//Act(when)
	//Assert(Then)
	len = get_mcps_heavy_flow(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
	len = get_mcps_light_flow(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
	len = get_mcps_enqueued(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
	len = get_mcps_processed(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
	len = get_mcps_dropped(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
	len = get_mcps_ignored(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
	len = get_mcps_distributed(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
	len = get_mcps_sauron_target_flow(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
	len = get_mcps_sauron_flow(buffer, NULL);
	EXPECT_EQ(test, len, strlen(buffer));
}

static void kunit_init_sauron(struct test *test)
{
	//Arrange(Given)
	mock_sauron = (struct sauron *)kzalloc(sizeof(struct sauron), GFP_KERNEL);
	if (mock_sauron == NULL) {
		return;
	}
	//Act (When)
	init_sauron(mock_sauron);
	//Assert(Then)
	EXPECT_TRUE(test, (mock_sauron->sauron_eyes[0].first == NULL));

	kfree(mock_sauron);
}

static void kunit_create_and_init_arps_config_then_release(struct test *test)
{
	//Arrange(Given)
	struct arps_config *config = NULL;
	struct mcps_config *mcps = (struct mcps_config *)kzalloc(sizeof(struct mcps_config), GFP_KERNEL);
	if (mcps == NULL) {
		return;
	}
	//Act (When)
	if (create_and_init_arps_config(mcps)) {
		return;
	}

	if (create_and_init_arps_config(mcps)) {
		return;
	}

	//Assert(Then)
	rcu_read_lock();
	config = rcu_dereference(mcps->arps_config);
	if (config == NULL) {
		release_arps_config(mcps);
		kfree(mcps);
		EXPECT_TRUE(test, (0));
		return;
	}

	EXPECT_EQ(test, config->weights[0], 5);
	EXPECT_EQ(test, config->weights[1], 5);
	EXPECT_EQ(test, config->weights[2], 2);
	EXPECT_EQ(test, config->weights[3], 5);
	EXPECT_EQ(test, config->weights[4], 5);
	rcu_read_unlock();

	//Act (When)
	release_arps_config(mcps);

	//Assert(Then)
	rcu_read_lock();
	config = rcu_dereference(mcps->arps_config);
	EXPECT_TRUE(test, (config == NULL));
	rcu_read_unlock();

	kfree(mcps);
}
#endif //#if !defined(CONFIG_UML)

/* NOTE: UML TC */
static void mcps_params_test_bar(struct test *test)
{
	return;
}

static int mcps_params_test_init(struct test *test)
{
	buffer = (char *)kzalloc(PAGE_SIZE, GFP_KERNEL);
	return 0;
}

static void mcps_params_test_exit(struct test *test)
{
	kfree(buffer);
}

static struct test_case mcps_params_test_cases[] = {
#if !defined(CONFIG_UML)
	/* NOTE: Target running TC */
#if defined(CONFIG_MCPS_ICGB) && \
	!defined(CONFIG_KLAT) && \
	(defined(CONFIG_SOC_EXYNOS9830) || defined(CONFIG_SOC_EXYNOS9820))
	TEST_CASE(kunit_check_mcps_in_addr),
	TEST_CASE(kunit_check_mcps_in6_addr),
	TEST_CASE(kunit_mcps_in_addr),
	TEST_CASE(kunit_mcps_in6_addr),
#endif
	TEST_CASE(kunit_mcps_flush),
	TEST_CASE(kunit_set_mcps_mask_cpu_when_input_nothing_do_not_change_any),
	TEST_CASE(kunit_set_mcps_mask_cpu_when_input_wrong_mask_set_empty_and_null),
	TEST_CASE(kunit_set_mcps_mask_cpu_when_input_correct_mask_set_successful),
	TEST_CASE(kunit_get_mcps_stats),
	TEST_CASE(kunit_init_sauron),
	TEST_CASE(kunit_create_and_init_arps_config_then_release),
	TEST_CASE(kunit_mcps_arps_config),
#endif
	/* NOTE: UML TC */
	TEST_CASE(mcps_params_test_bar),
	{},
};

static struct test_module mcps_params_test_module = {
	.name = "mcps_params_test",
	.init = mcps_params_test_init,
	.exit = mcps_params_test_exit,
	.test_cases = mcps_params_test_cases,
};

module_test(mcps_params_test_module);
