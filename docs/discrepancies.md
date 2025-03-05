# §7 Discrepancy Resolution Log

This document records every discrepancy found while reconciling the existing
firmware against `docs/Rearchitect_Spec.md`, and the decision taken for each
(Rearchitect Spec deliverable §11.7). It is the authoritative record of *why*
the refactor's pin map and torque path are what they are.

**Source-of-truth decision (overarching):** The **as-built code**
(`Core/Inc/main.h`, `Core/Src/board.c`, `Core/Src/stm32f1xx_hal_msp.c`) is the
authoritative pin map, because it reflects the hardware proven on-car. Where the
spec §3 "whiteboard" map disagrees, **spec §3 is corrected to match the code**,
not the reverse. The spec itself flags §3 as an unvalidated transcription.

---

## Resolved §7 items

### 7.1 — PA1 double-assignment
- **Spec §3/§7.1:** PA1 = current sensor (ADC_IN1) *and* "Free GPIO 2"; one must move.
- **As-built reality:** PA1 is **SDC status input** (`SDC_Status_In`, GPIO input,
  `main.h:69`). The **current sensor is on PA7** (`CurrentSensor_Pin`, ADC_IN7,
  `main.h:79`) — not PA1 at all.
- **Decision:** Keep as-built. `board_config.h`: PA1 = SDC status in, PA7 =
  current sense. The §7.1 "conflict" does not exist in hardware; spec §3 is wrong.

### 7.2 — Torque command path
- **Spec §7.2:** MCP4725 DAC (analog) vs CAN — which is authoritative to the inverter?
- **As-built reality:** I2C1 pins (PB6/PB7) are initialized but there is **no
  MCP4725 driver**. The CAN TX task exists but `tx_packet` is **never populated
  with a torque value** (`throttle_hex` and `MAXTRQ=160` are dead). Today no
  torque command reaches the inverter by either path.
- **Decision:** **CAN command is authoritative.** Build the CAN torque message
  and populate `tx_packet`; all plausibility/fault/timeout paths must drive the
  **CAN torque** to zero. MCP4725/I2C1 is dropped from the torque path.

### 7.3 — Speed sensor (PC2)
- **Spec §7.3:** ambiguous digital-vs-ADC; if a hall/frequency pulse it needs a
  timer input-capture pin, not ADC.
- **As-built reality:** PC2 is **not configured anywhere** — no GPIO, ADC, or
  timer. Speed sensing is unimplemented.
- **Decision:** Implement as a **timer input-capture** (hall/frequency pulse) —
  new `drivers/timer_capture` + speed task.

### 7.4 — RTD signal line
- **Spec §3/§7.4:** RTD on PC4, digital-or-analog unresolved.
- **As-built reality:** RTD driver action is a **digital input on PC1**
  (`RTD_Input`, `main.h:65`); AIR/TSAL status is on PC0 (`AIR_Status`). PC4 is
  unused.
- **Decision:** Keep as-built. RTD = **digital input on PC1**. Spec's PC4 dropped.

### 7.5 — USART1 RTS/CTS hardware flow control
- **Spec §3/§7.5:** confirm if flow control is actually wired.
- **As-built reality:** `huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE`
  (`main.c:450`). PA11/PA12 are configured as plain GPIO but inactive.
- **Decision:** Flow control is **not wired**. Do not configure RTS/CTS.

### 7.6 — Exact F103 variant
- **Spec §7.6:** confirm part (flash/RAM drives linker + heap).
- **As-built reality:** `STM32F103RBTx` — FLASH 128 KB @ 0x08000000, **RAM 20 KB**
  @ 0x20000000 (`STM32F103RBTX_FLASH.ld`; `STM32F103xB` in `CMakeLists.txt`).
  FreeRTOS heap_4, 3072 B, 1 kHz tick.
- **Decision:** Target **STM32F103RB; budget all task stacks against 20 KB RAM.**

---

## Additional discrepancies (beyond §7) found during reconciliation

These were not anticipated by §7 but contradict the spec §3 "authoritative" map.
All resolved in favor of as-built per the overarching decision.

| Function | Spec §3 | As-built code | Resolution |
|---|---|---|---|
| APPS1 / APPS2 | PC5 / PB0 | **PB0 / PC5** (labels swapped); APPS1→ADC2, APPS2→ADC1 (`board.c:8-9`) | Keep as-built; two independent ADC peripherals (good for plausibility). |
| PC3 function | Voltage sensor | **Brake-pressure (BSE)**, ADC_IN13 (`board.h:29`, `board.c:10`) | Keep as-built (PC3 = BSE). |
| Accumulator voltage | analog sense implied | **No voltage ADC pin** exists | Pre-charge ≥90 % gate must source voltage from **AMS over CAN** — confirm during Step 5. |
| CLI UART | not in spec | **USART2 on PA2/PA3** (`main.h:71-74`) | Document; dashboard stays on USART1 (PA9/PA10). |
| Onboard LED / button | not in spec | LD2=PA5, B1=PC13 (Nucleo-F103RB leftovers) | Document as board artifacts. |

---

## Build system (spec §9 vs repo)
- **Spec §9:** Makefile + OpenOCD.
- **Reality:** repo migrated to **CMake** (`CMakeLists.txt`, `CMakePresets.json`,
  `cmake/arm-none-eabi-gcc.cmake`, `scripts/`).
- **Decision:** **Keep CMake** (already arm-none-eabi-gcc + OpenOCD-capable);
  add a separate native/host CMake target for the `control/`/`safety/`/`proto/`
  unit tests. Honors §9's intent (modular build + host test build) without
  discarding the migration.

---

## Deliberate behavior changes (Step 5 — flagged for race-engineer review)

These correct weaknesses that were below a §5 invariant. Everything else is
preserved as-is.

1. **Single SDC writer.** Previously `error_task`, `bppc_task`, and `rtd_task`
   all wrote the shutdown-circuit output every cycle (a 3-way race). Now the
   **fault manager in `error_task` is the only writer**: SDC opens iff a *hard*
   fault is latched.
2. **Brake-throttle (BPPC) now cuts torque, not the tractive system.** The old
   code drove `set_fw(0)` (opened SDC) on a brake-throttle latch. Per §5.1/§5.2
   zero-torque is sufficient, so BPPC now raises `FAULT_BPPC` → torque held at
   zero, SDC stays closed. **This is less aggressive than before** — confirm it
   matches your rules interpretation.
3. **APPS fault can now recover.** `apps_fault` previously latched forever. The
   new `control/apps` clears the latch only when the channels agree *and* the
   pedal returns to idle (§5.1).
4. **APPS open-circuit / out-of-range** detection added (was absent).
5. **BSE open-circuit** detection added (raw ADC outside [50, 4000]); `bse_fault`
   was previously declared but never set.
6. **RTD buzzer is non-blocking.** Replaced the blocking `osDelay(3000)` with a
   timed state in the new TS-Off→TS-Active→RTD→Drive state machine; the
   RTD-exit SDC pulse was removed (the fault manager owns the SDC). Buzzer
   duration kept at the as-built 3 s (upper bound of the 1-3 s window).
7. **CAN command watchdog** (`safety/fault` `can_watchdog_t`) added but **not yet
   armed** — it is wired to CAN RX feeding in Step 6 to avoid a false zero-torque
   before the inverter heartbeat is decoded.

## Step 6 status — CAN torque path (PLACEHOLDER catalog)
- `proto/can_catalog.{h,c}` implements a hardware-free, host-tested torque codec.
  The torque command is now actually built and transmitted: `apps_task` computes
  the clamped request, `canbus_task` encodes it and applies the §5.7 zero-torque
  gate at the send boundary (`CAN_ID_VCU_TORQUE_CMD`).
- **The message IDs and torque scaling are PLACEHOLDERS** (`0x100/0x200/0x300`,
  1 unit/LSB little-endian). NOT flashable-correct until replaced with the real
  inverter/AMS catalog (ID, DLC, byte order, scaling, heartbeat period).
- The **CAN-timeout watchdog** mechanism (`safety/fault` + `can_is_inverter_heartbeat`)
  is host-tested but **not yet armed**: no CAN RX path exists (no `CAN1_RX0`
  IRQ/NVIC, no `HAL_CAN_RxFifo0MsgPendingCallback`). Arming it (feed on the
  inverter heartbeat) is wired in Step 7 alongside the NVIC/RTOS work, once the
  real heartbeat ID is known — left unarmed for now to avoid a false permanent
  zero-torque from an unfed placeholder watchdog.

## Open items to confirm during implementation
- **Inverter torque-command message**: ID, DLC, byte layout, scaling, endianness.
- **Inverter + AMS heartbeat** IDs and periods (for the CAN command watchdog).
- **Accumulator voltage source** for the pre-charge ≥90 % gate: no ADC pin exists,
  so it almost certainly arrives via AMS/BMS over CAN (`CAN_ID_AMS_DCBUS`
  placeholder) — confirm message/scaling before arming the gate.
