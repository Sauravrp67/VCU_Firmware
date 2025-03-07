# CAN Message Catalog

Codec: `Core/proto/can_catalog.{h,c}` (hardware-free, host-tested). CAN1 on
PB8/PB9 (remap CAN1_2). Standard 11-bit IDs, 8-byte DLC.

> ⚠ **PLACEHOLDER VALUES.** The IDs and scaling below are placeholders so the
> codec and TX path compile and are host-tested. They are **not** the production
> catalog. Replace from the inverter + AMS datasheets before flashing on-car.

## Messages

| Name | Dir | ID (placeholder) | Payload | Notes |
|---|---|---|---|---|
| `CAN_ID_VCU_TORQUE_CMD` | VCU → inverter | `0x100` | bytes[0..1] = int16 torque, LE, 1 unit/LSB; bytes[2..7] = 0 | gated to 0 on any torque-inhibiting fault |
| `CAN_ID_INVERTER_HB` | inverter → VCU | `0x200` | (heartbeat) | feeds the CAN command watchdog |
| `CAN_ID_AMS_DCBUS` | AMS → VCU | `0x300` | byte[0] = % of accumulator | for the pre-charge ≥90 % gate |

## Torque command encoding (placeholder)
- Signed 16-bit, little-endian in bytes [0..1]; remaining bytes zero.
- Range clamped by `control/torque` to `[0, TORQUE_MAX_CMD]` with no reverse.
- Confirm the inverter's torque resolution, endianness, sign convention, and any
  enable/state bytes before relying on this.

## To finalize
- Real inverter torque-command message: ID, DLC, byte layout, scaling, endianness.
- Real inverter + AMS heartbeat IDs and periods (watchdog timeout: currently
  `CAN_TIMEOUT_MS` = 100 ms).
- AMS DC-bus voltage message + scaling (for the pre-charge gate).
