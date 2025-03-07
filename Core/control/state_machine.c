#include "control/state_machine.h"

bool rtd_entry_allowed(bool ts_active, bool brake_pressed, bool rtd_button)
{
	/* Require an active tractive system and a brake-inclusive driver action. */
	return ts_active && brake_pressed && rtd_button;
}

bool precharge_complete(float dc_bus_pct_of_accumulator)
{
	return dc_bus_pct_of_accumulator >= PRECHARGE_MIN_PCT;
}

bool rtd_buzzer_ms_valid(uint32_t ms)
{
	return (ms >= RTD_BUZZER_MS_MIN) && (ms <= RTD_BUZZER_MS_MAX);
}
