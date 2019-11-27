cmake_minimum_required(VERSION 3.15)
project(rosy)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules)

add_library(rosy STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/event/event.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/graphics/graphics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/graphics/graphics.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/graphics/shader.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/input/input.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/timer/timer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/timer/timer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/window/window.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/window/window.h
        ${CMAKE_CURRENT_SOURCE_DIR}/boot.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/module.h
        ${CMAKE_CURRENT_SOURCE_DIR}/property.h
        ${CMAKE_CURRENT_SOURCE_DIR}/rosy.h)
