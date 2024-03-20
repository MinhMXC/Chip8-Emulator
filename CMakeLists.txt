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
        src/chip8.h
        src/chip8.cpp
        src/constants.h
        src/display.h
        src/SDLDisplay.h
        src/input_handler.h
        src/input_handler.cpp
        src/timer.h
        src/sound_player.h
)

add_executable(tests
        tests/test.cpp
        src/chip8.h
        src/chip8.cpp
        src/constants.h
        src/display.h
        src/input_handler.h
        src/input_handler.cpp
)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} Catch2::Catch2WithMain)
target_link_libraries(tests Catch2::Catch2WithMain)