cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME Meteo)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)
set(${CMAKE_PROJECT_NAME}_BOARD mega)

#set(${TARGET_NAME}_PROGRAMMER avrispmkii)

set(${CMAKE_PROJECT_NAME}_SKETCH Meteo.ino)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})

