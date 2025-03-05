#include "proto/can_catalog.h"

void can_encode_torque_cmd(uint8_t buf[CAN_DLC], int16_t torque_cmd)
{
	uint16_t u = (uint16_t)torque_cmd; /* two's-complement bit pattern */
	buf[0] = (uint8_t)(u & 0xFFu);
	buf[1] = (uint8_t)((u >> 8) & 0xFFu);
	for (int i = 2; i < CAN_DLC; i++)
		buf[i] = 0u;
}

int16_t can_decode_torque_cmd(const uint8_t buf[CAN_DLC])
{
	uint16_t u = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
	return (int16_t)u;
}

float can_decode_dc_bus_pct(const uint8_t buf[CAN_DLC])
{
	return (float)buf[0];
}

bool can_is_inverter_heartbeat(uint32_t id)
{
	return id == CAN_ID_INVERTER_HB;
}
