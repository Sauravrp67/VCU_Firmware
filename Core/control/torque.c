#include "control/torque.h"

int16_t torque_clamp(int16_t requested)
{
	if (requested < TORQUE_MIN_CMD) return TORQUE_MIN_CMD;
	if (requested > TORQUE_MAX_CMD) return TORQUE_MAX_CMD;
	return requested;
}

int16_t torque_from_throttle_pct(float throttle_pct)
{
	if (throttle_pct <= 0.0f)   return TORQUE_MIN_CMD;
	if (throttle_pct >= 100.0f) return TORQUE_MAX_CMD;
	/* +0.5f to round to nearest command unit. */
	return torque_clamp((int16_t)((throttle_pct / 100.0f) * (float)TORQUE_MAX_CMD + 0.5f));
}
