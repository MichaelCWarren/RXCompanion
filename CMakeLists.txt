cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(TXCompanion)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

include_directories(src src/Settings)

add_executable(TXCompanion
    src/main.cpp
)

add_subdirectory(src/MessagingService)
add_subdirectory(src/PPMDecoder)
add_subdirectory(src/PXXEncoder)
add_subdirectory(src/Settings)
add_subdirectory(src/SPortDecoder)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(TXCompanion pico_stdlib pico_multicore hardware_flash hardware_pio hardware_dma)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(TXCompanion)