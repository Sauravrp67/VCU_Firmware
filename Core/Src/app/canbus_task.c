#include "app/canbus_task.h"
#include "drivers/can_bus.h"
#include "proto/can_catalog.h"

/**
 * @brief CANBus task function
 *
 * @param arg App_data struct pointer converted to void pointer
 */
void canbus_task_fn(void *arg);

#define CANBUS_STACK_WORDS 256
TaskHandle_t canbus_task_start(app_data_t *data)
{
	static StackType_t stack[CANBUS_STACK_WORDS];
	static StaticTask_t tcb;
	return xTaskCreateStatic(canbus_task_fn, "CANBus Task", CANBUS_STACK_WORDS, (void *)data,
	                         CAN_PRIO, stack, &tcb);
}

void canbus_task_fn(void *arg)
{
	app_data_t *data = (app_data_t *)arg;

	canbus_t *canbus = &data->board.canbus;
	CAN_HandleTypeDef *hcan = canbus->hcan;
	CAN_TxHeaderTypeDef *tx_header = data->board.canbus.tx_header;
	canbus_packet_t can_packet;
	HAL_StatusTypeDef can_status;
	uint32_t task_notification;

	for (;;)
	{
		xTaskNotifyWait(0, UINT32_MAX, &task_notification, HAL_MAX_DELAY);
		if (task_notification & CANBUS_APPS)
		{
			// §5.7 authoritative zero-torque gate: any torque-inhibiting fault
			// (hard fault or BPPC latch) forces the transmitted command to zero.
			int16_t torque = fault_torque_inhibited(&data->faults) ? 0 : data->torque_cmd;
			can_encode_torque_cmd(can_packet.data, torque);
			tx_header->StdId = CAN_ID_VCU_TORQUE_CMD; /* PLACEHOLDER id */

			can_status =
			    HAL_CAN_AddTxMessage(hcan, tx_header, can_packet.data, &canbus->tx_mailbox);
			if (can_status != HAL_OK)
				fault_set(&data->faults, FAULT_CANBUS_TX);
			else
				fault_clear(&data->faults, FAULT_CANBUS_TX);
		}
	}
}
