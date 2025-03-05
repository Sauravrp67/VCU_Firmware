#include "safety/fault.h"

void fault_init(fault_mgr_t *m)
{
	m->latched = 0u;
}

void fault_set(fault_mgr_t *m, fault_flag_t f)
{
	m->latched |= (uint32_t)f;
}

void fault_clear(fault_mgr_t *m, fault_flag_t f)
{
	m->latched &= ~(uint32_t)f;
}

bool fault_active(const fault_mgr_t *m, fault_flag_t f)
{
	return (m->latched & (uint32_t)f) != 0u;
}

bool fault_is_hard(const fault_mgr_t *m)
{
	return (m->latched & (uint32_t)FAULT_HARD_MASK) != 0u;
}

bool fault_torque_inhibited(const fault_mgr_t *m)
{
	return (m->latched & (uint32_t)FAULT_TORQUE_INHIBIT_MASK) != 0u;
}

/* ---- CAN command watchdog ------------------------------------------------ */

void can_wd_init(can_watchdog_t *w)
{
	w->since_rx_ms = 0u;
	w->timed_out = false;
}

void can_wd_on_rx(can_watchdog_t *w)
{
	w->since_rx_ms = 0u;
	w->timed_out = false;
}

bool can_wd_update(can_watchdog_t *w, uint32_t dt_ms)
{
	if (w->since_rx_ms <= (UINT32_MAX - dt_ms))
		w->since_rx_ms += dt_ms;
	else
		w->since_rx_ms = UINT32_MAX;

	if (w->since_rx_ms >= CAN_TIMEOUT_MS)
		w->timed_out = true;

	return w->timed_out;
}
