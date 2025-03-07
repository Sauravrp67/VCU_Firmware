#include "app/safety_task.h"
#include "main.h"
#include "bsp/board_config.h"
#include "drivers/can_bus.h"

/**
 * @brief The safety_monitor function.
 *
 * Runs at SAFETY_FREQ (5 ms) as the highest-priority task. Doing acquisition
 * and the plausibility/latch/fault evaluation together in one fast task is what
 * lets the 100 ms APPS window and the brake-throttle latch be detected
 * with wide margin, and makes this task the single writer of the shutdown
 * circuit (no inter-task SDC race).
 */
void safety_task_fn(void *arg);

#define SAFETY_STACK_WORDS 256
TaskHandle_t safety_task_start(app_data_t *data)
{
	static StackType_t stack[SAFETY_STACK_WORDS];
	static StaticTask_t tcb;
	return xTaskCreateStatic(safety_task_fn, "safety", SAFETY_STACK_WORDS, (void *)data,
	                         SAFETY_PRIO, stack, &tcb);
}

void safety_task_fn(void *arg)
{
	app_data_t *data = (app_data_t *)arg;
	stm32f103_t *mcu = &data->board.stm32f103;
	pot_t *apps1 = &data->board.apps1;         /* PB0, ADC2 ch8  */
	pot_t *apps2 = &data->board.apps2;         /* PC5, ADC1 ch15 */
	pressure_sensor_t *bse = &data->board.bse; /* PC3, ADC1 ch13 */

	const uint32_t period = 1000u / SAFETY_FREQ;
	uint32_t entry;

	for (;;)
	{
		entry = osKernelGetTickCount();

		/* --- Acquire pedals. APPS1 is on its own ADC (ADC2); APPS2 and BSE
		 * share ADC1, so switch the channel before each ADC1 conversion. --- */
		apps1->count = stm32f103_adc_read(&mcu->hadc2);
		apps1->percent = pot_get_percent(apps1);

		stm32f103_adc_switch_channel(&mcu->hadc1, BSP_APPS2_ADC_CH);
		apps2->count = stm32f103_adc_read(&mcu->hadc1);
		apps2->percent = pot_get_percent(apps2);

		stm32f103_adc_switch_channel(&mcu->hadc1, bse->channel);
		bse->count = stm32f103_adc_read(&mcu->hadc1);
		bse->percent = pressure_sensor_get_percent(bse);

		data->throttle = (int)apps_throttle_pct(apps1->percent, apps2->percent);
		data->brake = (int)bse->percent;
		set_brakelight(data->brake >= BRAKE_LIGHT_THRESH);
		data->torque_cmd = torque_from_throttle_pct((float)data->throttle);

		/* --- Safety evaluation (all hardware-free, host-tested logic) --- */
		// APPS 10%/100 ms plausibility, open-circuit, and idle recovery
		if (apps_plausibility_update(&data->apps_state, apps1->percent, apps2->percent, period))
			fault_clear(&data->faults, FAULT_APPS);
		else
			fault_set(&data->faults, FAULT_APPS);

		// Brake-sensor open/short circuit
		if (bse->count < BSE_RAW_OPEN_LO || bse->count > BSE_RAW_OPEN_HI)
			fault_set(&data->faults, FAULT_BSE);
		else
			fault_clear(&data->faults, FAULT_BSE);

		// Brake-throttle plausibility latch
		if (bppc_update(&data->bppc_state, (float)data->brake, (float)data->throttle))
			fault_clear(&data->faults, FAULT_BPPC);
		else
			fault_set(&data->faults, FAULT_BPPC);

#if CAN_WATCHDOG_ARMED
		// Inverter/AMS command timeout -> zero torque once the real
		// heartbeat ID is configured and fed from the CAN RX callback).
		if (can_wd_update(&data->can_wd, period))
			fault_set(&data->faults, FAULT_CAN_TIMEOUT);
		else
			fault_clear(&data->faults, FAULT_CAN_TIMEOUT);
#endif

		/* --- Actuate: sole SDC writer. Hard fault => open SDC. --- */
		set_fw(!fault_is_hard(&data->faults));

		/* --- Request a torque TX (gated to zero on fault in canbus_task). --- */
		xTaskNotify(data->canbus_task, CANBUS_APPS, eSetBits);

		/* --- Prove the safety task is alive to the hardware watchdog. --- */
		stm32f103_iwdg_refresh();

		osDelayUntil(entry + period);
	}
}
