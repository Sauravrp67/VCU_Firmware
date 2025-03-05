/* §5.7 torque clamp + no reverse. */
#include "control/torque.h"
#include "test_framework.h"

static void test_clamp_no_reverse(void)
{
	CHECK_EQ_INT(torque_clamp(-1), 0);
	CHECK_EQ_INT(torque_clamp(-1000), 0);
	CHECK_EQ_INT(torque_clamp(0), 0);
	CHECK_EQ_INT(torque_clamp(50), 50);
}

static void test_clamp_to_power_cap(void)
{
	CHECK_EQ_INT(torque_clamp(TORQUE_MAX_CMD), TORQUE_MAX_CMD);
	CHECK_EQ_INT(torque_clamp(TORQUE_MAX_CMD + 1), TORQUE_MAX_CMD);
	CHECK_EQ_INT(torque_clamp(30000), TORQUE_MAX_CMD);
}

static void test_throttle_to_torque_mapping(void)
{
	CHECK_EQ_INT(torque_from_throttle_pct(0.0f), 0);
	CHECK_EQ_INT(torque_from_throttle_pct(100.0f), TORQUE_MAX_CMD);
	CHECK_EQ_INT(torque_from_throttle_pct(50.0f), TORQUE_MAX_CMD / 2);
	CHECK_EQ_INT(torque_from_throttle_pct(-10.0f), 0);              /* no reverse */
	CHECK_EQ_INT(torque_from_throttle_pct(150.0f), TORQUE_MAX_CMD); /* saturated */
}

int main(void)
{
	RUN(test_clamp_no_reverse);
	RUN(test_clamp_to_power_cap);
	RUN(test_throttle_to_torque_mapping);
	TEST_MAIN_END();
}
