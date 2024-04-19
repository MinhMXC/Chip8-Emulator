cmake_minimum_required(VERSION 3.27)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

SET(SDL2_PATH "C:/Users/MinhNguyen/CLionProjects/chip8-emulator/SDL")
SET(Catch2_PATH "C:/Users/MinhNguyen/CLionProjects/chip8-emulator/Catch2")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(Catch2 REQUIRED)

add_executable(chip8_emulator src/main.cpp
        src/emulators/chip8.h
        src/emulators/chip8.cpp
        src/constants.h
        src/displays/simple_display.h
        src/extras/input_handler.h
        src/extras/input_handler.cpp
        src/extras/oscillator.h
        src/emulators/emulator.h
        src/emulators/schip.h
        src/emulators/schip.cpp
        src/displays/advanced_display.h
        src/displays/simple_sdl_display.h
        src/displays/advanced_sdl_display.h
)

add_executable(chip8_test
        tests/chip8_test.cpp
        src/emulators/chip8.h
        src/emulators/chip8.cpp
        src/constants.h
        src/extras/input_handler.h
        src/extras/input_handler.cpp
)

add_executable(schip_test
        tests/schip_test.cpp
        src/emulators/schip.h
        src/emulators/schip.cpp
        src/constants.h
        src/extras/input_handler.h
        src/extras/input_handler.cpp
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
target_link_libraries(chip8_test Catch2::Catch2WithMain)
target_link_libraries(schip_test Catch2::Catch2WithMain)