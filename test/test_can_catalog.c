/* proto/ CAN codec (placeholder catalog). Verifies round-trip + framing. */
#include "proto/can_catalog.h"
#include "test_framework.h"

#include <string.h>

static void test_torque_roundtrip(void)
{
	uint8_t buf[CAN_DLC];
	int16_t vals[] = {0, 1, 160, -1, -160, 32767, -32768};
	for (unsigned i = 0; i < sizeof(vals)/sizeof(vals[0]); i++) {
		can_encode_torque_cmd(buf, vals[i]);
		CHECK_EQ_INT(can_decode_torque_cmd(buf), vals[i]);
	}
}

static void test_torque_is_little_endian_and_zero_padded(void)
{
	uint8_t buf[CAN_DLC];
	memset(buf, 0xAA, sizeof(buf));
	can_encode_torque_cmd(buf, 0x1234);
	CHECK_EQ_INT(buf[0], 0x34); /* LSB first */
	CHECK_EQ_INT(buf[1], 0x12);
	for (int i = 2; i < CAN_DLC; i++)
		CHECK_EQ_INT(buf[i], 0x00); /* remaining bytes cleared */
}

static void test_zero_torque_frame_is_all_zero(void)
{
	uint8_t buf[CAN_DLC];
	can_encode_torque_cmd(buf, 0);
	for (int i = 0; i < CAN_DLC; i++)
		CHECK_EQ_INT(buf[i], 0x00);
}

static void test_heartbeat_recognizer(void)
{
	CHECK_TRUE(can_is_inverter_heartbeat(CAN_ID_INVERTER_HB));
	CHECK_FALSE(can_is_inverter_heartbeat(CAN_ID_VCU_TORQUE_CMD));
	CHECK_FALSE(can_is_inverter_heartbeat(0x7FF));
}

static void test_dc_bus_decode(void)
{
	uint8_t buf[CAN_DLC] = {90, 0, 0, 0, 0, 0, 0, 0};
	CHECK_NEAR(can_decode_dc_bus_pct(buf), 90.0, 1e-6);
}

int main(void)
{
	RUN(test_torque_roundtrip);
	RUN(test_torque_is_little_endian_and_zero_padded);
	RUN(test_zero_torque_frame_is_all_zero);
	RUN(test_heartbeat_recognizer);
	RUN(test_dc_bus_decode);
	TEST_MAIN_END();
}
