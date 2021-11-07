// SPDX-License-Identifier: GPL-2.0
/*
 * KUnit test for UFS Write Booster test in UML.
 */

#include <kunit/test.h>
#include <kunit/mock.h>

#include <linux/delay.h>
#include <linux/blkdev.h>

struct request_queue queue;



static void wb_on_test(struct test *test)
{
	long long on_bytes = queue.tw->up_threshold_bytes + 1;
	int on_rqs = queue.tw->up_threshold_rqs + 1;
	int on_delay_ms = jiffies_to_msecs(queue.tw->on_delay);

	blk_reset_tw_state(&queue);

	blk_update_tw_state(&queue, on_rqs - 1, on_bytes - 1);
	EXPECT_EQ(test, TW_OFF, queue.tw->state);

	blk_update_tw_state(&queue, on_rqs - 1, on_bytes);
	EXPECT_EQ(test, TW_ON_READY, queue.tw->state);

	msleep(on_delay_ms);
	blk_update_tw_state(&queue, on_rqs - 1, on_bytes - 1);
	EXPECT_EQ(test, TW_ON, queue.tw->state);

	blk_reset_tw_state(&queue);

	blk_update_tw_state(&queue, on_rqs - 1, on_bytes - 1);
	EXPECT_EQ(test, TW_OFF, queue.tw->state);

	blk_update_tw_state(&queue, on_rqs, on_bytes - 1);
	EXPECT_EQ(test, TW_ON_READY, queue.tw->state);

	msleep(on_delay_ms);
	blk_update_tw_state(&queue, on_rqs - 1, on_bytes - 1);
	EXPECT_EQ(test, TW_ON, queue.tw->state);
}

static void wb_off_test(struct test *test)
{
	long long on_bytes = queue.tw->up_threshold_bytes + 1;
	int on_rqs = queue.tw->up_threshold_rqs + 1;
	long long off_bytes = queue.tw->down_threshold_bytes - 1;
	int off_rqs = queue.tw->down_threshold_rqs - 1;
	int off_delay_ms = jiffies_to_msecs(queue.tw->off_delay);

	wb_on_test(test);
	EXPECT_EQ(test, TW_ON, queue.tw->state);

	blk_update_tw_state(&queue, off_rqs + 1, off_bytes);
	EXPECT_EQ(test, TW_ON, queue.tw->state);

	blk_update_tw_state(&queue, off_rqs, off_bytes + 1);
	EXPECT_EQ(test, TW_ON, queue.tw->state);

	blk_update_tw_state(&queue, off_rqs, off_bytes);
	EXPECT_EQ(test, TW_OFF_READY, queue.tw->state);

	blk_update_tw_state(&queue, on_rqs, on_bytes);
	EXPECT_EQ(test, TW_ON, queue.tw->state);

	blk_update_tw_state(&queue, off_rqs, off_bytes);
	EXPECT_EQ(test, TW_OFF_READY, queue.tw->state);

	msleep(off_delay_ms);
	blk_update_tw_state(&queue, off_rqs, off_bytes);
	EXPECT_EQ(test, TW_OFF, queue.tw->state);
}

static void wb_on_interval_test(struct test *test)
{
	long long on_bytes = queue.tw->up_threshold_bytes + 1;
	int on_rqs = queue.tw->up_threshold_rqs + 1;
	long long off_bytes = queue.tw->down_threshold_bytes - 1;
	int off_rqs = queue.tw->down_threshold_rqs - 1;
	int on_interval_ms = jiffies_to_msecs(queue.tw->on_interval);

	blk_reset_tw_state(&queue);

	blk_update_tw_state(&queue, on_rqs - 1, on_bytes);
	EXPECT_EQ(test, TW_ON_READY, queue.tw->state);

	blk_update_tw_state(&queue, on_rqs - 1, off_bytes);
	EXPECT_EQ(test, TW_ON_READY, queue.tw->state);

	blk_update_tw_state(&queue, off_rqs, off_bytes);
	EXPECT_EQ(test, TW_OFF, queue.tw->state);

	blk_update_tw_state(&queue, on_rqs, on_bytes);
	EXPECT_EQ(test, TW_ON_READY, queue.tw->state);

	blk_update_tw_state(&queue, off_rqs, on_bytes - 1);
	EXPECT_EQ(test, TW_ON, queue.tw->state);

	blk_reset_tw_state(&queue);

	blk_update_tw_state(&queue, on_rqs, on_bytes - 1);
	EXPECT_EQ(test, TW_ON_READY, queue.tw->state);

	blk_update_tw_state(&queue, off_rqs, off_bytes);
	EXPECT_EQ(test, TW_OFF, queue.tw->state);

	msleep(on_interval_ms);
	blk_update_tw_state(&queue, on_rqs, on_bytes);
	EXPECT_EQ(test, TW_ON_READY, queue.tw->state);

	blk_update_tw_state(&queue, on_rqs, on_bytes);
	EXPECT_EQ(test, TW_ON_READY, queue.tw->state);
}



static int blk_ufs_wb_test_init(struct test *test)
{
	memset(&queue, 0x0, sizeof(struct request_queue));
	blk_alloc_turbo_write(&queue);

	return 0;
}

static void blk_ufs_wb_test_exit(struct test *test)
{
}

static struct test_case blk_ufs_wb_test_cases[] = {
	TEST_CASE(wb_on_test),
	TEST_CASE(wb_off_test),
	TEST_CASE(wb_on_interval_test),
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
static struct test_module blk_ufs_wb_test_module = {
	.name = "blk_ufs_wb_test",
	.init = blk_ufs_wb_test_init,
	.exit = blk_ufs_wb_test_exit,
	.test_cases = blk_ufs_wb_test_cases,
};

module_test(blk_ufs_wb_test_module);
