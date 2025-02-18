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
#include "board.h"

#define PLAUSIBILITY_THRESH 10
#define BRAKE_LIGHT_THRESH 5
#define BPPC_BSE_THRESH 10
#define BPPC_APPS_H_THRESH 25
#define BPPC_APPS_L_THRESH 5

#define ERR_FREQ 20
#define APPS_FREQ 20
#define BSE_FREQ 20
#define BPPC_FREQ 20
#define DASH_FREQ 5
#define CLI_FREQ 10

#define ERR_PRIO 17
#define RTD_PRIO 16
#define CLI_PRIO 15
#define APPS_PRIO 14
#define BPPC_PRIO 8
#define BSE_PRIO 7
#define CAN_PRIO 14
#define DASH_PRIO 4

#define MAXTRQ 160

typedef struct {
	int throttle;
	int brake;

	bool rtd_state;

	bool hard_fault;
	bool soft_fault;

	bool apps_fault;
	bool bse_fault;
	bool bppc_fault;
	bool canbus_fault;
	bool dashboard_fault;
	bool cli_fault;

	bool fw_state;
	bool tsal;
	bool rtd_button;
	bool imd_fail;
	bool bms_fail;
	bool bspd_fail;

	bool brakelight;

	board_t board;

	TaskHandle_t dev_task;
	TaskHandle_t cli_task;
	TaskHandle_t rtd_task;
	TaskHandle_t error_task;
	TaskHandle_t apps_task;
	TaskHandle_t bse_task;
	TaskHandle_t bppc_task;
	TaskHandle_t canbus_task;
	TaskHandle_t acc_task;
	TaskHandle_t dashboard_task;
	TaskHandle_t cool_task;

} app_data_t;

void app_create();
void cli_putline(char *line);
void set_fw(bool state);
void set_buzzer(bool state);
void set_brakelight(bool state);

#endif

