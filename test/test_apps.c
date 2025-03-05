/* §5.1 APPS plausibility: 10% deviation, 100 ms persistence, open-circuit,
 * recovery. */
#include "control/apps.h"
#include "test_framework.h"

static void test_throttle_is_mean_and_clamped(void)
{
	CHECK_NEAR(apps_throttle_pct(40.0f, 60.0f), 50.0f, 1e-4);
	CHECK_NEAR(apps_throttle_pct(0.0f, 0.0f), 0.0f, 1e-4);
	CHECK_NEAR(apps_throttle_pct(120.0f, 120.0f), 100.0f, 1e-4); /* clamp hi */
	CHECK_NEAR(apps_throttle_pct(-20.0f, -20.0f), 0.0f, 1e-4);   /* clamp lo */
}

static void test_plausible_when_in_agreement(void)
{
	apps_state_t st;
	apps_state_init(&st);
	/* 5% apart (< 10%) for a long time stays plausible. */
	for (int i = 0; i < 100; i++)
		CHECK_TRUE(apps_plausibility_update(&st, 50.0f, 55.0f, 10u));
}

static void test_deviation_under_100ms_is_tolerated(void)
{
	apps_state_t st;
	apps_state_init(&st);
	/* 20% apart but only for 100 ms total -> not yet latched (limit is >100ms). */
	CHECK_TRUE(apps_plausibility_update(&st, 30.0f, 60.0f, 50u)); /* 50ms */
	CHECK_TRUE(apps_plausibility_update(&st, 30.0f, 60.0f, 50u)); /* 100ms */
	/* One more tick crosses 100 ms -> torque cut. */
	CHECK_FALSE(apps_plausibility_update(&st, 30.0f, 60.0f, 10u)); /* 110ms */
}

static void test_persistent_deviation_cuts_torque(void)
{
	apps_state_t st;
	apps_state_init(&st);
	CHECK_TRUE(apps_plausibility_update(&st, 10.0f, 90.0f, 100u)); /* exactly 100 */
	CHECK_FALSE(apps_plausibility_update(&st, 10.0f, 90.0f, 1u));  /* > 100 -> cut */
	/* Latches: even a brief agreement at non-idle keeps it cut. */
	CHECK_FALSE(apps_plausibility_update(&st, 50.0f, 50.0f, 10u));
}

static void test_open_circuit_is_implausible(void)
{
	apps_state_t st;
	apps_state_init(&st);
	CHECK_FALSE(apps_signal_in_range(150.0f)); /* shorted high */
	CHECK_FALSE(apps_signal_in_range(-50.0f)); /* open/floating low */
	CHECK_TRUE(apps_signal_in_range(0.0f));
	CHECK_TRUE(apps_signal_in_range(100.0f));
	/* An out-of-range channel, even with matching values, trips after 100 ms. */
	CHECK_TRUE(apps_plausibility_update(&st, 150.0f, 150.0f, 100u));
	CHECK_FALSE(apps_plausibility_update(&st, 150.0f, 150.0f, 10u));
}

static void test_recovery_requires_agreement_and_idle(void)
{
	apps_state_t st;
	apps_state_init(&st);
	/* Latch a fault. */
	apps_plausibility_update(&st, 10.0f, 90.0f, 200u);
	CHECK_FALSE(apps_plausibility_update(&st, 10.0f, 90.0f, 10u));
	/* Agreement but pedal NOT at idle -> still cut. */
	CHECK_FALSE(apps_plausibility_update(&st, 40.0f, 40.0f, 10u));
	/* Agreement AND pedal released to idle -> recovers. */
	CHECK_TRUE(apps_plausibility_update(&st, 2.0f, 2.0f, 10u));
}

int main(void)
{
	RUN(test_throttle_is_mean_and_clamped);
	RUN(test_plausible_when_in_agreement);
	RUN(test_deviation_under_100ms_is_tolerated);
	RUN(test_persistent_deviation_cuts_torque);
	RUN(test_open_circuit_is_implausible);
	RUN(test_recovery_requires_agreement_and_idle);
	TEST_MAIN_END();
}
