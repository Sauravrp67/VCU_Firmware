/**
 * @file    plausibility.h
 * @brief   APPS/Brake plausibility — the "brake-throttle latch" (hardware-free).
 *
 * Layer: safety/ — pure, host-testable, no STM32/CMSIS/HAL includes.
 *
 * Safety invariant §5.2 (FSAE-EV EV.4.7):
 *  - When the mechanical brake is actuated AND APPS indicates > 25% pedal travel
 *    at the same time, motor power must be cut immediately.
 *  - The cut LATCHES until APPS drops below 5% travel — releasing the brake
 *    alone does not re-enable torque.
 */
#ifndef SAFETY_PLAUSIBILITY_H
#define SAFETY_PLAUSIBILITY_H

#include <stdbool.h>

/* §5.2 thresholds (pedal travel percentages). */
#define BPPC_BRAKE_THRESH_PCT   10.0f  /* brake considered actuated above this */
#define BPPC_APPS_HIGH_PCT      25.0f  /* throttle that triggers the latch       */
#define BPPC_APPS_LOW_PCT        5.0f  /* throttle below this clears the latch    */

typedef struct {
	bool latched;  /* true => torque cut, awaiting APPS < 5% to clear */
} bppc_state_t;

void bppc_state_init(bppc_state_t *st);

/**
 * Advance the brake-throttle plausibility latch.
 * Latches when brake > 10% AND throttle > 25%; clears only when throttle < 5%.
 * @return true if torque is permitted, false if torque must be zero.
 */
bool bppc_update(bppc_state_t *st, float brake_pct, float throttle_pct);

#endif /* SAFETY_PLAUSIBILITY_H */
