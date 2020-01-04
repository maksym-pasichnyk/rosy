cmake_minimum_required(VERSION 3.15)
project(rosy)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glslang)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules)

add_executable(rc rc.cpp)

function(add_resources project directory)
    list(REMOVE_DUPLICATES ARGN)
    set(output_directory "${CMAKE_CURRENT_SOURCE_DIR}/.generated")

    set(result)
    foreach(input ${ARGN})
        file(RELATIVE_PATH source ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${input})
        string(MAKE_C_IDENTIFIER ${input} input_identifier)
        set(output "${output_directory}/${input_identifier}.o")

        add_custom_command(OUTPUT ${output}
                COMMAND ld -r -b binary -o ${output} ${source}
                DEPENDS ${input}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                COMMENT "Building resource object ${output}"
                VERBATIM)
        list(APPEND result ${output})
    endforeach()

    add_custom_command(
            OUTPUT "${output_directory}/R.cpp" "${output_directory}/R.h"
            COMMAND rc ${output_directory} ${directory} ${ARGN}
            DEPENDS rc ${ARGN}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    add_library("${project}.rc" STATIC "${output_directory}/R.cpp" "${output_directory}/R.h" ${result})
    target_link_libraries(${project} "${project}.rc")
endfunction()

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
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/audio/audio.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/audio/audio.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/resources/resources.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/resources/resources.h
        ${CMAKE_CURRENT_SOURCE_DIR}/boot.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/module.h
        ${CMAKE_CURRENT_SOURCE_DIR}/property.h
        ${CMAKE_CURRENT_SOURCE_DIR}/rosy.h)

