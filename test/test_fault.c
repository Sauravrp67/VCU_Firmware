/* Fault manager + §5.7 CAN command watchdog (zero-torque on comms loss). */
#include "safety/fault.h"
#include "test_framework.h"

static void test_hard_fault_inhibits_torque_and_opens_sdc(void)
{
	fault_mgr_t m;
	fault_init(&m);
	CHECK_FALSE(fault_is_hard(&m));
	CHECK_FALSE(fault_torque_inhibited(&m));

	fault_set(&m, FAULT_APPS);
	CHECK_TRUE(fault_is_hard(&m));
	CHECK_TRUE(fault_torque_inhibited(&m));
}

static void test_bppc_inhibits_torque_but_not_sdc(void)
{
	fault_mgr_t m;
	fault_init(&m);
	fault_set(&m, FAULT_BPPC);
	CHECK_FALSE(fault_is_hard(&m));         /* BPPC does not kill tractive system */
	CHECK_TRUE(fault_torque_inhibited(&m)); /* but does cut torque */
}

static void test_soft_faults_do_not_inhibit_torque(void)
{
	fault_mgr_t m;
	fault_init(&m);
	fault_set(&m, FAULT_CANBUS_TX);
	fault_set(&m, FAULT_DASHBOARD);
	fault_set(&m, FAULT_CLI);
	CHECK_FALSE(fault_is_hard(&m));
	CHECK_FALSE(fault_torque_inhibited(&m));
}

static void test_faults_latch_until_cleared(void)
{
	fault_mgr_t m;
	fault_init(&m);
	fault_set(&m, FAULT_BPPC);
	CHECK_TRUE(fault_active(&m, FAULT_BPPC));
	fault_clear(&m, FAULT_BPPC);
	CHECK_FALSE(fault_active(&m, FAULT_BPPC));
	CHECK_FALSE(fault_torque_inhibited(&m));
}

static void test_can_watchdog_times_out_and_forces_zero_torque(void)
{
	can_watchdog_t w;
	can_wd_init(&w);
	fault_mgr_t m;
	fault_init(&m);

	/* Fed regularly -> never times out. */
	for (int i = 0; i < 10; i++)
	{
		can_wd_on_rx(&w);
		CHECK_FALSE(can_wd_update(&w, 50u)); /* 50ms < 100ms each interval */
	}

	/* Silence beyond 100 ms -> timeout -> raise hard fault -> zero torque. */
	can_wd_on_rx(&w);
	CHECK_FALSE(can_wd_update(&w, 60u));
	CHECK_TRUE(can_wd_update(&w, 60u)); /* 120ms since last rx -> timed out */
	fault_set(&m, FAULT_CAN_TIMEOUT);
	CHECK_TRUE(fault_is_hard(&m));
	CHECK_TRUE(fault_torque_inhibited(&m));

	/* Recovery on next valid RX. */
	can_wd_on_rx(&w);
	CHECK_FALSE(can_wd_update(&w, 10u));
}

int main(void)
{
	RUN(test_hard_fault_inhibits_torque_and_opens_sdc);
	RUN(test_bppc_inhibits_torque_but_not_sdc);
	RUN(test_soft_faults_do_not_inhibit_torque);
	RUN(test_faults_latch_until_cleared);
	RUN(test_can_watchdog_times_out_and_forces_zero_torque);
	TEST_MAIN_END();
}
