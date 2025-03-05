#include <assert.h>

#include "main.h"
#include "drivers/stm32f103.h"
#include "drivers/can_bus.h"

const osMutexAttr_t can1_mutex_attr = {
	.name = "CAN Bus Mutex",
	.attr_bits = osMutexPrioInherit | osMutexRecursive,
	.cb_mem = NULL,
	.cb_size = 0UL,
};

const osMutexAttr_t i2c1_mutex_attr = {
	.name = "MPU6050 Mutex",
	.attr_bits = osMutexPrioInherit | osMutexRecursive,
	.cb_mem = NULL,
	.cb_size = 0UL,
};

const osMutexAttr_t spi2_mutex_attr = {
	.name = "SD Card Mutex",
	.attr_bits = osMutexPrioInherit | osMutexRecursive,
	.cb_mem = NULL,
	.cb_size = 0UL,
};

const osMutexAttr_t uart1_mutex_attr = {
	.name = "CLI Mutex",
	.attr_bits = osMutexPrioInherit | osMutexRecursive,
	.cb_mem = NULL,
	.cb_size = 0UL,
};

const osMutexAttr_t uart3_mutex_attr = {
	.name = "Dashboard Mutex",
	.attr_bits = osMutexPrioInherit | osMutexRecursive,
	.cb_mem = NULL,
	.cb_size = 0UL,
};

void stm32f103_init(stm32f103_t *dev)
{
	extern ADC_HandleTypeDef hadc1;
	extern ADC_HandleTypeDef hadc2;


	extern CAN_HandleTypeDef hcan;

	extern I2C_HandleTypeDef hi2c1;

	extern SPI_HandleTypeDef hspi2;

//	extern TIM_HandleTypeDef htim3;
//	extern TIM_HandleTypeDef htim4;
//	extern TIM_HandleTypeDef htim5;

	extern UART_HandleTypeDef huart1;
	extern UART_HandleTypeDef huart2;

	dev->hadc1 = hadc1;
	dev->hadc2 = hadc2;
	dev->hcan1 = hcan;
	dev->hi2c1 = hi2c1;
	dev->hspi2 = hspi2;
//	dev->htim3 = htim3;
//	dev->htim4 = htim4;
//	dev->htim5 = htim5;
	dev->huart1 = huart1;
	dev->huart2 = huart2;

	dev->can1_mutex = osMutexNew(&can1_mutex_attr);
	assert(dev->can1_mutex);

	dev->i2c1_mutex = osMutexNew(&i2c1_mutex_attr);
	assert(dev->i2c1_mutex);

	dev->spi2_mutex = osMutexNew(&spi2_mutex_attr);
	assert(dev->spi2_mutex);

	dev->uart1_mutex = osMutexNew(&uart1_mutex_attr);
	assert(dev->uart1_mutex);

	dev->uart2_mutex = osMutexNew(&uart3_mutex_attr);
	assert(dev->uart2_mutex);
}

uint16_t stm32f103_adc_read(ADC_HandleTypeDef *hadc)
{
	uint16_t count;

	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
	count = HAL_ADC_GetValue(hadc);
	HAL_ADC_Stop(hadc);
	return count;
}

HAL_StatusTypeDef stm32f103_adc_switch_channel(ADC_HandleTypeDef *hadc, uint32_t channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	return HAL_ADC_ConfigChannel(hadc, &sConfig);
}

/* IWDG key-register sequences. */
#define IWDG_KEY_ACCESS  0x5555u  /* unlock PR/RLR writes */
#define IWDG_KEY_START   0xCCCCu  /* start the watchdog   */
#define IWDG_KEY_RELOAD  0xAAAAu  /* refresh (feed)       */
#define IWDG_PR_DIV256   6u       /* prescaler divider /256 */

void stm32f103_iwdg_init(void)
{
	IWDG->KR = IWDG_KEY_ACCESS;     /* enable write access to PR/RLR */
	IWDG->PR = IWDG_PR_DIV256;      /* LSI/256 ~= 156 Hz @ 40 kHz    */
	IWDG->RLR = 78u;                /* ~0.5 s nominal timeout        */
	while (IWDG->SR != 0u) { }      /* wait for PR/RLR update to apply */
	IWDG->KR = IWDG_KEY_START;      /* start (cannot be stopped)     */
	IWDG->KR = IWDG_KEY_RELOAD;     /* initial reload                */
}

void stm32f103_iwdg_refresh(void)
{
	IWDG->KR = IWDG_KEY_RELOAD;
}
