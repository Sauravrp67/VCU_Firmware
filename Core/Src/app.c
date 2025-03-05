#include <assert.h>
#include <string.h>

#include "app.h"
#include "tasks/safety_task.h"
#include "tasks/rtd_task.h"
#include "tasks/canbus_task.h"
#include "tasks/cli_task.h"
#include "tasks/dashboard_task.h"


app_data_t app = {0};

void app_create()
{
	app.throttle = 0;
	app.brake = 0;
	app.torque_cmd = 0;

	apps_state_init(&app.apps_state);
	bppc_state_init(&app.bppc_state);
	fault_init(&app.faults);
	can_wd_init(&app.can_wd);
	app.vcu_state = VCU_STATE_TS_OFF;

	app.fw_state = false;
	app.tsal = false;
	app.rtd_button = false;
	app.brakelight = false;

	board_init(&app.board);

//	HAL_UART_Receive_IT(app.board.cli.huart, &app.board.cli.c, 1);
	HAL_CAN_ActivateNotification(app.board.canbus.hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

	app.cli_task = cli_task_start(&app);
	assert(app.cli_task != NULL);
	app.rtd_task = rtd_task_start(&app);
	assert(app.rtd_task != NULL);
	app.canbus_task = canbus_task_start(&app);
	assert(app.canbus_task != NULL);
	app.dashboard_task = dashboard_task_start(&app);
	assert(app.dashboard_task != NULL);
	/* The safety monitor is started last so canbus_task exists for its notify. */
	app.safety_task = safety_task_start(&app);
	assert(app.safety_task != NULL);
}

void cli_putline(char *line)
{
	cli_printline(&app.board.cli, line);
}

void set_fw(bool state)
{
	app.fw_state = state;
	HAL_GPIO_WritePin(SDC_OP_GPIO_Port, SDC_OP_Pin, state);
}

void set_buzzer(bool state)
{
	HAL_GPIO_WritePin(RTD_OUTPUT_GPIO_Port, RTD_OUTPUT_Pin, state);
}

void set_brakelight(bool state)
{
	app.brakelight = state;
	HAL_GPIO_WritePin(Brake_Light_GPIO_Port, Brake_Light_Pin, state);
}
