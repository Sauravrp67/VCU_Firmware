#include "app/rtd_task.h"
#include "main.h"

/**
 * @brief Actual RTD task function
 *
 * @param arg App_data struct pointer converted to void pointer
 */
void rtd_task_fn(void *arg);

#define RTD_STACK_WORDS 128
TaskHandle_t rtd_task_start(app_data_t *data)
{
	static StackType_t stack[RTD_STACK_WORDS];
	static StaticTask_t tcb;
	return xTaskCreateStatic(rtd_task_fn, "RTD task", RTD_STACK_WORDS, (void *)data, RTD_PRIO,
	                         stack, &tcb);
}

void rtd_task_fn(void *arg)
{
	app_data_t *data = (app_data_t *)arg;
	const uint32_t period = 50; /* ms */
	uint32_t buzzer_start = 0;

	for (;;)
	{
		data->tsal = HAL_GPIO_ReadPin(AIR_Status_GPIO_Port, AIR_Status_Pin);
		data->rtd_button = HAL_GPIO_ReadPin(RTD_Input_GPIO_Port, RTD_Input_Pin);

		// A hard fault forces the fail-safe state from anywhere. The SDC itself
		// is driven by the safety monitor, not here.
		if (fault_is_hard(&data->faults))
		{
			set_buzzer(0);
			data->vcu_state = VCU_STATE_FAULT;
		}

		switch (data->vcu_state)
		{
		case VCU_STATE_FAULT:
			if (!fault_is_hard(&data->faults))
				data->vcu_state = VCU_STATE_TS_OFF;
			break;

		case VCU_STATE_TS_OFF:
			set_buzzer(0);
			// AIRs closed (tsal) => external pre-charge complete. Pre-charge
			// voltage gating is enabled once the production AMS DC-bus signal
			// is configured.
			if (data->tsal)
				data->vcu_state = VCU_STATE_TS_ACTIVE;
			break;

		case VCU_STATE_TS_ACTIVE:
			if (!data->tsal)
			{
				data->vcu_state = VCU_STATE_TS_OFF;
			}
			// RTD entry requires TS active and a brake-inclusive driver action.
			else if (rtd_entry_allowed(data->tsal, data->brakelight, data->rtd_button))
			{
				set_buzzer(1);
				buzzer_start = osKernelGetTickCount();
				data->vcu_state = VCU_STATE_RTD;
			}
			break;

		case VCU_STATE_RTD:
			// Sound the buzzer for 1-3 s, non-blocking, then enter Drive.
			if (!data->tsal || !data->rtd_button)
			{
				set_buzzer(0);
				data->vcu_state = VCU_STATE_TS_OFF;
			}
			else if ((osKernelGetTickCount() - buzzer_start) >= RTD_BUZZER_MS)
			{
				set_buzzer(0);
				data->vcu_state = VCU_STATE_DRIVE;
			}
			break;

		case VCU_STATE_DRIVE:
			// Ready to drive: torque responds to APPS (gated by the fault mgr).
			if (!data->tsal || !data->rtd_button)
				data->vcu_state = VCU_STATE_TS_OFF;
			break;

		case VCU_STATE_PRECHARGE:
		default:
			data->vcu_state = VCU_STATE_TS_OFF;
			break;
		}

		osDelay(period);
	}
}
