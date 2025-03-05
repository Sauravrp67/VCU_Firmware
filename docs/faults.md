# Fault Table

Single fault registry: `Core/safety/fault.h` (`fault_flag_t` + `fault_mgr_t`).
Every module reports faults as bits; **only `error_task` acts on them** and is
the sole writer of the shutdown circuit (SDC). Faults latch; recoverable ones are
cleared explicitly by their owning task.

| Flag | Source | Class | System response |
|---|---|---|---|
| `FAULT_APPS` | apps_task (§5.1) | hard | open SDC + zero-torque |
| `FAULT_BSE` | bse_task open-circuit (§5.6) | hard | open SDC + zero-torque |
| `FAULT_CAN_TIMEOUT` | CAN watchdog (§5.7) | hard | open SDC + zero-torque |
| `FAULT_BPPC` | bppc_task latch (§5.2) | torque-inhibit | zero-torque (SDC stays closed) |
| `FAULT_CLI` | cli_task | soft | none (status only) |
| `FAULT_CANBUS_TX` | canbus_task TX error | soft | none (status only) |
| `FAULT_DASHBOARD` | dashboard_task | soft | none (status only) |

- `FAULT_HARD_MASK` = APPS | BSE | CAN_TIMEOUT → opens the SDC.
- `FAULT_TORQUE_INHIBIT_MASK` = hard mask | BPPC → forces transmitted torque to 0
  (enforced at the CAN send boundary in `canbus_task`).

## Not a firmware fault
- **BSPD** (§5.3) is a standalone, non-programmable hardware circuit. The firmware
  deliberately does **not** implement a BSPD trip. The current sensor (PA7) feeds
  monitoring/telemetry only.

## Recovery
- `FAULT_APPS`: clears when the two channels agree **and** the pedal returns to
  idle (`control/apps`).
- `FAULT_BPPC`: clears when APPS drops below 5 % (`safety/plausibility`).
- `FAULT_BSE` / soft faults: cleared by their owning task when the condition ends.
