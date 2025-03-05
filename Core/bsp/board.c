#include "bsp/board.h"
#include "main.h"

void board_init(board_t *dev)
{
	stm32f103_init(&dev->stm32f103);

	pot_init(&dev->apps1, APPS1_0, APPS1_100, &dev->stm32f103.hadc2);
	pot_init(&dev->apps2, APPS2_0, APPS2_100, &dev->stm32f103.hadc1);
	pressure_sensor_init(&dev->bse, BSE_MIN, BSE_MAX, &dev->stm32f103.hadc1, BSE_ADC_CH);
	canbus_device_init(&dev->canbus, &dev->stm32f103.hcan1, &dev->stm32f103.can1_tx_header);
	cli_device_init(&dev->cli, &dev->stm32f103.huart2);
	dashboard_init(&dev->dashboard, &dev->stm32f103.huart1);
	speed_sensor_init(&dev->speed);
}
