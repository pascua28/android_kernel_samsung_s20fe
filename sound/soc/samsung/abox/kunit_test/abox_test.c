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

static void abox_test_of_abox_get_abox_data(struct test *test)
{
	EXPECT_NOT_NULL(test, p_abox_data->dev);
	EXPECT_TRUE(test, is_abox(p_abox_data->dev));
}

static void abox_test_of_abox_get_data(struct test *test)
{
	test_info(test, "test abox.%s - if null", __func__);
	EXPECT_NOT_NULL(test, abox_get_data(p_abox_data->dev));
}

static void abox_test_of_abox_cpu_enable(struct test *test)
{
	if (abox_is_on()) {
		test_info(test, "test abox.%s - on true", __func__);
		EXPECT_EQ(test, 0, abox_cpu_enable(true));
	} else {
		test_info(test, "test abox.%s - on false", __func__);
		EXPECT_EQ(test, 0, abox_cpu_enable(false));
	}
}

static void abox_test_of_is_abox(struct test *test)
{
	test_info(test, "test abox.%s - abox_data->dev", __func__);
	EXPECT_TRUE(test, is_abox(p_abox_data->dev));
}

static void abox_test_of_abox_probe_quirks(struct test *test)
{
	static const struct {
		const char *str;
		unsigned int bit;
	} map[] = {
		{ABOX_QUIRK_STR_ARAM_MODE, ABOX_QUIRK_BIT_ARAM_MODE},
		{ABOX_QUIRK_STR_INT_SKEW, ABOX_QUIRK_BIT_INT_SKEW}
	};
	int i, ret;

	test_info(test, "test abox.%s(abox_data*, device_node*) (%d, %d)",
			 __func__, p_abox_data, p_abox_data->dev->of_node);
	abox_probe_quirks(p_abox_data, p_abox_data->dev->of_node);

	for (i = 0; i < ARRAY_SIZE(map); i++) {
		ret = of_property_match_string(p_abox_data->dev->of_node,
			"samsung,quirks", map[i].str);
		EXPECT_LE(test, ret >= 0 ? 1 : 0,
			p_abox_data->quirks & map[i].bit);
	}

}

static void abox_test_of_abox_disable_qchannel(struct test *test)
{
	test_info(test, "test abox.%s(device*, abox_data*, qchannel, disable) \
			(%d, %d, %d, %d)", __func__, p_abox_data->dev,
			p_abox_data, ABOX_BCLK_UAIF0, 0);
	EXPECT_EQ(test, 0, abox_disable_qchannel(p_abox_data->dev,
			p_abox_data, ABOX_BCLK_UAIF0, 0));
	EXPECT_EQ(test, 0, abox_disable_qchannel(p_abox_data->dev,
			p_abox_data, ABOX_BCLK_UAIF0, 1));
}

static void abox_test_of_abox_set_system_state(struct test *test)
{
	test_info(test, "test abox.%s(abox_data*, system_state, en) \
			(%d, %d, %d)",
			__func__, p_abox_data->dev, p_abox_data, 0, 0);
	EXPECT_EQ(test, 0, abox_set_system_state(
				p_abox_data, SYSTEM_CALL, 1));
	EXPECT_EQ(test, 0, abox_set_system_state(
				p_abox_data, SYSTEM_OFFLOAD, 1));
	EXPECT_EQ(test, 0, abox_set_system_state(
				p_abox_data, SYSTEM_DISPLAY_OFF, 1));
}

static void abox_test_of_abox_addr_to_phys_addr(struct test *test)
{
	test_info(test, "test abox.%s(abox_data*, addr) (%d, %d)",
				__func__, p_abox_data->dev, p_abox_data, 0);
	EXPECT_EQ(test, 0, abox_addr_to_phys_addr(p_abox_data, 0));
	EXPECT_NE(test, 0, abox_addr_to_phys_addr(p_abox_data, 0x80000000));
	EXPECT_NE(test, 0, abox_addr_to_phys_addr(p_abox_data, 0x81000000));
}

static void abox_test_of_abox_dram_addr_to_kernel_addr(struct test *test)
{
	test_info(test, "test abox.%s(abox_data*, iova) (%d, %d)",
				__func__, p_abox_data->dev, p_abox_data, 0);
	EXPECT_NE(test, 0, (int)abox_dram_addr_to_kernel_addr(
				p_abox_data, 0x81000000));
}

static void abox_test_of_abox_addr_to_kernel_addr(struct test *test)
{
	test_info(test, "test abox.%s(abox_data*, addr) (%d, %d)",
				__func__, p_abox_data->dev, p_abox_data, 0);
	EXPECT_EQ(test, 0, (int)abox_addr_to_kernel_addr(
				p_abox_data, 0));
	EXPECT_NE(test, 0, (int)abox_addr_to_kernel_addr(
				p_abox_data, 0x80000000));
	EXPECT_NE(test, 0, (int)abox_addr_to_kernel_addr(
				p_abox_data, 0x81000000));
}

static void abox_test_of_abox_iova_to_phys(struct test *test)
{
	test_info(test, "test abox.%s(device*, iova) (%d, %d)",
				__func__, p_abox_data->dev, p_abox_data, 0);
	EXPECT_EQ(test, 0, abox_iova_to_phys(p_abox_data->dev, 0));
	EXPECT_NE(test, 0, abox_iova_to_phys(p_abox_data->dev, 0x80000000));
	EXPECT_NE(test, 0, abox_iova_to_phys(p_abox_data->dev, 0x81000000));
}

static void abox_test_of_abox_iova_to_virt(struct test *test)
{
	test_info(test, "test abox.%s(device*, addr) (%d, %d)",
				__func__, p_abox_data->dev, p_abox_data, 0);
	EXPECT_EQ(test, 0, (int)abox_iova_to_virt(p_abox_data->dev, 0));
	EXPECT_NE(test, 0, (int)abox_iova_to_virt(
				p_abox_data->dev, 0x80000000));
	EXPECT_NE(test, 0, (int)abox_iova_to_virt(
				p_abox_data->dev, 0x81000000));
}

static void abox_test_of_abox_show_gpr_min(struct test *test)
{
	char *buf = (char *)test_kzalloc(test, 5000, GFP_KERNEL);

	test_info(test, "test abox.%s(char*, len) (%d, %d)",
				__func__, p_abox_data->dev, buf, 5000);
	EXPECT_LE(test, 0, abox_show_gpr_min(buf, 5000));
}

static void abox_test_of_abox_read_gpr(struct test *test)
{
	test_info(test, "test abox.%s(core_id, gpr_id) (%d, %d)",
				__func__, p_abox_data->dev, 0, 0);
	EXPECT_LE(test, 0, abox_read_gpr(0, 0));
}

static void abox_test_of_abox_of_get_addr(struct test *test)
{
	test_info(test, "test abox.%s(abox_data*, name, addr, size) \
				(%d, NAME, %d, %d)",
				__func__, p_abox_data, 0, 0);
	test_info(test, "test abox.%s(ipc-tx-area)", __func__);
	EXPECT_LE(test, 0, abox_of_get_addr(p_abox_data,
				p_abox_data->dev->of_node,
				"samsung,ipc-tx-area", &p_abox_data->ipc_tx_addr,
				&p_abox_data->ipc_tx_size));
	test_info(test, "test abox.%s(ipc-rx-area)", __func__);
	EXPECT_LE(test, 0, abox_of_get_addr(p_abox_data,
				p_abox_data->dev->of_node,
				"samsung,ipc-rx-area", &p_abox_data->ipc_rx_addr,
				&p_abox_data->ipc_rx_size));
	test_info(test, "test abox.%s(shm-area)", __func__);
	EXPECT_LE(test, 0, abox_of_get_addr(p_abox_data,
				p_abox_data->dev->of_node,
				"samsung,shm-area", &p_abox_data->shm_addr,
				&p_abox_data->shm_size));
}

static void abox_test_of___abox_ipc_queue_empty(struct test *test)
{
	test_info(test, "test abox.%s(abox_data*) (%d)",
			__func__, p_abox_data);
	EXPECT_EQ(test, (p_abox_data->ipc_queue_end ==
					p_abox_data->ipc_queue_start),
				__abox_ipc_queue_empty(p_abox_data));
}

static void abox_test_of___abox_ipc_queue_full(struct test *test)
{
	test_info(test, "test abox.%s(abox_data*) (%d)",
					__func__, p_abox_data);
	EXPECT_EQ(test, (((p_abox_data->ipc_queue_end + 1) %
				ARRAY_SIZE(p_abox_data->ipc_queue)) ==
					p_abox_data->ipc_queue_start),
				__abox_ipc_queue_full(p_abox_data));
}

static void abox_test_of_abox_can_calliope_ipc(struct test *test)
{
	test_info(test, "test abox.%s(device*, abox_data*) (%d, %d)",
			__func__, p_abox_data->dev, p_abox_data);
	switch (p_abox_data->calliope_state) {
	case CALLIOPE_DISABLING:
	case CALLIOPE_ENABLED:
	case CALLIOPE_ENABLING:
		EXPECT_TRUE(test, abox_can_calliope_ipc(
			p_abox_data->dev, p_abox_data));
		break;
	default:
		EXPECT_FALSE(test, abox_can_calliope_ipc(
			p_abox_data->dev, p_abox_data));
	}
}

static void abox_test_of_abox_is_on(struct test *test)
{
	test_info(test, "test abox.%s()", __func__);
	if (p_abox_data && p_abox_data->enabled)
		EXPECT_TRUE(test, abox_is_on());
	else
		EXPECT_FALSE(test, abox_is_on());
}

static void abox_test_of_abox_correct_pll_rate(struct test *test)
{
	long long ret = 0;
	long long src_rate = 2;
	int diff_ppb = 1000000000;

	ret = src_rate * (diff_ppb + 1000000000)/diff_ppb;
	test_info(test, "test abox.%s(device*, src_rate, diff_ppb) \
				(%d, %d, %d)",
			__func__, p_abox_data->dev, src_rate, diff_ppb);
	EXPECT_EQ(test, ret, abox_correct_pll_rate(
				p_abox_data->dev, src_rate, diff_ppb));
}

static void abox_test_of_abox_timer_volatile_reg(struct test *test)
{
	int i;
	unsigned int tc[] = { 0,
		ABOX_TIMER_CTRL0(0),
		ABOX_TIMER_CTRL1(0),
		ABOX_TIMER_PRESET_LSB(0),
		ABOX_TIMER_CTRL0(1),
		ABOX_TIMER_CTRL1(1),
		ABOX_TIMER_PRESET_LSB(1),
		ABOX_TIMER_CTRL0(2),
		ABOX_TIMER_CTRL1(2),
		ABOX_TIMER_PRESET_LSB(2),
		ABOX_TIMER_CTRL0(3),
		ABOX_TIMER_CTRL1(3),
		ABOX_TIMER_PRESET_LSB(3)
	};

	for (i = 0; i < sizeof(tc)/sizeof(*tc); i++) {
		test_info(test, "test abox.%s(device*, reg) (%d, %d)",
				__func__, p_abox_data->dev, tc[i]);
		if (i == 0)
			EXPECT_FALSE(test, abox_timer_volatile_reg(
						p_abox_data->dev, tc[i]));
		else
			EXPECT_TRUE(test, abox_timer_volatile_reg(
						p_abox_data->dev, tc[i]));
	}
}

static void abox_test_of_abox_timer_readable_reg(struct test *test)
{
	int i;
	unsigned int tc[] = { 0,
		ABOX_TIMER_CTRL0(0),
		ABOX_TIMER_CTRL1(0),
		ABOX_TIMER_PRESET_LSB(0),
		ABOX_TIMER_CTRL0(1),
		ABOX_TIMER_CTRL1(1),
		ABOX_TIMER_PRESET_LSB(1),
		ABOX_TIMER_CTRL0(2),
		ABOX_TIMER_CTRL1(2),
		ABOX_TIMER_PRESET_LSB(2),
		ABOX_TIMER_CTRL0(3),
		ABOX_TIMER_CTRL1(3),
		ABOX_TIMER_PRESET_LSB(3),
	};

	for (i = 0; i < sizeof(tc)/sizeof(*tc); i++) {
		test_info(test, "test abox.%s(device*, reg) (%d, %d)",
			__func__, p_abox_data->dev, tc[i]);
		if (i == 0)
			EXPECT_FALSE(test, abox_timer_readable_reg(
						p_abox_data->dev, tc[i]));
		else
			EXPECT_TRUE(test, abox_timer_readable_reg(
						p_abox_data->dev, tc[i]));
	}
}

static void abox_test_of_abox_timer_writeable_reg(struct test *test)
{
	int i;
	unsigned int tc[] = { 0,
		ABOX_TIMER_CTRL0(0),
		ABOX_TIMER_CTRL1(0),
		ABOX_TIMER_CTRL0(1),
		ABOX_TIMER_CTRL1(1),
		ABOX_TIMER_CTRL0(2),
		ABOX_TIMER_CTRL1(2),
		ABOX_TIMER_CTRL0(3),
		ABOX_TIMER_CTRL1(3)
	};

	for (i = 0; i < sizeof(tc)/sizeof(*tc); i++) {
		test_info(test, "test abox.%s(device*, reg) (%d, %d)",
				__func__, p_abox_data->dev, tc[i]);
		if (i == 0)
			EXPECT_FALSE(test,
				abox_timer_writeable_reg(
					p_abox_data->dev, tc[i]));
		else
			EXPECT_TRUE(test,
				abox_timer_writeable_reg(
					p_abox_data->dev, tc[i]));
	}
}

static void abox_test_of_abox_get_requiring_int_freq_in_khz(struct test *test)
{
	test_info(test, "test abox.%s()", __func__);
	EXPECT_LE(test, 0, abox_get_requiring_int_freq_in_khz());
}

static void abox_test_of_abox_get_requiring_mif_freq_in_khz(struct test *test)
{
	test_info(test, "test abox.%s()", __func__);
	EXPECT_LE(test, 0, abox_get_requiring_mif_freq_in_khz());
}

static void abox_test_of_abox_get_requiring_aud_freq_in_khz(struct test *test)
{
	test_info(test, "test abox.%s()", __func__);
	EXPECT_LE(test, 0, abox_get_requiring_aud_freq_in_khz());
}

static void abox_test_of_abox_get_routing_path(struct test *test)
{
	unsigned int result = 0;

	test_info(test, "test abox.%s()", __func__);
	result = abox_get_routing_path();
	EXPECT_LE(test, 0, result);
	EXPECT_LE(test, result, 5);
}

static void abox_test_of_abox_dma_irq_handler(struct test *test)
{
	test_info(test, "test abox.%s(irq, abox_data*) (%d, %d)",
			__func__, WDMA1_BUF_FULL, p_abox_data);
	EXPECT_LE(test, 0, abox_dma_irq_handler(
				WDMA1_BUF_FULL, p_abox_data));
}

static void abox_test_of_abox_print_power_usage(struct test *test)
{
	test_info(test, "test abox.%s(device*, abox_data) (%d, %d)",
			 __func__, p_abox_data->dev, p_abox_data);
	EXPECT_EQ(test, 0, abox_print_power_usage(
				p_abox_data->dev, p_abox_data));
}

static void abox_test_of_calliope_version_show(struct test *test)
{
	char buf[6];

	test_info(test, "test abox.%s(device*, device_attribute, char*) \
			(%d, %d, %d)", __func__, p_abox_data->dev, 0, buf);
	EXPECT_EQ(test, 6, calliope_version_show(p_abox_data->dev, NULL, buf));
}
#endif

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int abox_test_init(struct test *test)
{
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void abox_test_exit(struct test *test)
{
}

/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case abox_test_cases[] = {
	/*
	 * This is a helper to create a test case object from a test case
	 * function; its exact function is not important to understand how to
	 * use KUnit, just know that this is how you associate test cases with a
	 * test module.
	 */
#if !defined(CONFIG_UML)
	/* NOTE: Target running TC */
	TEST_CASE(abox_test_of_abox_get_abox_data),
	TEST_CASE(abox_test_of_abox_get_data),
	TEST_CASE(abox_test_of_abox_cpu_enable),
	TEST_CASE(abox_test_of_is_abox),
	TEST_CASE(abox_test_of_abox_probe_quirks),
	TEST_CASE(abox_test_of_abox_disable_qchannel),
	TEST_CASE(abox_test_of_abox_set_system_state),
	TEST_CASE(abox_test_of_abox_addr_to_phys_addr),
	TEST_CASE(abox_test_of_abox_dram_addr_to_kernel_addr),
	TEST_CASE(abox_test_of_abox_addr_to_kernel_addr),
	TEST_CASE(abox_test_of_abox_iova_to_phys),
	TEST_CASE(abox_test_of_abox_iova_to_virt),
	TEST_CASE(abox_test_of_abox_show_gpr_min),
	TEST_CASE(abox_test_of_abox_read_gpr),
	TEST_CASE(abox_test_of_abox_of_get_addr),
	TEST_CASE(abox_test_of___abox_ipc_queue_empty),
	TEST_CASE(abox_test_of___abox_ipc_queue_full),
	TEST_CASE(abox_test_of_abox_can_calliope_ipc),
	TEST_CASE(abox_test_of_abox_is_on),
	TEST_CASE(abox_test_of_abox_correct_pll_rate),
	TEST_CASE(abox_test_of_abox_timer_volatile_reg),
	TEST_CASE(abox_test_of_abox_timer_readable_reg),
	TEST_CASE(abox_test_of_abox_timer_writeable_reg),
	TEST_CASE(abox_test_of_abox_get_requiring_int_freq_in_khz),
	TEST_CASE(abox_test_of_abox_get_requiring_mif_freq_in_khz),
	TEST_CASE(abox_test_of_abox_get_requiring_aud_freq_in_khz),
	TEST_CASE(abox_test_of_abox_get_routing_path),
	TEST_CASE(abox_test_of_abox_dma_irq_handler),
	TEST_CASE(abox_test_of_abox_print_power_usage),
	TEST_CASE(abox_test_of_calliope_version_show),
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
static struct test_module abox_test_module = {
	.name = "abox_test",
	.init = abox_test_init,
	.exit = abox_test_exit,
	.test_cases = abox_test_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(abox_test_module);
