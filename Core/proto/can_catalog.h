/**
 * @file    can_catalog.h
 * @brief   CAN message catalog + codec (hardware-free).
 *
 * Layer: proto/ — pure, host-testable, no STM32/CMSIS/HAL includes. Operates on
 * plain 8-byte payloads; the driver/app layer bridges these to the HAL TX/RX
 * packets.
 *
 * ============================ PLACEHOLDER VALUES ============================
 * The message IDs and torque scaling below are PLACEHOLDERS so the codec and the
 * TX torque path compile and can be host-tested. They are NOT the production CAN
 * identifiers. Before flashing on-car, replace them with the real inverter and
 * AMS catalog (message ID, DLC, byte order, scaling/endianness, heartbeat period)
 * from the device datasheets. Tracked in docs/discrepancies.md.
 * ===========================================================================
 */
#ifndef PROTO_CAN_CATALOG_H
#define PROTO_CAN_CATALOG_H

#include <stdbool.h>
#include <stdint.h>

#define CAN_DLC 8

/* PLACEHOLDER identifiers — replace with the real catalog. */
#define CAN_ID_VCU_TORQUE_CMD  0x100u  /* VCU -> inverter torque command   */
#define CAN_ID_INVERTER_HB     0x200u  /* inverter status heartbeat (§5.7) */
#define CAN_ID_AMS_DCBUS       0x300u  /* AMS DC-bus voltage (§5.4)         */

/* PLACEHOLDER torque encoding: signed 16-bit, 1 unit/LSB, little-endian in
 * bytes [0..1]; remaining bytes zero. Confirm resolution + endianness. */
void    can_encode_torque_cmd(uint8_t buf[CAN_DLC], int16_t torque_cmd);
int16_t can_decode_torque_cmd(const uint8_t buf[CAN_DLC]);

/* PLACEHOLDER AMS DC-bus percent-of-accumulator decode: byte 0 = percent. */
float   can_decode_dc_bus_pct(const uint8_t buf[CAN_DLC]);

/* Recognize the inverter heartbeat frame (feeds the CAN command watchdog). */
bool    can_is_inverter_heartbeat(uint32_t id);

#endif /* PROTO_CAN_CATALOG_H */
