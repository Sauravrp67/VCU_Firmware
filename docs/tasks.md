# FreeRTOS Task Table

Current task design (`Core/Src/app.c`, priorities/periods in `Core/Inc/app.h`).
FreeRTOS 10, preemptive, 1 kHz tick, `configTOTAL_HEAP_SIZE` 3072 B, heap_4.
RAM budget: **20 KB** (firmware currently links at ~51 %).

| Task | Priority | Period / trigger | Stack (words) | Responsibility |
|---|---|---|---|---|
| error_task | 17 | 20 Hz (50 ms) | 128 | central fault manager; sole SDC writer |
| rtd_task | 20 ⚠ | 50 ms loop | 128 | TS-Off→TS-Active→RTD→Drive state machine + buzzer |
| cli_task | 15 | 10 Hz | 128 | serial CLI |
| apps_task | 14 | 20 Hz | 128 | APPS read, throttle, plausibility, torque request |
| canbus_task | 14 | event (notify) | 512 | CAN TX of the gated torque command |
| bppc_task | 8 | 20 Hz | 128 | brake-throttle plausibility latch |
| bse_task | 7 | 20 Hz | 128 | brake read, open-circuit check, brake light |
| dashboard_task | 4 | 5 Hz | 128 | UART telemetry |
| defaultTask | Normal | 1 ms loop | 128 | idle placeholder |

## Known issues to address in the FreeRTOS redesign (Step 7)
- ⚠ **rtd_task uses a literal priority 20** instead of `RTD_PRIO` (16) — reconcile.
- **No dedicated `safety_monitor`**: plausibility runs in apps_task/bppc_task at
  20 Hz. Spec §8 wants the safety checks at 1–10 ms in the highest-priority task,
  fed into the fault manager, to guarantee detection well inside the 100 ms window.
- **Dynamic allocation** (`xTaskCreate`): spec §8/§6 wants static allocation
  (`xTaskCreateStatic`, static queues), no dynamic allocation after init.
- **No IWDG**: the independent watchdog should be fed from a path that proves the
  safety task is alive.
- **CAN watchdog not armed**: needs the CAN RX path (NVIC + RX callback) and the
  real inverter heartbeat ID.
- **Shared resources** (ADC1/ADC2, CAN, fault state): document the
  no-priority-inversion strategy (priority-inheriting mutex or single-writer).
- Per-task **stack sizing** vs the 20 KB budget should be measured and reported.
