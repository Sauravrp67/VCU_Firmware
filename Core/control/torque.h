/**
 * @file    torque.h
 * @brief   Torque request mapping + limits (hardware-free).
 *
 * Layer: control/ — pure, host-testable, no STM32/CMSIS/HAL includes.
 *
 * Safety invariant §5.7:
 *  - Commanded torque is clamped to the rulebook power cap; NO reverse.
 *  - (The CAN command path applies these limits before the inverter sees them.)
 */
#ifndef CONTROL_TORQUE_H
#define CONTROL_TORQUE_H

#include <stdint.h>

/* §5.7 — maximum commanded torque (inverter command units). Proxy for the
 * rulebook power cap (e.g. 80 kW); confirm scaling against the inverter's CAN
 * torque-command resolution when the catalog is finalized. */
#define TORQUE_MAX_CMD 160

/* No reverse: the floor is always zero. */
#define TORQUE_MIN_CMD 0

/** Clamp a raw torque command to [TORQUE_MIN_CMD, TORQUE_MAX_CMD] (no reverse). */
int16_t torque_clamp(int16_t requested);

/**
 * Map throttle travel (0..100 %) to a clamped torque command.
 * Linear 0..TORQUE_MAX_CMD; inputs outside 0..100 are saturated.
 */
int16_t torque_from_throttle_pct(float throttle_pct);

#endif /* CONTROL_TORQUE_H */
