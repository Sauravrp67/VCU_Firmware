#include "control/apps.h"

#include <math.h>

void apps_state_init(apps_state_t *st)
{
	st->implausible_ms = 0u;
	st->torque_inhibited = false;
}

float apps_throttle_pct(float apps1_pct, float apps2_pct)
{
	float t = (apps1_pct + apps2_pct) * 0.5f;
	if (t < 0.0f)
		t = 0.0f;
	if (t > 100.0f)
		t = 100.0f;
	return t;
}

bool apps_signal_in_range(float pct)
{
	return (pct >= APPS_SIGNAL_MIN_PCT) && (pct <= APPS_SIGNAL_MAX_PCT);
}

bool apps_plausibility_update(apps_state_t *st, float apps1_pct, float apps2_pct, uint32_t dt_ms)
{
	bool in_range = apps_signal_in_range(apps1_pct) && apps_signal_in_range(apps2_pct);
	bool deviating = fabsf(apps1_pct - apps2_pct) > APPS_PLAUSIBILITY_THRESH_PCT;
	bool implausible_now = deviating || !in_range;

	if (implausible_now)
	{
		/* Saturating accumulate so long faults don't wrap the counter. */
		if (st->implausible_ms <= (UINT32_MAX - dt_ms))
			st->implausible_ms += dt_ms;
		else
			st->implausible_ms = UINT32_MAX;

		if (st->implausible_ms > APPS_IMPLAUSIBILITY_LIMIT_MS)
			st->torque_inhibited = true;
	}
	else
	{
		st->implausible_ms = 0u;

		/* §5.1 recovery: signals agree (in_range && !deviating, satisfied here)
		 * AND pedal released to idle. */
		if (st->torque_inhibited && apps1_pct <= APPS_IDLE_PCT && apps2_pct <= APPS_IDLE_PCT)
		{
			st->torque_inhibited = false;
		}
	}

	return !st->torque_inhibited;
}
