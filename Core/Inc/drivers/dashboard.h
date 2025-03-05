/**
 * @file dashboard.h
 * @author Saurav Raj Paudel
 * @brief
 * @version 0.1
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __DASHBOARD_H_
#define __DASHBOARD_H_

#include "stm32f1xx_hal.h"

#define DASH_LINESZ 64

typedef struct
{
	UART_HandleTypeDef *huart;
	HAL_StatusTypeDef ret;
	char line[DASH_LINESZ];
} dashboard_t;

int dashboard_init(dashboard_t *dev, UART_HandleTypeDef *huart);
HAL_StatusTypeDef dashboard_write(dashboard_t *dev, char *str);

#endif /* INC_EXT_DRIVERS_DASHBOARD_H_ */
