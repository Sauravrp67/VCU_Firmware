#include "safety/plausibility.h"

void bppc_state_init(bppc_state_t *st)
{
	st->latched = false;
}

bool bppc_update(bppc_state_t *st, float brake_pct, float throttle_pct)
{
	if (st->latched)
	{
		/* Clear only when APPS drops below 5%; brake release alone is insufficient. */
		if (throttle_pct < BPPC_APPS_LOW_PCT)
			st->latched = false;
	}
	else if (brake_pct > BPPC_BRAKE_THRESH_PCT && throttle_pct > BPPC_APPS_HIGH_PCT)
	{
		st->latched = true;
	}

	return !st->latched;
}
