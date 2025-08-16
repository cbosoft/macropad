cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(deps/pico_sdk_import.cmake)

project(pkb)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(main
    src/main.c
    src/usb_descriptors.c
)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(main PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/deps)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(main pico_stdlib pico_unique_id tinyusb_device tinyusb_board)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(main)
