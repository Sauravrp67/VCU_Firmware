#include "tasks/canbus_task.h"
#include "device_drivers/can_bus.h"

/**
 * @brief CANBus task function
 *
 * @param arg App_data struct pointer converted to void pointer
 */
void canbus_task_fn(void *arg);

TaskHandle_t canbus_task_start(app_data_t *data) {
    TaskHandle_t handle;
    xTaskCreate(canbus_task_fn, "CANBus Task", 512, (void *)data, CAN_PRIO, &handle);
    return handle;
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

    for(;;)
    {
		xTaskNotifyWait(0, UINT32_MAX, &task_notification, HAL_MAX_DELAY);
		if(task_notification & CANBUS_APPS)
		{
			taskENTER_CRITICAL();
			for(int i = 0; i < DATALEN; i++) can_packet.data[i] = canbus->tx_packet.data[i];
			tx_header->StdId = canbus->tx_packet.id;
			for(int i = 0; i < DATALEN; i++) canbus->tx_packet.data[i] = 0;
			taskEXIT_CRITICAL();
			can_status = HAL_CAN_AddTxMessage(hcan, tx_header, can_packet.data, &canbus->tx_mailbox);
			if(can_status != HAL_OK)
				fault_set(&data->faults, FAULT_CANBUS_TX);
			else
				fault_clear(&data->faults, FAULT_CANBUS_TX);
		}
	}
}
