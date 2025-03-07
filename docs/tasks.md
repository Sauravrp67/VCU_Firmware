# FreeRTOS Task Table

FreeRTOS 10, preemptive, 1 kHz tick. All application tasks use **static
allocation** (`xTaskCreateStatic`); the idle/timer/default tasks and mutexes are
created at init. RAM budget **20 KB**; firmware links at ~74 % (debug).

| Task | Priority | Period / trigger | Stack (words) | Responsibility |
|---|---|---|---|---|
| **safety_monitor** | 24 (highest) | 5 ms (200 Hz) | 256 | APPS+BSE acquisition, all plausibility/latch checks, fault manager, **sole SDC writer**, **IWDG refresh** |
| rtd_task | 16 (`RTD_PRIO`) | 50 ms | 128 | TS-Off→TS-Active→RTD→Drive state machine + non-blocking buzzer |
| cli_task | 15 | 10 Hz | 128 | serial CLI |
| canbus_task | 14 | event (notify) | 256 | CAN TX of the gated torque command |
| dashboard_task | 4 | 5 Hz | 256 | UART telemetry; computes wheel speed over a 200 ms window |
| defaultTask | Normal | 1 ms | 128 | idle placeholder (dynamic, CubeMX) |

## Safety loop (safety_monitor)
The time-critical safety logic runs in **one highest-priority task at 5 ms**, so
the 100 ms APPS plausibility window and the brake-throttle latch are detected
with ~20× margin, and there is a single writer of the shutdown circuit. APPS1 is on
ADC2; APPS2 and BSE share ADC1 with explicit per-read channel switching. The
hardware **IWDG** (~0.5 s, register-level) is refreshed only here, so a hung
safety loop forces a reset.

## Interrupts
| IRQ | Priority | Purpose |
|---|---|---|
| `EXTI2` | 5 | speed-sensor edge count (PC2) |
| `USB_LP_CAN1_RX0` | 5 | CAN RX → feed CAN watchdog (heartbeat) + decode AMS DC-bus |
| `EXTI15_10` | 5 | user button (PC13) |
| `TIM3` | — | HAL time base |

NVIC priorities are ≥ `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` so the ISRs
may safely use FreeRTOS FromISR APIs.

## Remaining / to confirm on hardware
- **CAN watchdog** is wired (RX feeds it) but **disarmed** (`CAN_WATCHDOG_ARMED 0`)
  until the real inverter heartbeat ID replaces the placeholder.
- **Pre-charge gate**: `dc_bus_pct` is decoded from a placeholder AMS message but
  not yet used to gate RTD entry (would block on placeholder data).
- Stack high-water marks should be measured on hardware (`uxTaskGetStackHighWaterMark`).
- Speed sensor uses EXTI edge-counting because PC2 has no timer channel; see
  `design_notes.md` before changing the implementation.
