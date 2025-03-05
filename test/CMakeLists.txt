# Host (native) unit-test build for the hardware-free layers.
# Compiled with the host gcc/clang — NOT the ARM cross toolchain. This is the
# payoff of keeping control/ and safety/ free of any STM32/CMSIS/HAL include.
#
# Usage:
#   cmake -S test -B build/test
#   cmake --build build/test
#   ctest --test-dir build/test --output-on-failure
cmake_minimum_required(VERSION 3.16)
project(vcu_host_tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

set(CORE ${CMAKE_CURRENT_SOURCE_DIR}/../Core)

add_compile_options(-Wall -Wextra -Werror -g -O0)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CORE})

# Optional coverage: -DVCU_COVERAGE=ON
option(VCU_COVERAGE "Build host tests with gcov coverage" OFF)
if(VCU_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

function(vcu_add_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} m)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

vcu_add_test(test_linmap        test_linmap.c        ${CORE}/control/linmap.c)
vcu_add_test(test_apps          test_apps.c          ${CORE}/control/apps.c)
vcu_add_test(test_torque        test_torque.c        ${CORE}/control/torque.c)
vcu_add_test(test_state_machine test_state_machine.c ${CORE}/control/state_machine.c)
vcu_add_test(test_bppc          test_bppc.c          ${CORE}/safety/plausibility.c)
vcu_add_test(test_fault         test_fault.c         ${CORE}/safety/fault.c)
vcu_add_test(test_can_catalog   test_can_catalog.c   ${CORE}/proto/can_catalog.c)
