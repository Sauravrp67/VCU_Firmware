# Third-Party Dependencies

All third-party code is **vendored in-tree** (committed under `Drivers/` and
`Middlewares/`) rather than fetched at build time. For embedded/safety firmware
this is a deliberate choice: it guarantees reproducible, offline, version-locked
builds and keeps the exact sources that were validated on the bench. The trees
are marked `linguist-vendored` in `.gitattributes` so they are excluded from
GitHub's language stats and diffs.

Everything here originates from a single **STM32CubeMX / STM32Cube FW_F1**
project generation; `VCU_Version1.ioc` is the configuration of record.

| Component | Version | Location | License |
|---|---|---|---|
| STM32Cube FW_F1 (bundle) | V1.8.6 | — | mixed (per component below) |
| CMSIS Core (Cortex-M) | from FW_F1 V1.8.6 | `Drivers/CMSIS/Include` | Apache-2.0 |
| CMSIS Device STM32F1xx | from FW_F1 V1.8.6 | `Drivers/CMSIS/Device/ST/STM32F1xx` | BSD-3-Clause (ST) |
| STM32F1xx HAL Driver | from FW_F1 V1.8.6 | `Drivers/STM32F1xx_HAL_Driver` | BSD-3-Clause (ST) |
| FreeRTOS kernel | V10.3.1 | `Middlewares/Third_Party/FreeRTOS` | MIT |
| CMSIS-RTOS v2 wrapper | from FW_F1 V1.8.6 | `Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2` | Apache-2.0 |
| FatFs | R0.11 | `Middlewares/Third_Party/FatFs` | BSD-style (ChaN) |
| GNU Arm Embedded toolchain | 13.x (`arm-none-eabi-gcc`) | external | GPL (compiler) / runtime exceptions |

## Updating

These come as a set from the Cube package. To update, regenerate from
STM32CubeMX against a newer FW_F1 package (re-applying the `USER CODE` regions),
or replace the `Drivers/` and `Middlewares/` trees from a matching Cube package
and re-run the build + host tests.

## Not converted to submodule / FetchContent

A git submodule or CMake `FetchContent` of the full STM32CubeF1 package was
considered and intentionally **not** adopted: the in-tree layout is intertwined
with the CubeMX-generated glue (`FATFS/`, device headers, linker script,
startup), the package is large, and pinning a vendored snapshot is simpler and
more reproducible for this use case. Revisit if the project later standardizes on
a package manager (e.g. Conan/West) across multiple firmware repos.
