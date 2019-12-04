cmake_minimum_required(VERSION 3.15)
project(rosy)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(modules)

add_library(rosy STATIC
        modules/event/event.h
        modules/graphics/graphics.cpp
        modules/graphics/graphics.h
        modules/graphics/shader.h
        modules/input/input.h
        modules/timer/timer.cpp
        modules/timer/timer.h
        modules/window/window.cpp
        modules/window/window.h
        boot.cpp
        module.h
        property.h
        rosy.h modules/audio/audio.cpp modules/audio/audio.h)
