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

    float throttle_raw;
    uint16_t throttle_hex;
    uint32_t entry;

    for(;;)
    {
        entry = osKernelGetTickCount();

        apps1->count = stm32f103_adc_read(apps1->handle);
        apps2->count = stm32f103_adc_read(apps2->handle);
        apps1->percent = pot_get_percent(apps1);
        apps2->percent = pot_get_percent(apps2);

        throttle_raw = (apps1->percent + apps2->percent) / 2;
        data->throttle = (int)throttle_raw;

        // T.4.2.5 (2022)
        if(!pot_check_plausibility(apps1->percent, apps2->percent, PLAUSIBILITY_THRESH, APPS_FREQ / 10))
        {
            data->apps_fault = true;
        }

    	xTaskNotify(data->canbus_task, CANBUS_APPS, eSetBits);
        osDelayUntil(entry + (1000 / APPS_FREQ));
    }
}
