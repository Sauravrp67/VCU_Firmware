#ifndef __SAFETY_TASK_H_
#define __SAFETY_TASK_H_

#include "app.h"
#include "cmsis_os.h"

/**
 * @brief Starts the safety_monitor task — the highest-priority task that owns
 *        APPS/BSE acquisition, all plausibility checks, the fault manager, the
 *        shutdown-circuit output, and the IWDG refresh (§8).
 */
TaskHandle_t safety_task_start(app_data_t *data);

#endif
