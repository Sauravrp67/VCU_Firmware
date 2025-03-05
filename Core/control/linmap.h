/**
 * @file    linmap.h
 * @brief   Linear range mapping (hardware-free).
 *
 * Layer: control/ — pure, host-testable, no STM32/CMSIS/HAL includes.
 * Extracted from the Arduino-style map() in the former device_drivers/map.c so
 * sensor scaling can be unit-tested on a host.
 */
#ifndef CONTROL_LINMAP_H
#define CONTROL_LINMAP_H

/**
 * Map x from [in_min, in_max] onto [out_min, out_max] with rounding.
 * Returns out_min + out_range/2 if the input range is degenerate (in_min==in_max).
 */
long double linmap(long x, long in_min, long in_max, long out_min, long out_max);

#endif /* CONTROL_LINMAP_H */
