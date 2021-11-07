// SPDX-License-Identifier: GPL-2.0
/*
 * TODO: Add test description.
 */

#include <kunit/test.h>
#include <kunit/mock.h>
#include "../include/sec_battery.h"

extern bool sec_bat_get_cable_type(struct sec_battery_info *battery, int cable_source_type);
extern void sec_bat_get_charging_current_by_siop(
		struct sec_battery_info *battery, int *input_current, int *charging_current);
extern void sec_bat_get_charging_current_in_power_list(struct sec_battery_info *battery);
extern void sec_bat_get_input_current_in_power_list(struct sec_battery_info *battery);
extern int sec_bat_get_temp_by_temp_control_source(struct sec_battery_info *battery, enum sec_battery_temp_control_source tcs);
//extern u8 sec_bat_get_wireless20_power_class(struct sec_battery_info *battery);
extern int sec_bat_get_wireless_current(struct sec_battery_info *battery, int incurr);
extern void sec_bat_handle_tx_misalign(struct sec_battery_info *battery, bool trigger_misalign);
extern bool sec_bat_hv_wc_normal_mode_check(struct sec_battery_info *battery);
extern bool sec_bat_set_aging_step(struct sec_battery_info *battery, int step);
extern bool sec_bat_predict_wireless20_time_to_full_current(struct sec_battery_info *battery, int step);
extern void sec_bat_set_charging_status(struct sec_battery_info *battery, int status);
extern void sec_bat_set_decrease_iout(struct sec_battery_info *battery, bool last_delay);
extern void sec_bat_set_mfc_off(struct sec_battery_info *battery, bool need_ept);
extern void sec_bat_set_mfc_on(struct sec_battery_info *battery, bool always_on);
extern void sec_bat_change_default_current(struct sec_battery_info *battery, int cable_type, int input, int output);
extern int sec_bat_check_afc_input_current(struct sec_battery_info *battery, int input_current);
extern void sec_bat_set_rp_current(struct sec_battery_info *battery, int cable_type);
extern void sec_bat_wc_cv_mode_check(struct sec_battery_info *battery);
extern int sec_bat_check_pd_input_current(struct sec_battery_info *battery, int input_current);
extern void sec_bat_set_wireless20_current(struct sec_battery_info *battery, int rx_power);

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
static void sec_battery_test_start(struct test *test)
{
	/*
	 * This is an EXPECTATION; it is how KUnit tests things. When you want
	 * to test a piece of code, you set some expectations about what the
	 * code should do. KUnit then runs the test and verifies that the code's
	 * behavior matched what was expected.
	 */
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	cancel_delayed_work(&battery->monitor_work);
	wake_unlock(&battery->monitor_wake_lock);
}

static void sec_battery_test_end(struct test *test)
{
	/*
	 * This is an EXPECTATION; it is how KUnit tests things. When you want
	 * to test a piece of code, you set some expectations about what the
	 * code should do. KUnit then runs the test and verifies that the code's
	 * behavior matched what was expected.
	 */
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	wake_lock(&battery->monitor_wake_lock);
	queue_delayed_work(battery->monitor_wqueue, &battery->monitor_work, 0);
}

static int fill_powerlist_pd20(PDIC_SINK_STATUS *sink_status, int max_voltage)
{
	int i = 1;

	sink_status->power_list[i].max_voltage = 5000;
	sink_status->power_list[i].max_current = 3000;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 0;

	sink_status->power_list[i].max_voltage = 7000;
	sink_status->power_list[i].max_current = 2000;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 0;

	sink_status->power_list[i].max_voltage = 9000;
	sink_status->power_list[i].max_current = 1666;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 0;

	sink_status->power_list[i].max_voltage = 12000;
	sink_status->power_list[i].max_current = 2000;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 0;

	sink_status->has_apdo = false;

	sink_status->available_pdo_num = i-1;

	if (max_voltage <= 5000)
		sink_status->selected_pdo_num = 1;
	else if (max_voltage <= 7000)
		sink_status->selected_pdo_num = 2;
	else if (max_voltage <= 9000)
		sink_status->selected_pdo_num = 3;
	else
		sink_status->selected_pdo_num = 4;

	return i-1;
}

static int fill_powerlist_pd30(PDIC_SINK_STATUS *sink_status, int max_voltage)
{
	int i = 1;

	sink_status->power_list[i].max_voltage = 5000;
	sink_status->power_list[i].max_current = 3000;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 0;

	sink_status->power_list[i].max_voltage = 9000;
	sink_status->power_list[i].max_current = 1666;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 0;

	sink_status->power_list[i].max_voltage = 12000;
	sink_status->power_list[i].max_current = 2000;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 0;

	sink_status->power_list[i].min_voltage = 3300;
	sink_status->power_list[i].max_voltage = 5900;
	sink_status->power_list[i].max_current = 3000;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 1;

	sink_status->power_list[i].min_voltage = 3300;
	sink_status->power_list[i].max_voltage = 1100;
	sink_status->power_list[i].max_current = 2250;
	sink_status->power_list[i].accept = 1;
	sink_status->power_list[i++].apdo = 1;

	sink_status->has_apdo = true;

	sink_status->available_pdo_num = i-1;

	if (max_voltage > 9000)
		sink_status->selected_pdo_num = 2;
	else 
		sink_status->selected_pdo_num = 1;

	return i-1;
}

extern int make_pd_list(struct sec_battery_info *battery);
static void kunit_sec_make_pd_list(struct test *test)
{
	struct power_supply *psy;
	struct sec_battery_info *battery;
	PDIC_SINK_STATUS sink_status_backup;

	psy = power_supply_get_by_name("battery");
	battery = power_supply_get_drvdata(psy);

	sink_status_backup = battery->pdic_info.sink_status;

	fill_powerlist_pd20(&battery->pdic_info.sink_status, battery->pdata->max_input_voltage);
	make_pd_list(battery);
	EXPECT_EQ(test, battery->pd_list.num_fpdo, 2);
	EXPECT_EQ(test, battery->pd_list.num_apdo, 0);
	EXPECT_EQ(test, battery->pd_list.max_pd_count, 2);

	fill_powerlist_pd30(&battery->pdic_info.sink_status, battery->pdata->max_input_voltage);
	make_pd_list(battery);
	EXPECT_EQ(test, battery->pd_list.num_fpdo, 2);
	EXPECT_EQ(test, battery->pd_list.num_apdo, 2);
	EXPECT_EQ(test, battery->pd_list.max_pd_count, 4);

	battery->pdic_info.sink_status = sink_status_backup;
}

static void kunit_sec_ac_get_property(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);
	struct power_supply *psy_ac = power_supply_get_by_name("ac");
	int cable_type_backup = battery->cable_type;
	int health_backup = battery->health;
	union power_supply_propval val;

	test_info(test, "START %s test\n", __func__);
	battery->cable_type = SEC_BATTERY_CABLE_TA;
	battery->health = POWER_SUPPLY_HEALTH_GOOD;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 1);
	battery->health = POWER_SUPPLY_HEALTH_UNDERVOLTAGE;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 0);
	battery->health = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 0);
	battery->health = POWER_SUPPLY_HEALTH_GOOD;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 1);
	battery->cable_type = SEC_BATTERY_CABLE_USB;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 0);
	battery->cable_type = SEC_BATTERY_CABLE_PDIC_APDO;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 1);
	battery->cable_type = SEC_BATTERY_CABLE_PDIC;
	battery->pd_usb_attached = 0;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 1);
	battery->cable_type = SEC_BATTERY_CABLE_PDIC;
	battery->pd_usb_attached = 1;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 0);
	battery->cable_type = SEC_BATTERY_CABLE_NONE;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 0);
	battery->misc_event |= BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 0);
	lpcharge = 1;
	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_ONLINE, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_EQ(test, val.intval, 1);
	lpcharge = 0;

	if (power_supply_get_property(psy_ac, POWER_SUPPLY_PROP_TEMP, &val) < 0)
		test_info(test, "POWER_SUPPLY_PROP_ONLINE get failed\n");
	EXPECT_LE(test, val.intval, 1000);
	EXPECT_GE(test, val.intval, -200);

	battery->misc_event &= ~(BATT_MISC_EVENT_HICCUP_TYPE|BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE);
	if (power_supply_get_property(psy_ac,
				(enum power_supply_property)POWER_SUPPLY_EXT_PROP_WATER_DETECT, &val) < 0)
		test_info(test, "POWER_SUPPLY_EXT_PROP_WATER_DETECT get failed\n");
	EXPECT_EQ(test, val.intval, 0);
	battery->misc_event |= BATT_MISC_EVENT_HICCUP_TYPE;
	if (power_supply_get_property(psy_ac,
				(enum power_supply_property)POWER_SUPPLY_EXT_PROP_WATER_DETECT, &val) < 0)
		test_info(test, "POWER_SUPPLY_EXT_PROP_WATER_DETECT get failed\n");
	EXPECT_EQ(test, val.intval, 1);
	battery->misc_event &= ~BATT_MISC_EVENT_HICCUP_TYPE;
	battery->misc_event |= BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE;
	if (power_supply_get_property(psy_ac,
				(enum power_supply_property)POWER_SUPPLY_EXT_PROP_WATER_DETECT, &val) < 0)
		test_info(test, "POWER_SUPPLY_EXT_PROP_WATER_DETECT get failed\n");
	EXPECT_EQ(test, val.intval, 1);

	battery->misc_event &= ~(BATT_MISC_EVENT_HICCUP_TYPE|BATT_MISC_EVENT_UNDEFINED_RANGE_TYPE);
	battery->cable_type = cable_type_backup;
	battery->health = health_backup;
}

void sec_bat_afc_work(struct work_struct *work);
static void kunit_sec_bat_afc_work(struct test *test)
{
	struct power_supply *psy;
	struct sec_battery_info *battery;

	psy = power_supply_get_by_name("battery");
	battery = power_supply_get_drvdata(psy);

	sec_bat_set_current_event(battery, SEC_BAT_CURRENT_EVENT_AFC, SEC_BAT_CURRENT_EVENT_AFC);
	sec_bat_afc_work((struct work_struct *)&battery->afc_work);
	EXPECT_EQ(test, (battery->current_event & SEC_BAT_CURRENT_EVENT_AFC), 0);
}

static void kunit_sec_bat_aging_check(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	sec_age_data_t* age_data = battery->pdata->age_data;
	int num_age_step = battery->pdata->num_age_step;
	int cycle = battery->batt_cycle, step = battery->pdata->age_step;
	int i;

	for (i = 0; i < num_age_step; i++) {
		battery->batt_cycle = age_data[i].cycle;
		sec_bat_aging_check(battery);
		EXPECT_EQ(test, battery->pdata->age_step, i);

		battery->batt_cycle = age_data[i].cycle + 1;
		sec_bat_aging_check(battery);
		EXPECT_EQ(test, battery->pdata->age_step, i);
	}

	battery->batt_cycle = cycle;
	sec_bat_aging_check(battery);
	EXPECT_EQ(test, battery->pdata->age_step, step);
 }

extern int sec_bat_cable_check(struct sec_battery_info *battery,
				muic_attached_dev_t attached_dev);
static void kunit_sec_bat_cable_check(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int table[][2] = {
		{ATTACHED_DEV_JIG_UART_OFF_MUIC, SEC_BATTERY_CABLE_NONE},
		{ATTACHED_DEV_JIG_UART_ON_MUIC, SEC_BATTERY_CABLE_NONE},
		{ATTACHED_DEV_SMARTDOCK_MUIC, SEC_BATTERY_CABLE_NONE},
		{ATTACHED_DEV_DESKDOCK_MUIC, SEC_BATTERY_CABLE_NONE},
		{ATTACHED_DEV_JIG_USB_ON_MUIC, SEC_BATTERY_CABLE_NONE},
		{ATTACHED_DEV_UNDEFINED_CHARGING_MUIC, SEC_BATTERY_CABLE_NONE},
		{ATTACHED_DEV_UNDEFINED_RANGE_MUIC, SEC_BATTERY_CABLE_NONE},
		{ATTACHED_DEV_OTG_MUIC, SEC_BATTERY_CABLE_OTG},
		{ATTACHED_DEV_JIG_UART_OFF_VB_OTG_MUIC, SEC_BATTERY_CABLE_OTG},
		{ATTACHED_DEV_HMT_MUIC, SEC_BATTERY_CABLE_OTG},
		{ATTACHED_DEV_TIMEOUT_OPEN_MUIC, SEC_BATTERY_CABLE_TIMEOUT},
		{ATTACHED_DEV_USB_MUIC, SEC_BATTERY_CABLE_USB},
		{ATTACHED_DEV_JIG_USB_OFF_MUIC, SEC_BATTERY_CABLE_USB},
		{ATTACHED_DEV_SMARTDOCK_USB_MUIC, SEC_BATTERY_CABLE_USB},
		{ATTACHED_DEV_UNOFFICIAL_ID_USB_MUIC, SEC_BATTERY_CABLE_USB},
		{ATTACHED_DEV_JIG_UART_ON_VB_MUIC, SEC_BATTERY_CABLE_UARTOFF},
		{ATTACHED_DEV_JIG_UART_OFF_VB_MUIC, SEC_BATTERY_CABLE_UARTOFF},
		{ATTACHED_DEV_JIG_UART_OFF_VB_FG_MUIC, SEC_BATTERY_CABLE_UARTOFF},
		{ATTACHED_DEV_RDU_TA_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_TA_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_CARDOCK_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_DESKDOCK_VB_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_SMARTDOCK_TA_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_UNOFFICIAL_TA_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_UNOFFICIAL_ID_TA_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_UNOFFICIAL_ID_ANY_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_UNSUPPORTED_ID_VB_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_AFC_CHARGER_DISABLED_MUIC, SEC_BATTERY_CABLE_TA},
		{ATTACHED_DEV_AFC_CHARGER_5V_MUIC, SEC_BATTERY_CABLE_HV_TA_CHG_LIMIT},
		{ATTACHED_DEV_QC_CHARGER_5V_MUIC, SEC_BATTERY_CABLE_HV_TA_CHG_LIMIT},
		{ATTACHED_DEV_AFC_CHARGER_5V_DUPLI_MUIC, SEC_BATTERY_CABLE_HV_TA_CHG_LIMIT},
		{ATTACHED_DEV_CDP_MUIC, SEC_BATTERY_CABLE_USB_CDP  },
		{ATTACHED_DEV_UNOFFICIAL_ID_CDP_MUIC, SEC_BATTERY_CABLE_USB_CDP  },
		{ATTACHED_DEV_USB_LANHUB_MUIC, SEC_BATTERY_CABLE_LAN_HUB             },
		{ATTACHED_DEV_CHARGING_CABLE_MUIC, SEC_BATTERY_CABLE_POWER_SHARING   },
		{ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC, SEC_BATTERY_CABLE_PREPARE_TA},
		{ATTACHED_DEV_QC_CHARGER_PREPARE_MUIC, SEC_BATTERY_CABLE_PREPARE_TA  },
		{ATTACHED_DEV_QC_CHARGER_9V_MUIC, SEC_BATTERY_CABLE_9V_TA            },
		{ATTACHED_DEV_AFC_CHARGER_9V_MUIC, SEC_BATTERY_CABLE_9V_TA      },
		{ATTACHED_DEV_AFC_CHARGER_9V_DUPLI_MUIC, SEC_BATTERY_CABLE_9V_TA},
#if defined(CONFIG_MUIC_HV_12V)
		{ATTACHED_DEV_AFC_CHARGER_12V_MUIC, SEC_BATTERY_CABLE_12V_TA},
		{ATTACHED_DEV_AFC_CHARGER_12V_DUPLI_MUIC, SEC_BATTERY_CABLE_12V_TA},
#endif
		{ATTACHED_DEV_AFC_CHARGER_ERR_V_MUIC, -1},
		{ATTACHED_DEV_AFC_CHARGER_ERR_V_DUPLI_MUIC, -1},
		{ATTACHED_DEV_QC_CHARGER_ERR_V_MUIC, -1},

		{ATTACHED_DEV_HV_ID_ERR_UNDEFINED_MUIC, SEC_BATTERY_CABLE_9V_UNKNOWN  },
		{ATTACHED_DEV_HV_ID_ERR_UNSUPPORTED_MUIC, SEC_BATTERY_CABLE_9V_UNKNOWN},
		{ATTACHED_DEV_HV_ID_ERR_SUPPORTED_MUIC, SEC_BATTERY_CABLE_9V_UNKNOWN  },
		{ATTACHED_DEV_VZW_INCOMPATIBLE_MUIC, SEC_BATTERY_CABLE_UNKNOWN         },
	};
	int res, i;
	int cnt = sizeof(table) / sizeof(int) / 2;
	int qc_fail = battery->cisd.cable_data[CISD_CABLE_QC_FAIL];
	int afc_fail = battery->cisd.cable_data[CISD_CABLE_AFC_FAIL];

	for (i = 0; i < cnt; i++) {
		res = sec_bat_cable_check(battery, table[i][0]);
		EXPECT_EQ(test, res, table[i][1]);
	}
	EXPECT_EQ(test, battery->cisd.cable_data[CISD_CABLE_QC_FAIL], qc_fail+1);
	EXPECT_EQ(test, battery->cisd.cable_data[CISD_CABLE_AFC_FAIL], afc_fail+2);
}

extern void sec_bat_cable_work(struct work_struct *work);
/* check mfc off in case of charging power over 4W */
static void kunit_sec_bat_cable_work_check_mfc_off(struct test *test,
		struct sec_battery_info *battery)
{
	union power_supply_propval val;
	int table[][2] = {{SEC_BATTERY_CABLE_USB_CDP, 1},
			{SEC_BATTERY_CABLE_NONE, 0},
			{SEC_BATTERY_CABLE_TA, 1},
			{SEC_BATTERY_CABLE_NONE, 0},
			{SEC_BATTERY_CABLE_USB, 0},
			{SEC_BATTERY_CABLE_NONE, 0}};
	int i;
	int cnt = sizeof(table) / sizeof(int) / 2;
	int old_cable = battery->wire_status;

	for (i = 0; i < cnt; i++) {
		battery->wire_status = table[i][0];
		sec_bat_cable_work((struct work_struct *)&battery->cable_work);
		psy_do_property(battery->pdata->wireless_charger_name, get,
				POWER_SUPPLY_EXT_PROP_WPC_EN, val);
		EXPECT_EQ(test, val.intval, table[i][1]);
	}
	battery->wire_status = old_cable;
	sec_bat_cable_work((struct work_struct *)&battery->cable_work);
}

static void kunit_sec_bat_cable_work_check_wireless(struct test *test,
		struct sec_battery_info *battery)
{
	int table[][2] = {{SEC_WIRELESS_PAD_WPC, SEC_BATTERY_CABLE_WIRELESS},
			{SEC_WIRELESS_PAD_WPC_HV, SEC_BATTERY_CABLE_HV_WIRELESS},
			{SEC_WIRELESS_PAD_WPC_PACK, SEC_BATTERY_CABLE_WIRELESS_PACK}};
	int i;
	int cnt = sizeof(table) / sizeof(int) / 2;
	int old_cable = battery->wire_status;
	int old_wc_status = battery->wc_status;

	battery->wire_status = SEC_BATTERY_CABLE_NONE;
	for (i = 0; i < cnt; i++) {
		battery->wc_status = table[i][0];
		sec_bat_cable_work((struct work_struct *)&battery->cable_work);
		EXPECT_EQ(test, battery->cable_type, table[i][1]);
	}
	battery->wire_status = old_cable;
	battery->wc_status = old_wc_status;
	sec_bat_cable_work((struct work_struct *)&battery->cable_work);
}

static void kunit_sec_bat_cable_work_check_wireless_with_cable(struct test *test,
		struct sec_battery_info *battery)
{
	int table[][2] = {{SEC_WIRELESS_PAD_WPC, SEC_BATTERY_CABLE_WIRELESS},
			{SEC_WIRELESS_PAD_WPC_HV, SEC_BATTERY_CABLE_HV_WIRELESS},
			{SEC_WIRELESS_PAD_WPC_PACK, SEC_BATTERY_CABLE_WIRELESS_PACK}};
	int i;
	int cnt = sizeof(table) / sizeof(int) / 2;
	int old_cable = battery->wire_status;
	int old_wc_status = battery->wc_status;

	battery->wire_status = SEC_BATTERY_CABLE_USB;
	for (i = 0; i < cnt; i++) {
		battery->wc_status = table[i][0];
		sec_bat_cable_work((struct work_struct *)&battery->cable_work);
		EXPECT_EQ(test, battery->cable_type, table[i][1]);
	}

	battery->wire_status = SEC_BATTERY_CABLE_TA;
	for (i = 0; i < cnt; i++) {
		battery->wc_status = table[i][0];
		sec_bat_cable_work((struct work_struct *)&battery->cable_work);
		EXPECT_EQ(test, battery->cable_type, SEC_BATTERY_CABLE_TA);
	}
	battery->wire_status = old_cable;
	battery->wc_status = old_wc_status;
	sec_bat_cable_work((struct work_struct *)&battery->cable_work);
}

static void kunit_sec_bat_cable_work(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	test_info(test, "START %s test\n", __func__);
	kunit_sec_bat_cable_work_check_mfc_off(test, battery);
	kunit_sec_bat_cable_work_check_wireless(test, battery);
	kunit_sec_bat_cable_work_check_wireless_with_cable(test, battery);

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_get_cable_type(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	test_info(test, "START %s test\n", __func__);
	sec_bat_get_cable_type(battery, battery->pdata->cable_source_type);
	SUCCEED(test);
	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_get_charging_current_by_siop(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int backup_siop_level = battery->siop_level;
	int backup_cable_type = battery->cable_type;
	int input_current = -1;
	int charging_current = -1;
	int test_siop_level[] = {100, 70, 10, 0};
	int i = 0;

	test_info(test, "START %s test\n", __func__);

	for (i = 0; i < sizeof(test_siop_level) / sizeof(int); i++) {
		battery->siop_level = test_siop_level[i];
		for(battery->cable_type = 0; battery->cable_type < SEC_BATTERY_CABLE_MAX; battery->cable_type++) {
			input_current = battery->pdata->charging_current[battery->cable_type].input_current_limit;
			charging_current = battery->pdata->charging_current[battery->cable_type].fast_charging_current;

			sec_bat_get_charging_current_by_siop(battery, &input_current, &charging_current);
			EXPECT_LE(test, input_current,
					battery->pdata->charging_current[battery->cable_type].input_current_limit);
			EXPECT_LE(test, charging_current,
					battery->pdata->charging_current[battery->cable_type].fast_charging_current);
		}
	}

	test_info(test, "%s test done\n", __func__);

	battery->siop_level = backup_siop_level;
	battery->cable_type = backup_cable_type;
}

/* sec_bat_get_input_current_in_power_list */
/* sec_bat_get_charging_current_in_power_list */
static void kunit_sec_bat_get_input_charging_current_in_power_list(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int backup_current_pdo_num = battery->pdic_info.sink_status.current_pdo_num;
	int backup_max_voltage = battery->pdic_info.sink_status.power_list[0].max_voltage;
	int backup_max_current = battery->pdic_info.sink_status.power_list[0].max_current;
	int backup_input_current_limit_pdic = battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC].input_current_limit;
	int backup_input_current_limit_apdo = battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC_APDO].input_current_limit;
	int backup_charging_current_pdic = battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC].fast_charging_current;
	int backup_charging_current_apdo = battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC_APDO].fast_charging_current;
	int backup_now_isApdo = battery->pd_list.now_isApdo;
	int backup_step_charging_status = battery->step_charging_status;
	int backup_wire_status = battery->wire_status;
	int test_voltage[] = {5000, 5000, 9000, 9000, 12000};
	int test_current[] = {2000, 3000, 2000, 3000, 2500};
	int i = 0;

	test_info(test, "START %s test\n", __func__);

	if (sizeof(test_voltage) != sizeof(test_current))
		FAIL(test, "Test cases does not match. Please check test_voltage and test_current\n");

	battery->pd_list.now_isApdo = 0;
	battery->step_charging_status = -1;
	battery->pdic_info.sink_status.current_pdo_num = 0;
	battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC].input_current_limit = 0;
	battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC_APDO].input_current_limit = 0;

	for (i = 0; i < sizeof(test_voltage) / sizeof(int); i++) {
		battery->wire_status = SEC_BATTERY_CABLE_PDIC;
		battery->pdic_info.sink_status.power_list[0].max_voltage = test_voltage[i];
		battery->pdic_info.sink_status.power_list[0].max_current = test_current[i];
		sec_bat_get_charging_current_in_power_list(battery);
		EXPECT_LE(test,
				battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC].fast_charging_current,
				battery->pdata->max_charging_current);

		battery->wire_status = SEC_BATTERY_CABLE_PDIC_APDO;
		sec_bat_get_charging_current_in_power_list(battery);
		EXPECT_LE(test,
				battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC_APDO].fast_charging_current,
				battery->pdata->max_charging_current);

		sec_bat_get_input_current_in_power_list(battery);
		EXPECT_EQ(test, battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC].input_current_limit,
				battery->pdic_info.sink_status.power_list[0].max_current);
		EXPECT_EQ(test, battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC_APDO].input_current_limit,
				battery->pdic_info.sink_status.power_list[0].max_current);
	}

	test_info(test, "%s test done\n", __func__);

	battery->pdic_info.sink_status.current_pdo_num = backup_current_pdo_num;
	battery->pdic_info.sink_status.power_list[0].max_voltage = backup_max_voltage;
	battery->pdic_info.sink_status.power_list[0].max_current = backup_max_current;
	battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC].fast_charging_current = backup_charging_current_pdic;
	battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC_APDO].fast_charging_current = backup_charging_current_apdo;
	battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC].input_current_limit = backup_input_current_limit_pdic;
	battery->pdata->charging_current[SEC_BATTERY_CABLE_PDIC_APDO].input_current_limit = backup_input_current_limit_apdo;
	battery->pd_list.now_isApdo = backup_now_isApdo;
	battery->step_charging_status = backup_step_charging_status;
	battery->wire_status = backup_wire_status;
}

static void kunit_sec_bat_get_temp_by_temp_control_source(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	test_info(test, "START %s test\n", __func__);
	EXPECT_EQ(test, sec_bat_get_temp_by_temp_control_source(battery, TEMP_CONTROL_SOURCE_CHG_THM), battery->chg_temp);
	EXPECT_EQ(test, sec_bat_get_temp_by_temp_control_source(battery, TEMP_CONTROL_SOURCE_USB_THM), battery->usb_temp);
	EXPECT_EQ(test, sec_bat_get_temp_by_temp_control_source(battery, TEMP_CONTROL_SOURCE_WPC_THM), battery->wpc_temp);
	EXPECT_EQ(test, sec_bat_get_temp_by_temp_control_source(battery, TEMP_CONTROL_SOURCE_NONE), battery->temperature);
	EXPECT_EQ(test, sec_bat_get_temp_by_temp_control_source(battery, TEMP_CONTROL_SOURCE_BAT_THM), battery->temperature);
	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_get_wireless20_power_class(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int backup_cable_type = battery->cable_type;

	test_info(test, "START %s test\n", __func__);
	battery->cable_type = SEC_BATTERY_CABLE_PREPARE_WIRELESS_20;
	EXPECT_EQ(test, sec_bat_get_wireless20_power_class(battery), battery->wc20_power_class);
	battery->cable_type = SEC_BATTERY_CABLE_WIRELESS;
	EXPECT_EQ(test, sec_bat_get_wireless20_power_class(battery), SEC_WIRELESS_RX_POWER_CLASS_1);
	test_info(test, "%s test done\n", __func__);

	battery->cable_type = backup_cable_type;
}

static void kunit_sec_bat_get_wireless_current(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);
	int incurr = 0;
	int backup_cable_type = battery->cable_type;
	int backup_sleep_mode = sleep_mode;
	int backup_chg_limit = battery->chg_limit;
	int backup_siop_level = battery->siop_level;
	int backup_lcd_status = battery->lcd_status;
	int backup_status = battery->status;
	int backup_charging_mode = battery->charging_mode;
	int backup_capacity = battery->capacity;
	int backup_wpc_input_limit_by_tx_check = battery->pdata->wpc_input_limit_by_tx_check;

	test_info(test, "START %s test\n", __func__);

	/* WPC_SLEEP_MODE */
	battery->cable_type = SEC_BATTERY_CABLE_HV_WIRELESS;
	sleep_mode = 1;
	for (incurr = 3000; incurr > 0; incurr -= 500)
		EXPECT_LE(test, sec_bat_get_wireless_current(battery, incurr), battery->pdata->sleep_mode_limit_current);

	sleep_mode = backup_sleep_mode;

	/* WPC_TEMP_MODE */
	battery->chg_limit = 1;

	battery->siop_level = 100;
	battery->lcd_status = 0;
	for (incurr = 3000; incurr > 0; incurr -= 500) {
		if (incurr > battery->pdata->wpc_input_limit_current) {
			battery->cable_type = SEC_BATTERY_CABLE_WIRELESS_TX;
			battery->pdata->wpc_input_limit_by_tx_check = 1;
			EXPECT_LE(test, sec_bat_get_wireless_current(battery, incurr),
					battery->pdata->wpc_input_limit_current_by_tx);
			battery->pdata->wpc_input_limit_by_tx_check = backup_wpc_input_limit_by_tx_check;

			battery->cable_type = SEC_BATTERY_CABLE_WIRELESS;
			EXPECT_LE(test, sec_bat_get_wireless_current(battery, incurr), battery->pdata->wpc_input_limit_current);
		}
	}

	battery->siop_level = 70;
	battery->lcd_status = 1;
	for (incurr = 3000; incurr > 0; incurr -= 500)
		EXPECT_LE(test, sec_bat_get_wireless_current(battery, incurr), battery->pdata->wpc_lcd_on_input_limit_current);

	battery->chg_limit = backup_chg_limit;
	battery->siop_level = backup_siop_level;
	battery->lcd_status = backup_lcd_status;

	/* Full-Additional state */
	battery->status = POWER_SUPPLY_STATUS_FULL;
	battery->charging_mode = SEC_BATTERY_CHARGING_2ND;
	for (incurr = 3000; incurr > 0; incurr -= 500)
		EXPECT_LE(test, sec_bat_get_wireless_current(battery, incurr),
				battery->pdata->siop_hv_wireless_input_limit_current);

	battery->status = backup_status;
	battery->charging_mode = backup_charging_mode;

	/* Hero Stand Pad CV */
	battery->capacity = 100;
	battery->cable_type = SEC_BATTERY_CABLE_WIRELESS_STAND;
	for (incurr = 3000; incurr > 0; incurr -= 500)
		EXPECT_LE(test, sec_bat_get_wireless_current(battery, incurr), battery->pdata->wc_hero_stand_cv_current);

	battery->cable_type = SEC_BATTERY_CABLE_WIRELESS_HV_STAND;
	battery->chg_limit = 1;
	for (incurr = 3000; incurr > 0; incurr -= 500)
		EXPECT_LE(test, sec_bat_get_wireless_current(battery, incurr), battery->pdata->wc_hero_stand_cv_current);

	battery->chg_limit = 0;
	for (incurr = 3000; incurr > 0; incurr -= 500)
		EXPECT_LE(test, sec_bat_get_wireless_current(battery, incurr), battery->pdata->wc_hero_stand_hv_cv_current);

	battery->chg_limit = backup_chg_limit;
	battery->cable_type = backup_cable_type;
	battery->capacity = backup_capacity;

	/* Full-None state && SIOP_LEVEL 100 */
	battery->siop_level = 100;
	battery->lcd_status = 0;
	battery->status = POWER_SUPPLY_STATUS_FULL;
	battery->charging_mode = SEC_BATTERY_CHARGING_NONE;
	for (incurr = 3000; incurr > 0; incurr -= 500)
		EXPECT_EQ(test, sec_bat_get_wireless_current(battery, incurr), battery->pdata->wc_full_input_limit_current);

	battery->siop_level = backup_siop_level;
	battery->lcd_status = backup_lcd_status;
	battery->status = backup_status;
	battery->charging_mode = backup_charging_mode;

	test_info(test, "%s test done\n", __func__);

	battery->cable_type = backup_cable_type;

}

static void kunit_sec_bat_handle_tx_misalign(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	unsigned int backup_tx_event = battery->tx_event;

	test_info(test, "START %s test\n", __func__);

	battery->tx_misalign_cnt = 1;
	sec_bat_handle_tx_misalign(battery, 1);
	EXPECT_EQ(test, (battery->tx_retry_case & SEC_BAT_TX_RETRY_MISALIGN) ? true : false, true);

	battery->tx_misalign_cnt = 3;
	sec_bat_handle_tx_misalign(battery, 1);
	EXPECT_EQ(test, (battery->tx_retry_case & SEC_BAT_TX_RETRY_MISALIGN) ? true : false, false);

	battery->tx_misalign_cnt = 0;
	battery->tx_misalign_passed_time = 0;
	battery->tx_event = backup_tx_event;

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_hv_wc_normal_mode_check(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int backup_sleep_mode = sleep_mode;

	test_info(test, "START %s test\n", __func__);

	sleep_mode = 1;
	EXPECT_EQ(test, sec_bat_hv_wc_normal_mode_check(battery), true);

	sleep_mode = backup_sleep_mode;

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_init_chg_work(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int backup_cable_type = battery->cable_type;
	unsigned int backup_misc_event = battery->misc_event;
	unsigned int backup_charger_mode = battery->charger_mode;


	test_info(test, "START %s test\n", __func__);

	battery->cable_type = SEC_BATTERY_CABLE_NONE;
	battery->misc_event = 0;
	queue_delayed_work(battery->monitor_wqueue, &battery->init_chg_work, 0);

	SUCCEED(test);
	battery->cable_type = backup_cable_type;
	battery->misc_event = backup_misc_event;
	sec_bat_set_charge(battery, backup_charger_mode);

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_set_aging_step(struct test *test)
{
        struct power_supply *psy = power_supply_get_by_name("battery");
        struct sec_battery_info *battery = power_supply_get_drvdata(psy);

        int backup_age_step = battery->pdata->age_step;
        unsigned int backup_chg_float_voltage = battery->pdata->chg_float_voltage;
        unsigned int backup_swelling_normal_float_voltage = battery->pdata->swelling_normal_float_voltage;
        unsigned int backup_recharge_condition_vcell = battery->pdata->recharge_condition_vcell;
        unsigned int backup_full_condition_soc = battery->pdata->full_condition_soc;
        unsigned int backup_full_condition_vcell = battery->pdata->full_condition_vcell;

        int i = 0;

	test_info(test, "START %s test\n", __func__);

        for(i = 0; i < battery->pdata->num_age_step; i++) {
                sec_bat_set_aging_step(battery, i);
                EXPECT_EQ(test, battery->pdata->age_step, i);
                EXPECT_EQ(test, battery->pdata->chg_float_voltage, battery->pdata->age_data[i].float_voltage);
                EXPECT_EQ(test, battery->pdata->swelling_normal_float_voltage, battery->pdata->chg_float_voltage);
                EXPECT_EQ(test, battery->pdata->recharge_condition_vcell, battery->pdata->age_data[i].recharge_condition_vcell);
                EXPECT_EQ(test, battery->pdata->full_condition_soc, battery->pdata->age_data[i].full_condition_soc);
                EXPECT_EQ(test, battery->pdata->full_condition_vcell, battery->pdata->age_data[i].full_condition_vcell);
        }

        battery->pdata->age_step = backup_age_step;
        battery->pdata->chg_float_voltage = backup_chg_float_voltage;
        battery->pdata->swelling_normal_float_voltage = backup_swelling_normal_float_voltage;
        battery->pdata->recharge_condition_vcell = backup_recharge_condition_vcell;
        battery->pdata->full_condition_soc = backup_full_condition_soc;
        battery->pdata->full_condition_vcell = backup_full_condition_vcell;

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_predict_wireless20_time_to_full_current(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

        unsigned int backup_ttf_predict_wc20_charge_current = battery->ttf_predict_wc20_charge_current;

        int i;

	test_info(test, "START %s test\n", __func__);

        for(i = 0; i < SEC_WIRELESS_RX_POWER_MAX; i++) {
                sec_bat_predict_wireless20_time_to_full_current(battery, i);
                EXPECT_EQ(test, battery->ttf_predict_wc20_charge_current, battery->pdata->wireless_power_info[i].ttf_charge_current);
        }
        
        battery->ttf_predict_wc20_charge_current = backup_ttf_predict_wc20_charge_current;

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_set_charging_status(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

        int backup_status = battery->status;
        int i = 0;

	test_info(test, "START %s test\n", __func__);

        for (i = 0; i <= POWER_SUPPLY_STATUS_FULL; i++) {
                sec_bat_set_charging_status(battery, i);
                EXPECT_EQ(test, i, battery->status);
        }

        battery->status = backup_status;
        sec_bat_set_charging_status(battery, battery->status);
        
	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_set_decrease_iout(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);
	union power_supply_propval value = {0, };
	int backup_input_current = battery->input_current;

	test_info(test, "START %s test\n", __func__);

	battery->input_current = 1000;
	sec_bat_set_decrease_iout(battery, false);

	psy_do_property(battery->pdata->charger_name, get,
						POWER_SUPPLY_PROP_CURRENT_AVG, value);

	EXPECT_LE(test, value.intval, battery->input_current);

	value.intval = battery->input_current = backup_input_current;
	psy_do_property(battery->pdata->charger_name, set,
		POWER_SUPPLY_EXT_PROP_PAD_VOLT_CTRL, value);

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_change_default_current(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int i, backup_input, backup_output;

	test_info(test, "START %s test\n", __func__);

	for (i = 0; i < SEC_BATTERY_CABLE_MAX; i++) {
		backup_input = battery->pdata->charging_current[i].input_current_limit;
		backup_output = battery->pdata->charging_current[i].fast_charging_current;
		sec_bat_change_default_current(battery, i, backup_input * 2, backup_output * 2);
		EXPECT_EQ(test, battery->pdata->charging_current[i].input_current_limit, backup_input * 2);
		EXPECT_EQ(test, battery->pdata->charging_current[i].fast_charging_current, backup_output * 2);
		battery->pdata->charging_current[i].input_current_limit = backup_input;
		battery->pdata->charging_current[i].fast_charging_current = backup_output;
	}

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_check_afc_input_current(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int backup_cable_type = battery->cable_type;
	unsigned int backup_current_event = battery->current_event & SEC_BAT_CURRENT_EVENT_AFC;
	int input_current;

	test_info(test, "START %s test\n", __func__);

	sec_bat_set_current_event(battery, SEC_BAT_CURRENT_EVENT_AFC, SEC_BAT_CURRENT_EVENT_AFC);
	battery->cable_type = SEC_BATTERY_CABLE_HV_WIRELESS;
	input_current = sec_bat_check_afc_input_current(battery, 1800);
	EXPECT_EQ(test, input_current, battery->pdata->pre_wc_afc_input_current);

	sec_bat_set_current_event(battery, SEC_BAT_CURRENT_EVENT_AFC, SEC_BAT_CURRENT_EVENT_AFC);
	battery->cable_type = SEC_BATTERY_CABLE_PREPARE_TA;
	input_current = sec_bat_check_afc_input_current(battery, 1800);
	EXPECT_EQ(test, input_current, battery->pdata->pre_afc_input_current);

	sec_bat_set_current_event(battery, 0, SEC_BAT_CURRENT_EVENT_AFC);
	input_current = sec_bat_check_afc_input_current(battery, 1234);
	EXPECT_EQ(test, input_current, 1234);

	battery->cable_type = backup_cable_type;
	sec_bat_set_current_event(battery, backup_current_event, SEC_BAT_CURRENT_EVENT_AFC);

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_set_rp_current(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int backup_rp_currentlvl = battery->pdic_info.sink_status.rp_currentlvl;
	int backup_current_event = battery->current_event;
	int backup_store_mode = battery->store_mode;

	test_info(test, "START %s test\n", __func__);

	battery->pdic_info.sink_status.rp_currentlvl = RP_CURRENT_ABNORMAL;
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_USB);
	EXPECT_EQ(test, battery->pdata->rp_current_abnormal_rp3,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].input_current_limit);
	EXPECT_EQ(test, battery->pdata->rp_current_abnormal_rp3,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].fast_charging_current);
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_TA);
	EXPECT_EQ(test, battery->pdata->rp_current_abnormal_rp3,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].input_current_limit);
	EXPECT_EQ(test, battery->pdata->rp_current_abnormal_rp3,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].fast_charging_current);

	battery->pdic_info.sink_status.rp_currentlvl = RP_CURRENT_LEVEL3;
	battery->current_event = SEC_BAT_CURRENT_EVENT_HV_DISABLE;
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_USB);
	EXPECT_EQ(test, battery->pdata->default_input_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].input_current_limit);
	EXPECT_EQ(test, battery->pdata->default_charging_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].fast_charging_current);
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_TA);
	EXPECT_EQ(test, battery->pdata->default_input_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].input_current_limit);
	EXPECT_EQ(test, battery->pdata->default_charging_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].fast_charging_current);
	battery->current_event = backup_current_event;

	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_USB);
	EXPECT_EQ(test, battery->pdata->rp_current_rdu_rp3,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].input_current_limit);
	EXPECT_EQ(test, battery->pdata->max_charging_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].fast_charging_current);
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_TA);
	EXPECT_EQ(test, battery->pdata->rp_current_rdu_rp3,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].input_current_limit);
	EXPECT_EQ(test, battery->pdata->max_charging_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].fast_charging_current);

	battery->store_mode = 0;
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_USB);
	EXPECT_EQ(test, battery->pdata->rp_current_rp3,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].input_current_limit);
	EXPECT_EQ(test, battery->pdata->max_charging_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].fast_charging_current);
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_TA);
	EXPECT_EQ(test, battery->pdata->rp_current_rp3,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].input_current_limit);
	EXPECT_EQ(test, battery->pdata->max_charging_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].fast_charging_current);
	battery->store_mode = backup_store_mode;

	battery->pdic_info.sink_status.rp_currentlvl = RP_CURRENT_LEVEL2;
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_USB);
	EXPECT_EQ(test, battery->pdata->rp_current_rp2,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].input_current_limit);
	EXPECT_EQ(test, battery->pdata->rp_current_rp2,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].fast_charging_current);
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_TA);
	EXPECT_EQ(test, battery->pdata->rp_current_rp2,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].input_current_limit);
	EXPECT_EQ(test, battery->pdata->rp_current_rp2,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].fast_charging_current);

	battery->pdic_info.sink_status.rp_currentlvl = RP_CURRENT_LEVEL_DEFAULT;
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_USB);
	EXPECT_EQ(test, battery->pdata->default_usb_input_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].input_current_limit);
	EXPECT_EQ(test, battery->pdata->default_usb_charging_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].fast_charging_current);
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_TA);
	EXPECT_EQ(test, battery->pdata->default_input_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].input_current_limit);
	EXPECT_EQ(test, battery->pdata->default_charging_current,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_TA].fast_charging_current);

	battery->current_event = SEC_BAT_CURRENT_EVENT_USB_SUPER;
	sec_bat_set_rp_current(battery, SEC_BATTERY_CABLE_USB);
	EXPECT_EQ(test, USB_CURRENT_SUPER_SPEED,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].input_current_limit);
	EXPECT_EQ(test, USB_CURRENT_SUPER_SPEED,
		battery->pdata->charging_current[SEC_BATTERY_CABLE_USB].fast_charging_current);
	battery->current_event = backup_current_event;

	battery->pdic_info.sink_status.rp_currentlvl = backup_rp_currentlvl;

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_check_pd_input_current(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	unsigned int backup_current_event = battery->current_event & SEC_BAT_CURRENT_EVENT_SELECT_PDO;
	int input_current;

	test_info(test, "START %s test\n", __func__);

	input_current = battery->current_max;
	sec_bat_set_current_event(battery, 0, SEC_BAT_CURRENT_EVENT_SELECT_PDO);
	input_current = sec_bat_check_pd_input_current(battery, input_current);
	EXPECT_EQ(test, input_current, battery->current_max);

	input_current = battery->current_max;
	sec_bat_set_current_event(battery, SEC_BAT_CURRENT_EVENT_SELECT_PDO, SEC_BAT_CURRENT_EVENT_SELECT_PDO);
	input_current = sec_bat_check_pd_input_current(battery, input_current);
	EXPECT_EQ(test, input_current, SELECT_PDO_INPUT_CURRENT);

	sec_bat_set_current_event(battery, backup_current_event, SEC_BAT_CURRENT_EVENT_SELECT_PDO);

	test_info(test, "%s test done\n", __func__);
}
#endif

static void kunit_sec_bat_wc_cv_mode_check(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	int backup_capacity = battery->capacity;
	bool backup_wc_cv_mode = battery->wc_cv_mode;

	test_info(test, "START %s test\n", __func__);

	battery->capacity = 90;
	sec_bat_wc_cv_mode_check(battery);
	EXPECT_EQ(test, battery->wc_cv_mode, true);

	battery->capacity = backup_capacity;
	battery->wc_cv_mode = backup_wc_cv_mode;

	test_info(test, "%s test done\n", __func__);
}

static void kunit_sec_bat_set_wireless20_current(struct test *test)
{
	struct power_supply *psy = power_supply_get_by_name("battery");
	struct sec_battery_info *battery = power_supply_get_drvdata(psy);

	unsigned int backup_cable_type = battery->cable_type;
	unsigned int backup_wc_status = battery->wc_status;
	unsigned int backup_wc20_power_class = battery->wc20_power_class;
	int backup_temperature = battery->temperature;
	int backup_siop_level = battery->siop_level;
	bool backup_lcd_status = battery->lcd_status;
	bool backup_sleep_mode = sleep_mode;
	bool backup_store_mode = battery->store_mode;
	int backup_chg_limit = battery->chg_limit;
	int backup_wpc_input_limit_by_tx_check = battery->pdata->wpc_input_limit_by_tx_check;
	int i = 0;
	unsigned int backup_current_event = battery->current_event;

	test_info(test, "START %s test\n", __func__);

	battery->cable_type = SEC_BATTERY_CABLE_HV_WIRELESS_20;
	battery->wc_status = SEC_WIRELESS_PAD_WPC_HV_20;
	battery->temperature = 300;
	battery->siop_level = 100;
	battery->lcd_status = 0;
	sleep_mode = false;
	battery->store_mode = false;
	battery->chg_limit = 0;
	battery->pdata->wpc_input_limit_by_tx_check = 0;
	sec_bat_set_current_event(battery, 0x0, 0xffffffff);

	for (i = 0; i < SEC_WIRELESS_RX_POWER_MAX; i++) {
		sec_bat_set_wireless20_current(battery, i);
		test_info(test, "%s WC2.0 rx power[%d] - input current check\n", __func__, i);
		EXPECT_EQ(test, battery->pdata->wireless_power_info[i].input_current_limit,
			battery->pdata->charging_current[SEC_BATTERY_CABLE_HV_WIRELESS_20].input_current_limit);
		test_info(test, "%s WC2.0 rx power[%d] - input current check\n", __func__, i);
		EXPECT_EQ(test, battery->pdata->wireless_power_info[i].input_current_limit,
			battery->input_current);
		test_info(test, "%s WC2.0 rx power[%d] - output current check\n", __func__, i);
		EXPECT_EQ(test, battery->pdata->wireless_power_info[i].fast_charging_current,
			battery->pdata->charging_current[SEC_BATTERY_CABLE_HV_WIRELESS_20].fast_charging_current);
		test_info(test, "%s WC2.0 rx power[%d] - output current check\n", __func__, i);
		EXPECT_EQ(test, battery->pdata->wireless_power_info[i].fast_charging_current,
			battery->charging_current);
	}
	battery->cable_type = backup_cable_type;
	battery->wc_status = backup_wc_status;
	battery->wc20_power_class = backup_wc20_power_class;
	battery->temperature = backup_temperature;
	battery->siop_level = backup_siop_level;
	battery->lcd_status = backup_lcd_status;
	sleep_mode = backup_sleep_mode;
	battery->store_mode = backup_store_mode;
	battery->chg_limit = backup_chg_limit;
	battery->pdata->wpc_input_limit_by_tx_check = backup_wpc_input_limit_by_tx_check;
	sec_bat_set_current_event(battery, backup_current_event, backup_current_event);

	test_info(test, "%s test done\n", __func__);
}

/* NOTE: UML TC */
static void sec_battery_test_bar(struct test *test)
{
	/* Test cases for UML */
	return;
}

/*
 * This is run once before each test case, see the comment on
 * example_test_module for more information.
 */
static int sec_battery_test_init(struct test *test)
{
	return 0;
}

/*
 * This is run once after each test case, see the comment on example_test_module
 * for more information.
 */
static void sec_battery_test_exit(struct test *test)
{
}

/*
 * Here we make a list of all the test cases we want to add to the test module
 * below.
 */
static struct test_case sec_battery_test_cases[] = {
	/*
	 * This is a helper to create a test case object from a test case
	 * function; its exact function is not important to understand how to
	 * use KUnit, just know that this is how you associate test cases with a
	 * test module.
	 */
#if !defined(CONFIG_UML)
	/* NOTE: Target running TC */

	TEST_CASE(sec_battery_test_start),
	/* test_start has cancel_delayed_work of monitor_work */

	TEST_CASE(kunit_sec_ac_get_property),
	TEST_CASE(kunit_sec_make_pd_list),
	TEST_CASE(kunit_sec_bat_afc_work),
	TEST_CASE(kunit_sec_bat_aging_check),
	TEST_CASE(kunit_sec_bat_cable_check),
	TEST_CASE(kunit_sec_bat_cable_work),
	TEST_CASE(kunit_sec_bat_get_cable_type),
	TEST_CASE(kunit_sec_bat_get_charging_current_by_siop),
	TEST_CASE(kunit_sec_bat_get_input_charging_current_in_power_list),
	TEST_CASE(kunit_sec_bat_get_temp_by_temp_control_source),
	TEST_CASE(kunit_sec_bat_get_wireless20_power_class),
	TEST_CASE(kunit_sec_bat_set_wireless20_current),
	TEST_CASE(kunit_sec_bat_get_wireless_current),
	TEST_CASE(kunit_sec_bat_handle_tx_misalign),
	TEST_CASE(kunit_sec_bat_hv_wc_normal_mode_check),
	TEST_CASE(kunit_sec_bat_init_chg_work),

	TEST_CASE(kunit_sec_bat_set_aging_step),
	TEST_CASE(kunit_sec_bat_predict_wireless20_time_to_full_current),
	TEST_CASE(kunit_sec_bat_set_charging_status),
	TEST_CASE(kunit_sec_bat_set_decrease_iout),

	TEST_CASE(kunit_sec_bat_change_default_current),
	TEST_CASE(kunit_sec_bat_check_afc_input_current),
	TEST_CASE(kunit_sec_bat_set_rp_current),
	TEST_CASE(kunit_sec_bat_wc_cv_mode_check),
	TEST_CASE(kunit_sec_bat_check_pd_input_current),

	TEST_CASE(sec_battery_test_end),
	/* test_end has queue_delayed_work of monitor_work */

#endif
	/* NOTE: UML TC */
	TEST_CASE(sec_battery_test_bar),
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
static struct test_module sec_battery_test_module = {
	.name = "sec_battery_test",
	.init = sec_battery_test_init,
	.exit = sec_battery_test_exit,
	.test_cases = sec_battery_test_cases,
};

/*
 * This registers the above test module telling KUnit that this is a suite of
 * tests that need to be run.
 */
module_test(sec_battery_test_module);
