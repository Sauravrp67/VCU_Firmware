/* §5.4 pre-charge >=90% gate; §5.5 RTD entry gating + 1-3 s buzzer window. */
#include "control/state_machine.h"
#include "test_framework.h"

static void test_rtd_entry_requires_ts_brake_and_button(void)
{
	CHECK_FALSE(rtd_entry_allowed(false, true,  true));  /* TS not active */
	CHECK_FALSE(rtd_entry_allowed(true,  false, true));  /* no brake */
	CHECK_FALSE(rtd_entry_allowed(true,  true,  false)); /* no button */
	CHECK_TRUE (rtd_entry_allowed(true,  true,  true));  /* all present */
}

static void test_buzzer_duration_within_1_to_3s(void)
{
	CHECK_TRUE(rtd_buzzer_ms_valid(RTD_BUZZER_MS));
	CHECK_TRUE(rtd_buzzer_ms_valid(1000u));
	CHECK_TRUE(rtd_buzzer_ms_valid(3000u));
	CHECK_FALSE(rtd_buzzer_ms_valid(500u));
	CHECK_FALSE(rtd_buzzer_ms_valid(3500u));
}

static void test_precharge_gate_at_90pct(void)
{
	CHECK_FALSE(precharge_complete(0.0f));
	CHECK_FALSE(precharge_complete(89.9f));
	CHECK_TRUE (precharge_complete(90.0f));
	CHECK_TRUE (precharge_complete(99.0f));
}

int main(void)
{
	RUN(test_rtd_entry_requires_ts_brake_and_button);
	RUN(test_buzzer_duration_within_1_to_3s);
	RUN(test_precharge_gate_at_90pct);
	TEST_MAIN_END();
}
