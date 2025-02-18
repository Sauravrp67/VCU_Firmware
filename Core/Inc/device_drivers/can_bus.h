/**
* @file can_bus.h
* @author Saurav Raj Paudel
* @brief
* @version 0.1
* @date 2025-01-10
*
* @copyright Copyright (c) 2025
*
*/

#ifndef __CAN_BUS_H_
#define __CAN_BUS_H_

#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

#define DATALEN 8

typedef struct {
	uint32_t id;
	uint8_t data[DATALEN];
} canbus_packet_t;

typedef struct {
	CAN_HandleTypeDef *hcan;
	CAN_TxHeaderTypeDef *tx_header;
	uint32_t tx_mailbox;
	canbus_packet_t rx_packet;
	canbus_packet_t tx_packet;
} canbus_t;

void canbus_device_init(canbus_t *dev,CAN_HandleTypeDef *hcan,CAN_TxHeaderTypeDef *tx_header);

#endif
