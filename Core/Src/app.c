#include <string.h>

#include "app.h"
#include "tasks/bse_task.h"
#include "tasks/rtd_task.h"
#include "tasks/error_task.h"
#include "tasks/bse_task.h"
#include "tasks/bppc_task.h"
#include "tasks/apps_task.h"
#include "tasks/canbus_task.h"
#include "tasks/cli_task.h"
#include "tasks/dashboard_task.h"


app_data_t app = {0};

void app_create()
{
	app.throttle = 0;
	app.brake = 0;

	app.rtd_state = false;

	app.hard_fault = false;
	app.soft_fault = false;


	app.apps_fault = false;
	app.bse_fault = false;
	app.bppc_fault = false;
	app.cli_fault = false;
	app.canbus_fault = false;
	app.dashboard_fault = false;

	app.fw_state = false;
	app.tsal = false;
	app.rtd_button = false;
	app.imd_fail = false;
	app.bms_fail = false;
	app.bspd_fail = false;

	app.brakelight = false;

	app.throttle = 0;
	app.brake = 0;

	board_init(&app.board);

//	HAL_UART_Receive_IT(app.board.cli.huart, &app.board.cli.c, 1);
	HAL_CAN_ActivateNotification(app.board.canbus.hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

	assert(app.cli_task = cli_task_start(&app));
	assert(app.rtd_task = rtd_task_start(&app));
	assert(app.error_task = error_task_start(&app));
	assert(app.canbus_task = canbus_task_start(&app));
	assert(app.bse_task = bse_task_start(&app));
	assert(app.apps_task = apps_task_start(&app));
	assert(app.bppc_task = bppc_task_start(&app));
	assert(app.acc_task = acc_task_start(&app));
	assert(app.dashboard_task = dashboard_task_start(&app));
	assert(app.cool_task = cool_task_start(&app));
}

void set_fw(bool state)
{
	app.fw_state = state;
	HAL_GPIO_WritePin(Firmware_Ok_GPIO_Port, Firmware_Ok_Pin, state);
}

void set_buzzer(bool state)
{
	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, state);
}

void set_brakelight(bool state)
{
	app.brakelight = state;
	HAL_GPIO_WritePin(Brake_Light_GPIO_Port, Brake_Light_Pin, state);
}
