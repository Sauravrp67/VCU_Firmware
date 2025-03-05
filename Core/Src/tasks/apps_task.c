#include <math.h>

#include "main.h"
#include "tasks/apps_task.h"
#include "device_drivers/can_bus.h"

#define TO_LSB(x) (x & 0xff)
#define TO_MSB(x) (x >> 8) & 0xff

/**
 * @brief Actual APPS task function
 *
 * @param arg App_data struct pointer converted to void pointer
 */
void apps_task_fn(void *arg);

TaskHandle_t apps_task_start(app_data_t *data)
{
    TaskHandle_t handle;
    xTaskCreate(apps_task_fn, "APPS task", 128, (void *)data, APPS_PRIO, &handle);
    return handle;
}

void apps_task_fn(void *arg)
{
    app_data_t *data = (app_data_t *)arg;
    pot_t *apps1 = &data->board.apps1;
    pot_t *apps2 = &data->board.apps2;

    const uint32_t period_ms = 1000u / APPS_FREQ;
    uint32_t entry;

    for(;;)
    {
        entry = osKernelGetTickCount();

        apps1->count = stm32f103_adc_read(apps1->handle);
        apps2->count = stm32f103_adc_read(apps2->handle);
        apps1->percent = pot_get_percent(apps1);
        apps2->percent = pot_get_percent(apps2);

        data->throttle = (int)apps_throttle_pct(apps1->percent, apps2->percent);

        // §5.1: 10% / 100 ms plausibility incl. open-circuit; latches, and now
        // recovers when the channels agree and the pedal returns to idle.
        if(apps_plausibility_update(&data->apps_state,
                                    apps1->percent, apps2->percent, period_ms))
        {
            fault_clear(&data->faults, FAULT_APPS);
        }
        else
        {
            fault_set(&data->faults, FAULT_APPS);
        }

    	xTaskNotify(data->canbus_task, CANBUS_APPS, eSetBits);
        osDelayUntil(entry + period_ms);
    }
}
