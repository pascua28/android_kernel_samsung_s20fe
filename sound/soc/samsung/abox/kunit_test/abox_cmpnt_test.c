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

struct abox_cmpnt_test_context {
	struct snd_soc_card *card;
	struct abox_data *adp;
};

static void abox_cmpnt_test_of_cal_ofactor(struct test *test)
{
	unsigned int isr, osr, ofactor;
	const struct asrc_ctrl *ctrl = &asrc_table[0][0];

	isr = (ctrl->isr / 100) << ctrl->ovsf;
	osr = (ctrl->osr / 100) << ctrl->dcmf;
	ofactor = ctrl->ifactor * isr / osr;
	EXPECT_EQ(test, ofactor, cal_ofactor(ctrl));
}

static void abox_cmpnt_test_of_is_os_limit(struct test *test)
{
	EXPECT_EQ(test, 5, is_limit(100));
	EXPECT_EQ(test, 5, os_limit(100));
}

static void abox_cmpnt_test_of_sif_idx(struct test *test)
{
	enum ABOX_CONFIGMSG configmsg;
	int result;

	configmsg = SET_SIFS0_FORMAT;
	result = configmsg - SET_SIFS0_FORMAT;
	EXPECT_EQ(test, result, sif_idx(configmsg));
}

static void abox_cmpnt_test_of_get_set_sif_rate(struct test *test)
{
	unsigned int tmpRate, val;
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	val = get_sif_rate(tc->adp, SET_SIFS0_FORMAT);
	EXPECT_LE(test, 0, val);
	tmpRate = 36000;
	set_sif_rate(tc->adp, SET_SIFS0_FORMAT, tmpRate);
	EXPECT_EQ(test, tmpRate, get_sif_rate(tc->adp, SET_SIFS0_FORMAT));
	set_sif_rate(tc->adp, SET_SIFS0_FORMAT, val);
}

static void abox_cmpnt_test_of_get_set_sif_format(struct test *test)
{
	snd_pcm_format_t tmpFormat, val;
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	val = get_sif_format(tc->adp, SET_SIFS0_FORMAT);
	EXPECT_LE(test, 0, val);
	tmpFormat = SNDRV_PCM_FORMAT_S16;
	set_sif_format(tc->adp, SET_SIFS0_FORMAT, tmpFormat);
	EXPECT_EQ(test, tmpFormat, get_sif_format(tc->adp, SET_SIFS0_FORMAT));
	set_sif_format(tc->adp, SET_SIFS0_FORMAT, val);
}

static void abox_cmpnt_test_of_get_sif_physical_width(struct test *test)
{
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	EXPECT_LE(test, 0, get_sif_physical_width(tc->adp, SET_SIFS0_FORMAT));
}

static void abox_cmpnt_test_of_get_set_sif_width(struct test *test)
{
	int val, i;
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	val = get_sif_width(tc->adp, SET_SIFS0_FORMAT);
	EXPECT_LE(test, 0, val);
	set_sif_width(tc->adp, SET_SIFS0_FORMAT, 8);

	EXPECT_EQ(test, snd_pcm_format_width(SNDRV_PCM_FORMAT_S16),
			get_sif_width(tc->adp, SET_SIFS0_FORMAT));
	for (i = 16; i <= 32; i += 8) {
		set_sif_width(tc->adp, SET_SIFS0_FORMAT, i);
		EXPECT_EQ(test, i, get_sif_width(tc->adp, SET_SIFS0_FORMAT));
	}
	set_sif_width(tc->adp, SET_SIFS0_FORMAT, val);
}

static void abox_cmpnt_test_of_get_set_sif_channels(struct test *test)
{
	int tmpChan, val;
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	val = get_sif_channels(tc->adp, SET_SIFS0_FORMAT);
	EXPECT_LE(test, 0, val);
	tmpChan = 4;
	set_sif_channels(tc->adp, SET_SIFS0_FORMAT, tmpChan);
	EXPECT_EQ(test, tmpChan, get_sif_channels(tc->adp, SET_SIFS0_FORMAT));
	set_sif_channels(tc->adp, SET_SIFS0_FORMAT, val);
}

static void abox_cmpnt_test_of_rate_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SIFS0 Rate");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_EQ(test, 0, rate_get(kcontrol, &ev));
	EXPECT_LE(test, 0, rate_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_width_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SIFS0 Width");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_EQ(test, 0, width_get(kcontrol, &ev));
	EXPECT_LE(test, 0, width_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_channels_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SIFS0 Channel");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_EQ(test, 0, channels_get(kcontrol, &ev));
	EXPECT_LE(test, 0, channels_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_audio_mode_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX Audio Mode");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_EQ(test, 0, audio_mode_get(kcontrol, &ev));
	EXPECT_LE(test, 0, audio_mode_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_sound_type_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX Sound Type");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_EQ(test, 0, sound_type_get(kcontrol, &ev));
	EXPECT_LE(test, 0, sound_type_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_display_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX Display");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_EQ(test, 0, display_get(kcontrol, &ev));
	EXPECT_LE(test, 0, display_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_tickle_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX Tickle");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_EQ(test, 0, tickle_get(kcontrol, &ev));
	EXPECT_LE(test, 0, tickle_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_asrc_factor_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX ASRC Factor CP");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_EQ(test, 0, asrc_factor_get(kcontrol, &ev));
	EXPECT_LE(test, 0, asrc_factor_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_spus_asrc_enable_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	ev.value.integer.value[0] = spus_asrc_force_enable[0] ? 0 : 1;
	EXPECT_LE(test, 0, spus_asrc_enable_put(kcontrol, &ev));
	ev.value.integer.value[0] = spus_asrc_force_enable[0] ? 0 : 1;
	EXPECT_LE(test, 0, spus_asrc_enable_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_spum_asrc_enable_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUM ASRC0");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	ev.value.integer.value[0] = spum_asrc_force_enable[0] ? 0 : 1;
	EXPECT_LE(test, 0, spum_asrc_enable_put(kcontrol, &ev));
	ev.value.integer.value[0] = spum_asrc_force_enable[0] ? 0 : 1;
	EXPECT_LE(test, 0, spum_asrc_enable_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_spus_spum_asrc_id_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spus_asrc_id_put(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUM ASRC0");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spum_asrc_id_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_get_apf_coef(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	int coef;

	tc = test->priv;
	coef = get_apf_coef(tc->adp, 0, 0);
	EXPECT_LE(test, 0, coef);
	set_apf_coef(tc->adp, 0, 0, 1);
	EXPECT_EQ(test, 1, get_apf_coef(tc->adp, 0, 0));
	set_apf_coef(tc->adp, 0, 0, coef);
}

static void abox_cmpnt_test_of_asrc_apf_coef_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card,
			"ABOX SPUS ASRC0 APF COEF");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, asrc_apf_coef_get(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card,
			"ABOX SPUS ASRC0 APF COEF");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, asrc_apf_coef_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_wake_lock_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX Wakelock");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, wake_lock_get(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX Wakelock");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, wake_lock_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_reset_log_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX Reset Log");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, reset_log_get(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX Reset Log");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, reset_log_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_spus_asrc_os_is_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0 OS");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spus_asrc_os_get(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0 OS");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spus_asrc_os_put(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0 IS");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spus_asrc_is_get(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0 IS");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spus_asrc_is_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_spum_asrc_os_is_get_put(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_kcontrol *kcontrol;
	struct snd_ctl_elem_value ev;

	tc = test->priv;
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0 OS");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spum_asrc_os_get(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0 OS");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spum_asrc_os_put(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0 IS");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spum_asrc_is_get(kcontrol, &ev));
	kcontrol = snd_soc_card_get_kcontrol(tc->card, "ABOX SPUS ASRC0 IS");
	if (!kcontrol)
		FAIL(test, "KCONTROL IS EMPTY");
	EXPECT_LE(test, 0, spum_asrc_is_put(kcontrol, &ev));
}

static void abox_cmpnt_test_of_asrc_cache_find_get_widget(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_soc_dapm_widget *w;
	int idx, stream;

	tc = test->priv;
	idx = 3;
	stream = SNDRV_PCM_STREAM_PLAYBACK;
	w = asrc_get_widget(tc->adp->cmpnt, idx, stream);
	EXPECT_NOT_NULL(test, w);
	EXPECT_EQ(test, w, asrc_find_widget(tc->card, idx, stream));
	asrc_cache_widget(w, idx, stream);
	EXPECT_EQ(test, spus_asrc_widgets[idx], w);
}

static void abox_cmpnt_test_of_is_direct_connection(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	unsigned int val;

	tc = test->priv;
	val = 0;
	snd_soc_component_read(tc->adp->cmpnt, ABOX_ROUTE_CTRL2, &val);
	val &= ABOX_RSRC_CONNECTION_TYPE_MASK(ABOX_RSRC1 - ABOX_RSRC0);
	val >>= ABOX_RSRC_CONNECTION_TYPE_L(ABOX_RSRC1 - ABOX_RSRC0);
	EXPECT_EQ(test, !val, is_direct_connection(tc->adp->cmpnt, ABOX_RSRC1));

	val = 0;
	snd_soc_component_read(tc->adp->cmpnt, ABOX_ROUTE_CTRL2, &val);
	val &= ABOX_NSRC_CONNECTION_TYPE_MASK(ABOX_NSRC7 - ABOX_NSRC0);
	val >>= ABOX_NSRC_CONNECTION_TYPE_L(ABOX_NSRC7 - ABOX_NSRC0);
	EXPECT_EQ(test, !val, is_direct_connection(tc->adp->cmpnt, ABOX_NSRC7));

	EXPECT_FALSE(test, is_direct_connection(tc->adp->cmpnt, ABOX_RSRC0-1));
}

static void abox_cmpnt_test_of_get_source_sink_dai_id(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	int i;

	tc = test->priv;
	for (i = 0; i < 0x122; i++) {
		EXPECT_LE(test, 0, get_source_dai_id(tc->adp, i));
		EXPECT_LE(test, 0, get_sink_dai_id(tc->adp, i));
	}
}

static void abox_cmpnt_test_of_find_dai(struct test *test)
{
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	EXPECT_EQ(test, NULL, find_dai(tc->card, 0x999));
	EXPECT_NOT_NULL(test, find_dai(tc->card, ABOX_RDMA0));
}

static void abox_cmpnt_test_of_get_configmsg(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	enum ABOX_CONFIGMSG msg, format;

	tc = test->priv;
	EXPECT_LE(test, 0, get_configmsg(ABOX_SIFS0, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_SIFS1, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_SIFS2, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_SIFS3, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_SIFS4, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_SIFS5, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_RSRC0, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_RSRC1, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_NSRC0, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_NSRC1, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_NSRC2, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_NSRC3, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_NSRC4, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_NSRC5, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_NSRC6, &msg, &format));
	EXPECT_LE(test, 0, get_configmsg(ABOX_NSRC7, &msg, &format));
	EXPECT_GT(test, 0, get_configmsg(ABOX_SIFS0-10, &msg, &format));
}

static void abox_cmpnt_test_of_sifsx_cnt_val(struct test *test)
{
	EXPECT_EQ(test, 3999, sifsx_cnt_val(32000000, 16000, 16, 2));
}

static void abox_cmpnt_test_of_abox_cmpnt_sif_get_dst_format(struct test *test)
{
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	EXPECT_EQ(test, 9, abox_cmpnt_sif_get_dst_format(tc->adp, 1, 0));
}

static void abox_cmpnt_test_of_event_test(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_soc_dapm_widget *widget;

	tc = test->priv;
	widget = asrc_find_widget(tc->card, 1, 0);
	if (!widget) {
		EXPECT_NOT_NULL(test, widget);
		return;
	}
	EXPECT_LE(test, 0, spus_in0_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in1_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in2_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in3_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in4_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in5_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in6_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in7_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in8_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in9_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in10_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, spus_in11_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, sifs0_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, sifs1_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, sifs2_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, sifs3_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, sifs4_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, sifs5_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, nsrc0_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, nsrc1_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, nsrc2_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, nsrc3_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
	EXPECT_LE(test, 0, nsrc4_event(widget, NULL, SND_SOC_DAPM_PRE_PMU));
}

static void abox_cmpnt_test_of_asrc_info_test(struct test *test)
{
	struct abox_cmpnt_test_context *tc;
	struct snd_soc_dapm_widget *w;
	int asrc_id;
	bool asrc_active;

	tc = test->priv;
	w = asrc_find_widget(tc->card, 1, 0);
	EXPECT_NOT_NULL(test, w);
	EXPECT_EQ(test, w->shift, asrc_get_idx(w));
	EXPECT_EQ(test, ERR_PTR(-EINVAL), asrc_get_kcontrol(-1, 0));
	EXPECT_EQ(test, &spus_asrc_controls[1], asrc_get_kcontrol(1, 0));
	EXPECT_EQ(test, ERR_PTR(-EINVAL), asrc_get_kcontrol(100, 0));
	EXPECT_EQ(test, &spum_asrc_controls[1], asrc_get_kcontrol(1, 1));
	EXPECT_EQ(test, ERR_PTR(-EINVAL), asrc_get_kcontrol(100, 1));

	EXPECT_EQ(test, ERR_PTR(-EINVAL), asrc_get_id_kcontrol(-1, 0));
	EXPECT_EQ(test, &spus_asrc_id_controls[1], asrc_get_id_kcontrol(1, 0));
	EXPECT_EQ(test, ERR_PTR(-EINVAL), asrc_get_id_kcontrol(100, 0));
	EXPECT_EQ(test, &spum_asrc_id_controls[1], asrc_get_id_kcontrol(1, 1));
	EXPECT_EQ(test, ERR_PTR(-EINVAL), asrc_get_id_kcontrol(100, 1));

	asrc_id = asrc_get_id(tc->adp->cmpnt, 1, 0);
	EXPECT_EQ(test, -EINVAL, asrc_get_id(tc->adp->cmpnt, 100, 0));
	EXPECT_FALSE(test, asrc_get_active(tc->adp->cmpnt, -1, 0));
	asrc_active = asrc_get_active(tc->adp->cmpnt, 1, 0);
	EXPECT_FALSE(test, asrc_get_active(tc->adp->cmpnt, 1, 0));
	EXPECT_LE(test, 0, asrc_get_idx_of_id(tc->adp->cmpnt, 1, 0));
	EXPECT_FALSE(test, asrc_get_id_active(tc->adp->cmpnt, -1, 0));
	EXPECT_FALSE(test, asrc_get_id_active(tc->adp->cmpnt, 1, 0));

	EXPECT_NULL(test, asrc_put_id(tc->adp->cmpnt, 1, 0, asrc_id));
	EXPECT_EQ(test, 0, asrc_put_active(w, 0, asrc_active));
	EXPECT_LE(test, 0, asrc_exchange_id(tc->adp->cmpnt, 0, 1, 2));
}

static void abox_cmpnt_test_of_asrc_lock(struct test *test)
{
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	EXPECT_LE(test, 0, abox_cmpnt_asrc_lock(tc->adp,
				0, 1, asrc_get_id(tc->adp->cmpnt, 1, 0)));
	EXPECT_LE(test, 0, abox_cmpnt_asrc_lock(tc->adp,
				1, 1, asrc_get_id(tc->adp->cmpnt, 1, 1)));
	EXPECT_LE(test, 0, abox_cmpnt_asrc_lock(tc->adp,
				1, 1, asrc_get_id(tc->adp->cmpnt, 1, 0)));

	EXPECT_TRUE(test, asrc_is_lock(0, asrc_get_id(tc->adp->cmpnt, 1, 0)));
	EXPECT_TRUE(test, asrc_is_lock(0, asrc_get_id(tc->adp->cmpnt, 1, 1)));

	EXPECT_LE(test, -1, asrc_get_lock_id(0, 0));
	EXPECT_LE(test, -1, asrc_get_lock_id(1, 0));
}

static void abox_cmpnt_test_of_asrc_get_num(struct test *test)
{
	int sz;

	sz = sizeof(spus_asrc_sorted_id)/sizeof(*spus_asrc_sorted_id);
	EXPECT_EQ(test, sz, asrc_get_num(0));
	sz = sizeof(spum_asrc_sorted_id)/sizeof(*spum_asrc_sorted_id);
	EXPECT_EQ(test, sz, asrc_get_num(1));
}

static void abox_cmpnt_test_of_asrc_get_max_channels(struct test *test)
{
	EXPECT_EQ(test, spus_asrc_max_channels[0],
			asrc_get_max_channels(0, 0));
	EXPECT_EQ(test, spum_asrc_max_channels[0],
			asrc_get_max_channels(0, 1));
}

static void abox_cmpnt_test_of_asrc_get_sorted_id(struct test *test)
{
	EXPECT_EQ(test, spus_asrc_sorted_id[0], asrc_get_sorted_id(0, 0));
	EXPECT_EQ(test, spum_asrc_sorted_id[0], asrc_get_sorted_id(0, 1));
}

static void abox_cmpnt_test_of_asrc_get_set_channels(struct test *test)
{
	int spus_ch, spum_ch;

	spus_ch = asrc_get_channels(0, 0);
	spum_ch = asrc_get_channels(0, 1);
	EXPECT_EQ(test, spus_asrc_channels[0], spus_ch);
	EXPECT_EQ(test, spum_asrc_channels[0], spum_ch);

	asrc_set_channels(0, 0, 4);
	EXPECT_EQ(test, spus_asrc_channels[0], 4);
	asrc_set_channels(0, 0, spus_ch);
	asrc_set_channels(0, 1, 4);
	EXPECT_EQ(test, spum_asrc_channels[0], 4);
	asrc_set_channels(0, 1, spum_ch);
}

static void abox_cmpnt_test_of_cmpnt_probe_remove(struct test *test)
{
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	cmpnt_remove(tc->adp->cmpnt);
	cmpnt_probe(tc->adp->cmpnt);
	EXPECT_EQ(test, 0, strcmp(tc->adp->ws.name, "abox"));
}

static void abox_cmpnt_test_of_reset_update_cnt_val(struct test *test)
{
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	EXPECT_LE(test, 0, abox_cmpnt_reset_cnt_val(
			tc->adp->dev, tc->adp->cmpnt, ABOX_UAIF6));
	EXPECT_LE(test, 0, abox_cmpnt_reset_cnt_val(
			tc->adp->dev, tc->adp->cmpnt, ABOX_DSIF));
	EXPECT_GT(test, 0, abox_cmpnt_reset_cnt_val(
			tc->adp->dev, tc->adp->cmpnt, ABOX_DSIF+10));
	EXPECT_EQ(test, 0, abox_cmpnt_update_cnt_val(tc->adp->dev));
}
#endif

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int abox_cmpnt_test_init(struct test *test)
{
	struct abox_cmpnt_test_context *tc;

	tc = test_kzalloc(test, sizeof(*tc), GFP_KERNEL);
	test->priv = tc;
	if (!tc) {
		FAIL(test, "failed to allocate memory");
		return -ENOMEM;
	}
	tc->adp = abox_get_abox_data();
	if (!tc->adp) {
		FAIL(test, "failed to get abox_data");
		return -EFAULT;
	}
	if (!tc->adp->dev) {
		FAIL(test, "failed to get dev from abox_data");
		return -EFAULT;
	}
	if (!tc->adp->cmpnt) {
		FAIL(test, "failed to get cmpnt from abox_data");
		return -EFAULT;
	}
	tc->card = tc->adp->cmpnt->card;
	if (!tc->card) {
		FAIL(test, "failed to get card from abox compnt");
		return -EFAULT;
	}
	abox_request_cpu_gear(tc->adp->dev, tc->adp,
		TEST_CPU_GEAR_ID, 4, "calliope_cmd");
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void abox_cmpnt_test_exit(struct test *test)
{
	struct abox_cmpnt_test_context *tc;

	tc = test->priv;
	abox_request_cpu_gear(tc->adp->dev, tc->adp,
		TEST_CPU_GEAR_ID, 12, "calliope_cmd");
}

/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case abox_cmpnt_test_cases[] = {
	/*
	 * This is a helper to create a test case object from a test case
	 * function; its exact function is not important to understand how to
	 * use KUnit, just know that this is how you associate test cases with a
	 * test module.
	 */
#if !defined(CONFIG_UML)
	/* NOTE: Target running TC */
	TEST_CASE(abox_cmpnt_test_of_cal_ofactor),
	TEST_CASE(abox_cmpnt_test_of_is_os_limit),
	TEST_CASE(abox_cmpnt_test_of_sif_idx),
	TEST_CASE(abox_cmpnt_test_of_get_set_sif_rate),
	TEST_CASE(abox_cmpnt_test_of_get_set_sif_format),
	TEST_CASE(abox_cmpnt_test_of_get_sif_physical_width),
	TEST_CASE(abox_cmpnt_test_of_get_set_sif_width),
	TEST_CASE(abox_cmpnt_test_of_get_set_sif_channels),
	TEST_CASE(abox_cmpnt_test_of_rate_get_put),
	TEST_CASE(abox_cmpnt_test_of_width_get_put),
	TEST_CASE(abox_cmpnt_test_of_channels_get_put),
	TEST_CASE(abox_cmpnt_test_of_audio_mode_get_put),
	TEST_CASE(abox_cmpnt_test_of_sound_type_get_put),
	TEST_CASE(abox_cmpnt_test_of_display_get_put),
	TEST_CASE(abox_cmpnt_test_of_tickle_get_put),
	TEST_CASE(abox_cmpnt_test_of_asrc_factor_get_put),
	TEST_CASE(abox_cmpnt_test_of_spus_asrc_enable_put),
	TEST_CASE(abox_cmpnt_test_of_spum_asrc_enable_put),
	TEST_CASE(abox_cmpnt_test_of_spus_spum_asrc_id_put),
	TEST_CASE(abox_cmpnt_test_of_get_apf_coef),
	TEST_CASE(abox_cmpnt_test_of_asrc_apf_coef_get_put),
	TEST_CASE(abox_cmpnt_test_of_wake_lock_get_put),
	TEST_CASE(abox_cmpnt_test_of_reset_log_get_put),
	TEST_CASE(abox_cmpnt_test_of_spus_asrc_os_is_get_put),
	TEST_CASE(abox_cmpnt_test_of_spum_asrc_os_is_get_put),
	TEST_CASE(abox_cmpnt_test_of_asrc_cache_find_get_widget),
	TEST_CASE(abox_cmpnt_test_of_is_direct_connection),
	TEST_CASE(abox_cmpnt_test_of_get_source_sink_dai_id),
	TEST_CASE(abox_cmpnt_test_of_find_dai),
	TEST_CASE(abox_cmpnt_test_of_get_configmsg),
	TEST_CASE(abox_cmpnt_test_of_sifsx_cnt_val),
	TEST_CASE(abox_cmpnt_test_of_abox_cmpnt_sif_get_dst_format),
	TEST_CASE(abox_cmpnt_test_of_event_test),
	TEST_CASE(abox_cmpnt_test_of_asrc_info_test),
	TEST_CASE(abox_cmpnt_test_of_asrc_lock),
	TEST_CASE(abox_cmpnt_test_of_asrc_get_num),
	TEST_CASE(abox_cmpnt_test_of_asrc_get_max_channels),
	TEST_CASE(abox_cmpnt_test_of_asrc_get_sorted_id),
	TEST_CASE(abox_cmpnt_test_of_asrc_get_set_channels),
	TEST_CASE(abox_cmpnt_test_of_cmpnt_probe_remove),
	TEST_CASE(abox_cmpnt_test_of_reset_update_cnt_val),
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
static struct test_module abox_cmpnt_test_module = {
	.name = "abox_cmpnt_test",
	.init = abox_cmpnt_test_init,
	.exit = abox_cmpnt_test_exit,
	.test_cases = abox_cmpnt_test_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(abox_cmpnt_test_module);
