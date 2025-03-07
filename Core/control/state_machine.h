/**
 * @file    state_machine.h
 * @brief   VCU tractive-system state machine + RTD/pre-charge gating (hardware-free).
 *
 * Layer: control/ — pure, host-testable, no STM32/CMSIS/HAL includes.
 *
 * Safety requirements:
 *  - Pre-charge the DC bus to >= 90% of accumulator voltage before closing the
 *    second AIR.
 *  - RTD entry requires the tractive system to be active and a dedicated driver
 *    action that includes the brake pedal (brake + start button).
 *  - On RTD entry, sound the buzzer continuously for 1-3 s.
 */
#ifndef CONTROL_STATE_MACHINE_H
#define CONTROL_STATE_MACHINE_H

#include <stdbool.h>
#include <stdint.h>

/* DC bus must reach this fraction of accumulator voltage before AIR #2. */
#define PRECHARGE_MIN_PCT 90.0f

/* RTD buzzer duration must be within [1000, 3000] ms (1-3 s). */
#define RTD_BUZZER_MS_MIN 1000u
#define RTD_BUZZER_MS_MAX 3000u
#define RTD_BUZZER_MS     3000u /* preserves the as-built 3 s (upper bound) */

typedef enum
{
	VCU_STATE_TS_OFF = 0, /* tractive system off */
	VCU_STATE_TS_ACTIVE,  /* AIRs/precharge engaged, not yet driving */
	VCU_STATE_PRECHARGE,  /* pre-charging DC bus toward >= 90% */
	VCU_STATE_RTD,        /* ready-to-drive: buzzer done, torque enabled */
	VCU_STATE_DRIVE,      /* actively responding to APPS */
	VCU_STATE_FAULT,      /* latched fault: fail-safe zero-torque */
} vcu_state_t;

/** Return true when the brake-inclusive driver action permits RTD entry. */
bool rtd_entry_allowed(bool ts_active, bool brake_pressed, bool rtd_button);

/** Return true once the DC bus reaches 90% of accumulator voltage. */
bool precharge_complete(float dc_bus_pct_of_accumulator);

/** Compile-time-checked at runtime: buzzer duration sits within the 1-3 s window. */
bool rtd_buzzer_ms_valid(uint32_t ms);

#endif /* CONTROL_STATE_MACHINE_H */
