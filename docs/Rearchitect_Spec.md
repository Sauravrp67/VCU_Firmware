# VCU Firmware Re-Architecture — Coding Agent Brief

> **How to use this document:** This is a specification, not a patch list. Read the
> existing codebase first, then reconcile it against the contract below. Where the
> code and this spec disagree, **stop and surface the discrepancy** rather than
> silently picking one — several items here are explicitly marked as unresolved and
> need a human decision. Treat every requirement under *Safety Invariants* as binding.

---

## 1. Mission

Re-architect and harden an existing Formula Student / FSAE-EV **Vehicle Control Unit (VCU)**
firmware running on an **STM32F103** (Cortex-M3, ARMv7-M) under a **priority-based FreeRTOS**
model. The current code works on-car but needs:

1. A clean **layered architecture** with a single hardware contract (pin map) as the source of truth.
2. **Safety-critical logic (plausibility, fault handling, RTD sequencing) decoupled from hardware** so it is unit-testable on a host.
3. A **deterministic FreeRTOS task design** that fits the F103's RAM budget and respects the timing windows the rules impose.
4. A reproducible **arm-none-eabi-gcc + Makefile + OpenOCD** build, with a separate host build for tests.

Do **not** change pin assignments, CAN IDs, or safety thresholds without flagging. Preserve all
existing on-car behavior unless it violates a Safety Invariant.

---

## 2. Target Hardware

| Item | Value | Notes |
|---|---|---|
| MCU | STM32F103 (Cortex-M3) | Confirm exact part (C8/CB/RC) from the linker script — flash is 64/128/256 KB, RAM is 20/48 KB. **Budget for 20 KB RAM unless proven otherwise.** |
| RTOS | FreeRTOS, preemptive, priority-based | Confirm tick rate and `configTOTAL_HEAP_SIZE` |
| Toolchain | `arm-none-eabi-gcc` | Existing |
| Flash/debug | OpenOCD | Existing `.cfg` + Makefile target |
| Build | GNU Make | Existing |

---

## 3. Hardware Contract (Pin Map)

Transcribed from the original design (whiteboard). **This is authoritative for the refactor — do not
reassign pins; only flag conflicts.** Validate each against STM32F103 alternate-function tables.

### Inputs

| Function | Pin | Peripheral / type | Validated? |
|---|---|---|---|
| RTD signal | PC4 | Digital or analog (ADC_IN14) | ⚠ pick one — see §7 |
| Current sensor (BSPD-related sense) | PA1 | Analog (ADC_IN1) | ⚠ **conflict** — also listed as Free GPIO 2 |
| CAN RX | PB8 | CAN1_RX (remap) | ✓ |
| CAN TX | PB9 | CAN1_TX (remap) | ✓ |
| APPS sensor 1 | PC5 | Analog (ADC_IN15) | ✓ |
| APPS sensor 2 | PB0 | Analog (ADC_IN8) | ✓ |
| Speed sensor | PC2 | listed "Digital I/P / Analog in" (ADC_IN12) | ⚠ ambiguous — see §7 |
| Voltage sensor (signal-conditioned) | PC3 | Analog (ADC_IN13) | ✓ |
| Free GPIO 1 | PC0 | GPIO (ADC_IN10 capable) | ✓ |
| Free GPIO 2 | PA1 | GPIO | ⚠ **conflict** with current sensor |

### Outputs

| Function | Pin | Peripheral / type | Validated? |
|---|---|---|---|
| RTD buzzer | PA4 | GPIO | ✓ |
| Dashboard UART RX | PA10 | USART1_RX | ✓ |
| Dashboard UART TX | PA9 | USART1_TX | ✓ |
| Dashboard flow control | RTS/CTS | USART1 hardware flow control | confirm if actually wired |
| APPS / torque-request output | PB6 (SCL) / PB7 (SDA) | I2C1 → **MCP4725 DAC** | ✓ — see §7 (analog vs CAN torque path) |
| Brake light (signal-conditioned) | PB2 | GPIO digital out | ✓ |
| Shutdown-circuit (SDC) output | PB10 | GPIO digital out | ✓ — see §5.6 |
| SD card MOSI | PB15 | SPI2_MOSI | ✓ |
| SD card SCK | PB13 | SPI2_SCK | ✓ |
| SD card NSS | PB12 | SPI2_NSS | ✓ |
| SD card MISO | (PB14) | SPI2_MISO | implied, confirm wired |
| Free GPIO out 1 | PB11 | GPIO | ✓ |
| Free GPIO out 2 | PB1 | GPIO | ✓ |

**Deliverable:** a single `board_config.h` (or `bsp_pins.h`) that is the *only* place pins are
defined. No magic pin numbers anywhere else in the tree.

---

## 4. Subsystem Inventory (what the VCU does)

- **Pedal acquisition:** dual-channel APPS (PC5, PB0), brake input, voltage/current sensing — all ADC1.
- **Torque request output:** computed torque → MCP4725 DAC (I2C1) as analog and/or CAN to the inverter. *Resolve the authoritative path (§7).*
- **CAN bus:** interface to inverter and AMS/BMS (PB8/PB9).
- **Dashboard:** telemetry/status over USART1 (PA9/PA10).
- **Data logging:** SD card over SPI2 (FatFs likely).
- **Driver-facing actuators:** RTD buzzer (PA4), brake light (PB2), SDC output (PB10).
- **State machine:** Tractive-System-Off → TS-Active → Pre-charge → Ready-To-Drive → Drive, with fault transitions.

---

## 5. Safety Invariants (BINDING)

These are the binding behaviors. Thresholds below reflect the FSAE-EV ruleset; **rule numbering shifts
year to year (the older `EVx.x` scheme is now `EV.x.x` / `T.x.x`), so confirm exact clause numbers
against the specific competition rulebook this car runs (Formula Bharat 2025 / FSAE 2024–25).** The
*numeric thresholds* below are stable across recent versions.

### 5.1 APPS (accelerator) plausibility
- Two independent APPS are required; torque is derived from them.
- **Implausibility = deviation > 10% pedal travel between the two sensors.** If implausibility
  **persists > 100 ms**, motor power must be shut down completely (the tractive system need not be
  killed — zero-torque is sufficient). Recovery only when the signals agree again and the pedal is released to idle.
- Also treat out-of-range / floating / open-circuit on either channel as implausible.

### 5.2 APPS / Brake pedal plausibility (the "brake-throttle latch")
- When the **mechanical brake is actuated AND APPS indicates > 25% pedal travel** at the same time,
  motor power must be shut down immediately.
- The shutdown **latches** until **APPS drops below 5%** pedal travel — releasing the brake alone does not re-enable torque.

### 5.3 BSPD — *hardware, not firmware*
- The **Brake System Plausibility Device is a standalone, non-programmable circuit** per the rules.
  **The VCU firmware must NOT claim to implement the BSPD.** The current-sense input (PA1) may feed
  VCU monitoring/telemetry, but the rule-mandated BSPD trip (hard braking + >10% throttle for >1 s;
  loss of brake/throttle signal for 100 ms) is the hardware's job. Keep this boundary explicit in the architecture and in comments.

### 5.4 Pre-charge / AIR sequencing
- Pre-charge the intermediate (DC-bus) circuit to **≥ 90% of accumulator voltage before closing the
  second AIR**. Pre-charge must be disabled whenever the shutdown circuit is deactivated.
- Sequence on shutdown: open AIRs, then verify a safe discharge within the rule time limit.

### 5.5 Ready-To-Drive (RTD)
- Entering RTD requires the **tractive system active AND a dedicated driver action that includes the
  brake pedal being pressed** (e.g., brake + start button).
- On entering RTD, emit the **RTD sound continuously for 1–3 s, ≥ 80 dBA (fast weighting)**, via the
  buzzer (PA4). The car is "ready to drive" only once motors will respond to APPS.

### 5.6 Shutdown circuit (SDC) & fault response
- The SDC carries the current driving the AIRs; the VCU's SDC output (PB10) participates in / monitors it.
- Any safety fault (APPS implausibility, brake-throttle latch, CAN timeout, sensor open-circuit,
  watchdog) must drive a **fail-safe zero-torque state** and the appropriate SDC/AIR action. Faults are latched; recovery is explicit and gated on safe conditions.

### 5.7 Torque/command limits
- Commanded torque clamped to the rulebook power cap (e.g., **80 kW**); **no reverse**.
- CAN command path to the inverter must have a **timeout watchdog** that forces zero-torque on loss of communication.

---

## 6. Target Architecture

Refactor toward strict layering; dependencies point downward only.

```
app/        FreeRTOS tasks, wiring of modules, main()
control/    state machine, torque map, plausibility logic   <- HARDWARE-FREE, host-testable
safety/     fault manager, watchdog policy, latch logic      <- HARDWARE-FREE, host-testable
proto/      CAN message catalog/codec, dashboard protocol     <- HARDWARE-FREE, host-testable
drivers/    can, adc, i2c (mcp4725), uart, spi/sd, gpio       <- thin, hardware-touching
bsp/        clock, pin config (board_config.h), startup, NVIC
rtos/       FreeRTOSConfig.h, hooks
test/       host unit tests for control/, safety/, proto/
```

Hard rules:
- **`control/`, `safety/`, and `proto/` must not `#include` any STM32 / CMSIS / driver header.**
  They take plain inputs (e.g., `apps1_pct`, `apps2_pct`, `brake_pct`, `dt_ms`) and return decisions
  (e.g., `torque_request`, `fault_flags`). This is what makes the safety logic testable without hardware.
- Drivers expose narrow interfaces; no business logic in drivers.
- No dynamic allocation after init. Prefer static FreeRTOS objects (`xTaskCreateStatic`, static queues).
- A single fault enum + fault manager; every module reports faults, only the fault manager decides the system response.

---

## 7. Discrepancies To Resolve (decide with a human; do not guess)

1. **PA1 double-assignment** — current-sensor analog input vs. "Free GPIO 2." One must move. Recommend keeping the current sense and reassigning the free GPIO.
2. **Torque path** — MCP4725 DAC (analog) vs. CAN torque command. Which is authoritative to the inverter? If both exist, define the relationship (e.g., DAC is the live command, CAN is status/AMS) and ensure the plausibility shutdowns drive *the actual command path* to zero.
3. **Speed sensor on PC2** — if it is a frequency/hall pulse, it belongs on a timer input-capture pin, not an ADC channel. Confirm sensor type; re-route if needed.
4. **RTD on PC4** — digital line or analog? Pick one and document.
5. **USART1 RTS/CTS** — is hardware flow control actually wired, or are RTS/CTS unused? Don't configure flow control that isn't connected.
6. **Exact F103 variant** — flash/RAM size drives the linker script and the FreeRTOS heap; confirm before sizing tasks.

---

## 8. FreeRTOS Task Architecture (propose, then justify)

Produce a task table and justify priorities against the timing windows in §5. A reasonable starting point —
adjust to the real code:

| Task | Priority | Period / trigger | Responsibility |
|---|---|---|---|
| `safety_monitor` | highest | 1–10 ms | run plausibility checks, evaluate fault manager, enforce zero-torque/SDC |
| `pedal_acquire` | high | 1–5 ms | sample APPS×2 + brake via ADC, scale, hand to control |
| `torque_command` | high | matched to control loop | apply torque map + limits, write DAC / CAN command |
| `can_rx` / `can_tx` | high | event / periodic | inverter + AMS messaging, command timeout watchdog |
| `state_machine` | medium | 10 ms | TS/precharge/RTD/drive transitions, buzzer timing |
| `dashboard` | low | 20–100 ms | UART telemetry |
| `datalog` | lowest | 100 ms+ | SD/FatFs writes (must never block higher tasks) |

Requirements:
- The **100 ms APPS window** and the **brake-throttle latch** must be evaluated at a rate that
  guarantees detection well within the limit — do not run plausibility only in a slow task.
- No priority inversion on shared resources (ADC, CAN, the fault state). Use mutexes with priority
  inheritance or lock-free single-writer patterns; document the choice.
- The hardware watchdog (IWDG) must be fed from a path that proves the safety task is alive — not from a low-priority task.
- Size every task stack and report total RAM against the budget.

---

## 9. Build & Toolchain

- Keep `arm-none-eabi-gcc` + Make + OpenOCD. Refactor the Makefile to be modular:
  - `make` → target ELF/BIN/HEX, `make flash` → OpenOCD, `make clean`.
  - Separate **host test build** (`make test`) compiling `control/`, `safety/`, `proto/` with the
    native gcc/clang + a unit-test runner (Unity/Ceedling or similar). This is the payoff of the hardware-free layering.
- Treat warnings as errors on the firmware build (`-Wall -Wextra -Werror`), `-ffunction-sections
  -fdata-sections` + `--gc-sections`, and verify the linker map fits flash/RAM.
- Keep the linker script and startup explicit and commented; confirm vector table and heap/stack regions.

---

## 10. Coding Standards & Constraints

- Safety-critical C style: no `malloc` after init, no recursion in safety paths, bounded loops,
  explicit `volatile` for hardware/ISR-shared state, fixed-width types.
- Lean toward MISRA-C-style discipline for `control/` and `safety/`; note deviations.
- All ISRs short; defer work to tasks via queues/notifications.
- Every magic number tied to a rule (10%, 100 ms, 25%, 5%, 90%, 80 kW, 1–3 s) must be a named
  constant with a comment citing the rule area.
- No silent failure paths: every error returns or sets a fault.

---

## 11. Deliverables

1. A short **refactor plan** (ordered, low-risk-first) before touching code.
2. The **layered directory tree** + `board_config.h` as the single pin source of truth.
3. **Hardware-free `control/`, `safety/`, `proto/`** modules with clean interfaces.
4. **Host unit tests** covering: APPS 10%/100 ms plausibility, brake-throttle 25%/5% latch,
   CAN-timeout zero-torque, torque clamp/no-reverse, RTD entry gating + 1–3 s buzzer, precharge ≥90% gate.
5. Updated **modular Makefile** with `flash` and `test` targets.
6. Generated docs: **pin-map table, task table, fault table, CAN message catalog** (Markdown).
7. A written list of every **discrepancy from §7** with the decision taken (or left open).

---

## 12. Acceptance Criteria

- Firmware builds clean (`-Werror`) and links within the F103 flash/RAM budget.
- `make test` passes on host with the safety-invariant tests above; coverage of `control/` + `safety/` is reported.
- No pin defined outside `board_config.h`; no STM32/CMSIS include inside `control/`, `safety/`, `proto/`.
- Every Safety Invariant in §5 maps to a named, tested code path, and the BSPD is documented as hardware (not implemented in firmware).
- All §7 discrepancies are resolved or explicitly logged as open with rationale.

---

### Source notes on the rule thresholds (for the human, not the agent)
APPS implausibility = >10% pedal-travel deviation, power cut if it persists >100 ms; brake-throttle
plausibility cuts power when brake is actuated with >25% APPS and latches until APPS <5%; RTD sound
1–3 s at ≥80 dBA with a brake-inclusive activation action; pre-charge to ≥90% of accumulator voltage
before closing the second AIR; BSPD is a required standalone non-programmable circuit. Confirm clause
numbers against the exact competition rulebook before relying on them in a design report.