/**
 * @file board.h
 * @author Saurav Raj Paudel
 * @brief
 * @version 0.1
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __BOARD_H_
#define __BOARD_H_

#include <stdbool.h>

#include "drivers/stm32f103.h"
#include "drivers/can_bus.h"
#include "drivers/current_sensor.h"
#include "drivers/dashboard.h"
#include "drivers/map.h"
#include "drivers/pot.h"
#include "drivers/pressure_sense.h"
#include "drivers/speed_sensor.h"
#include "drivers/cli.h"

#define BSE_MIN 200
#define BSE_MAX 600

#define BSE_ADC_CH 13

#define APPS1_0 200
#define APPS1_100 300
#define APPS2_0 200
#define APPS2_100 300

#define CANBUS_ISR 0x2

typedef struct {
	stm32f103_t stm32f103;
	pot_t apps1;
	pot_t apps2;
	pressure_sensor_t bse;
	canbus_t canbus;
	dashboard_t dashboard;
	current_sensor_t currentSensor;
	speed_sensor_t speed;
	cli_t cli;
} board_t;

void board_init(board_t *dev);
#endif
