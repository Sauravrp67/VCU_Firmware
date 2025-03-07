/**
 * @file    fault.h
 * @brief   Single fault enum + central fault manager + CAN command watchdog
 *          (hardware-free).
 *
 * Layer: safety/ — pure, host-testable, no STM32/CMSIS/HAL includes.
 *
 * Every module reports faults as bits; only the fault manager decides the
 * system response. Faults latch; recovery is explicit and gated on safe
 * conditions. Any hard fault drives a fail-safe zero-torque state and the
 * SDC/AIR action.
 *
 * The BSPD is a standalone, non-programmable hardware circuit. The
 * VCU firmware does NOT implement it; there is deliberately no BSPD trip here.
 */
#ifndef SAFETY_FAULT_H
#define SAFETY_FAULT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	FAULT_NONE = 0,
	FAULT_APPS = 1u << 0,        /* APPS implausibility        (hard, zero-torque) */
	FAULT_BSE = 1u << 1,         /* brake sensor fault         (hard, zero-torque) */
	FAULT_CAN_TIMEOUT = 1u << 2, /* inverter/AMS comms lost    (hard, zero-torque) */
	FAULT_BPPC = 1u << 3,        /* brake-throttle latch       (cuts torque only)  */
	FAULT_CLI = 1u << 4,         /* soft (telemetry/CLI)                            */
	FAULT_CANBUS_TX = 1u << 5,   /* soft (a single TX failure)                      */
	FAULT_DASHBOARD = 1u << 6,   /* soft                                            */
} fault_flag_t;

/* Hard faults open the shutdown circuit (kill the tractive system). */
#define FAULT_HARD_MASK (FAULT_APPS | FAULT_BSE | FAULT_CAN_TIMEOUT)

/* Anything in this mask forces zero-torque (hard faults + the BPPC latch). */
#define FAULT_TORQUE_INHIBIT_MASK (FAULT_HARD_MASK | FAULT_BPPC)

typedef struct
{
	uint32_t latched; /* OR of all faults raised since the last explicit clear */
} fault_mgr_t;

void fault_init(fault_mgr_t *m);
void fault_set(fault_mgr_t *m, fault_flag_t f);
void fault_clear(fault_mgr_t *m, fault_flag_t f); /* for recoverable faults */
bool fault_active(const fault_mgr_t *m, fault_flag_t f);

/** True if any hard fault is latched -> open SDC, zero-torque. */
bool fault_is_hard(const fault_mgr_t *m);

/** True if torque must be held at zero (hard fault or BPPC latch). */
bool fault_torque_inhibited(const fault_mgr_t *m);

/* ---- CAN command watchdog ----------------------------------------------- */

/* Loss-of-communication window before torque is forced to zero. The rules fix
 * 100 ms for brake/throttle-signal loss; we reuse it for the inverter/AMS
 * command link. Confirm against the inverter heartbeat period. */
#define CAN_TIMEOUT_MS 100u

typedef struct
{
	uint32_t since_rx_ms;
	bool timed_out;
} can_watchdog_t;

void can_wd_init(can_watchdog_t *w);
void can_wd_on_rx(can_watchdog_t *w); /* feed on each valid RX */
/** Advance by dt_ms; @return true if currently timed out (force zero-torque). */
bool can_wd_update(can_watchdog_t *w, uint32_t dt_ms);

#endif /* SAFETY_FAULT_H */
