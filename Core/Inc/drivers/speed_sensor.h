/**
 * @file    speed_sensor.h
 * @brief   Wheel/driveshaft speed sensor on PC2 via EXTI edge counting.
 *
 * NOTE (hardware constraint): on the STM32F103, PC2 has NO timer-channel
 * alternate function (it is ADC_IN12 / EXTI2 only), so the spec's "timer
 * input-capture" is not possible on this pin. We therefore count rising edges
 * via EXTI2 and convert the count over a fixed window to a frequency. For higher
 * precision at speed, either move the sensor to a timer-capable pin or use a
 * free-running timer base. Tracked in docs/discrepancies.md.
 */
#ifndef __SPEED_SENSOR_H_
#define __SPEED_SENSOR_H_

#include <stdint.h>

typedef struct {
	volatile uint32_t pulses; /* edge count since the last update (ISR-incremented) */
	float hz;                 /* most recent computed pulse frequency */
} speed_sensor_t;

/** Configure PC2 as an EXTI2 rising-edge interrupt and enable its NVIC line. */
void speed_sensor_init(speed_sensor_t *s);

/** Call from the EXTI2 ISR on each edge. */
void speed_sensor_on_edge(speed_sensor_t *s);

/** Consume the accumulated edge count over window_ms and update `hz`. */
void speed_sensor_update(speed_sensor_t *s, uint32_t window_ms);

#endif
