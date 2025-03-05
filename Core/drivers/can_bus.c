#include "drivers/can_bus.h"

void canbus_device_init(canbus_t *dev, CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *tx_header)
{
	dev->hcan = hcan;
	dev->tx_header = tx_header;

	dev->tx_header->IDE = CAN_ID_STD;
	dev->tx_header->StdId = 0x00;
	dev->tx_header->ExtId = 0x00;
	dev->tx_header->RTR = CAN_RTR_DATA;
	dev->tx_header->DLC = 8;
	dev->tx_header->TransmitGlobalTime = DISABLE;

	HAL_CAN_Start(hcan);
}
