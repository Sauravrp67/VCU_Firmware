#include "device_drivers/speed_sensor.h"
#include "main.h"
#include "bsp/board_config.h"

void speed_sensor_init(speed_sensor_t *s)
{
	s->pulses = 0u;
	s->hz = 0.0f;

	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef gpio = {0};
	gpio.Pin = BSP_SPEED_SENSOR_PIN;          /* PC2 */
	gpio.Mode = GPIO_MODE_IT_RISING;
	gpio.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(BSP_SPEED_SENSOR_PORT, &gpio);

	/* Priority 5 keeps the ISR at/above configLIBRARY_MAX_SYSCALL priority so it
	 * may use FromISR APIs if needed later (matches the EXTI15_10 setup). */
	HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

void speed_sensor_on_edge(speed_sensor_t *s)
{
	s->pulses++;
}

void speed_sensor_update(speed_sensor_t *s, uint32_t window_ms)
{
	if (window_ms == 0u) return;
	uint32_t n = s->pulses;
	s->pulses = 0u;
	s->hz = (float)n * 1000.0f / (float)window_ms;
}
