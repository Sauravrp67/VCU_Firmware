/* Brake-throttle latch: cut at brake>10% and APPS>25%, clear only at APPS<5%. */
#include "safety/plausibility.h"
#include "test_framework.h"

static void test_no_latch_when_only_braking(void)
{
	bppc_state_t st;
	bppc_state_init(&st);
	CHECK_TRUE(bppc_update(&st, 80.0f, 0.0f));  /* hard brake, no throttle */
	CHECK_TRUE(bppc_update(&st, 80.0f, 20.0f)); /* throttle under 25% */
}

static void test_latches_on_brake_and_high_throttle(void)
{
	bppc_state_t st;
	bppc_state_init(&st);
	CHECK_FALSE(bppc_update(&st, 20.0f, 30.0f)); /* brake>10 && throttle>25 -> cut */
}

static void test_brake_release_alone_does_not_clear(void)
{
	bppc_state_t st;
	bppc_state_init(&st);
	CHECK_FALSE(bppc_update(&st, 20.0f, 30.0f)); /* latched */
	/* Release brake but keep throttle high -> still cut. */
	CHECK_FALSE(bppc_update(&st, 0.0f, 30.0f));
	CHECK_FALSE(bppc_update(&st, 0.0f, 10.0f)); /* throttle 10% (>=5) still cut */
}

static void test_clears_only_below_5pct(void)
{
	bppc_state_t st;
	bppc_state_init(&st);
	CHECK_FALSE(bppc_update(&st, 20.0f, 30.0f)); /* latched */
	CHECK_TRUE(bppc_update(&st, 0.0f, 4.0f));    /* throttle <5% -> clears */
	CHECK_TRUE(bppc_update(&st, 20.0f, 20.0f));  /* below 25% so no re-latch */
}

int main(void)
{
	RUN(test_no_latch_when_only_braking);
	RUN(test_latches_on_brake_and_high_throttle);
	RUN(test_brake_release_alone_does_not_clear);
	RUN(test_clears_only_below_5pct);
	TEST_MAIN_END();
}
