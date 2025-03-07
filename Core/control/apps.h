/**
 * @file    apps.h
 * @brief   APPS (accelerator) plausibility + throttle derivation (hardware-free).
 *
 * Layer: control/ — pure, host-testable, no STM32/CMSIS/HAL includes. Operates
 * on plain percentages (0..100) handed in by the acquisition task.
 *
 * APPS plausibility requirements:
 *  - Two independent APPS; implausibility = deviation > 10% pedal travel.
 *  - If implausibility persists > 100 ms, torque must be cut (zero-torque).
 *  - Out-of-range / floating / open-circuit on either channel is implausible.
 *  - Recovery only when the signals agree again AND the pedal is released to idle.
 */
#ifndef CONTROL_APPS_H
#define CONTROL_APPS_H

#include <stdbool.h>
#include <stdint.h>

/* Deviation over this much pedal travel (percentage points) is implausible.
 * (FSAE-EV APPS plausibility; older ref T.4.2.5.) */
#define APPS_PLAUSIBILITY_THRESH_PCT 10.0f

/* Implausibility must persist beyond this duration before torque is cut. */
#define APPS_IMPLAUSIBILITY_LIMIT_MS 100u

/* A channel reading outside this band is treated as open/short/out-of-range.
 * Percentages are post-scaling; valid pedal travel lives within [lo, hi]. */
#define APPS_SIGNAL_MIN_PCT (-5.0f)
#define APPS_SIGNAL_MAX_PCT 105.0f

/* Pedal is "released to idle" below this travel (gates recovery from a latch). */
#define APPS_IDLE_PCT 5.0f

typedef struct
{
	uint32_t implausible_ms; /* how long the deviation has currently persisted */
	bool torque_inhibited;   /* latched: torque stays cut until recovery gate met */
} apps_state_t;

/** Reset plausibility state to plausible / not-inhibited. */
void apps_state_init(apps_state_t *st);

/** Derived throttle = mean of the two channels (clamped 0..100). */
float apps_throttle_pct(float apps1_pct, float apps2_pct);

/** True if a single channel reading is within the valid band (not open/short). */
bool apps_signal_in_range(float pct);

/**
 * Advance the plausibility check by dt_ms.
 * Implausible when the two channels deviate > 10% OR either is out-of-range.
 * Latches torque_inhibited once implausibility persists > 100 ms.
 * Recovery (clears the latch) requires the channels to agree AND both readings
 * to be at/below idle.
 * @return true if torque is permitted, false if torque must be zero.
 */
bool apps_plausibility_update(apps_state_t *st, float apps1_pct, float apps2_pct, uint32_t dt_ms);

#endif /* CONTROL_APPS_H */
