/**
* @file app.h
* @author Saurav Raj Paudel
* @brief
* @version 0.1
* @date 2025-01-10
*
* @copyright Copyright (c) 2025
*
*/

#ifndef __APP_H_
#define __APP_H_

#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"

/* Hardware-free decision logic (host-tested) drives the safety behavior. */
#include "control/apps.h"
#include "control/torque.h"
#include "control/state_machine.h"
#include "safety/plausibility.h"
#include "safety/fault.h"

/* Brake-light illuminates above this brake travel (T.4.3.3). The APPS/BPPC/
 * torque thresholds now live with their logic in control/ and safety/. */
#define BRAKE_LIGHT_THRESH 5

/* A BSE raw ADC reading outside this band is treated as open/short-circuit. */
#define BSE_RAW_OPEN_LO 50u
#define BSE_RAW_OPEN_HI 4000u

/* The safety-critical loop (APPS/BSE acquisition + plausibility + BPPC + fault
 * manager + SDC + IWDG) runs in one highest-priority task at SAFETY_FREQ so the
 * 100 ms plausibility window is detected with wide margin (§8). */
#define SAFETY_FREQ 200   /* Hz -> 5 ms safety loop */
#define DASH_FREQ 5
#define CLI_FREQ 10

/* Priorities (higher number = more urgent). */
#define SAFETY_PRIO 24    /* highest user task */
#define RTD_PRIO 16
#define CLI_PRIO 15
#define CAN_PRIO 14
#define DASH_PRIO 4

/* CAN command watchdog (§5.7): leave DISARMED until the real inverter heartbeat
 * ID replaces the placeholder in proto/can_catalog.h. Arming it with a
 * placeholder ID would never be fed and would force permanent zero-torque. */
#define CAN_WATCHDOG_ARMED 0

typedef struct {
	int throttle;            /* derived APPS travel, 0..100 */
	int brake;               /* derived BSE travel, 0..100 */
	int16_t torque_cmd;      /* clamped torque command (sent over CAN, Step 6) */

	/* Hardware-free module state (the single owners of safety decisions). */
	apps_state_t   apps_state;   /* §5.1 APPS plausibility + recovery */
	bppc_state_t   bppc_state;   /* §5.2 brake-throttle latch */
	fault_mgr_t    faults;       /* single fault registry; only error_task acts on it */
	can_watchdog_t can_wd;       /* §5.7 CAN command watchdog */
	vcu_state_t    vcu_state;    /* TS-Off -> ... -> Drive state machine */

	/* Inputs / actuator status. */
	bool fw_state;           /* commanded SDC output state (1 = closed) */
	bool tsal;               /* tractive-system-active (AIR status) input */
	bool rtd_button;         /* RTD driver-action button */
	bool brakelight;         /* brake-light output state */

	board_t board;

	TaskHandle_t safety_task;
	TaskHandle_t cli_task;
	TaskHandle_t rtd_task;
	TaskHandle_t canbus_task;
	TaskHandle_t dashboard_task;

} app_data_t;

void app_create();
void cli_putline(char *line);
void set_fw(bool state);
void set_buzzer(bool state);
void set_brakelight(bool state);

#endif
