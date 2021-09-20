cmake_minimum_required(VERSION 3.15)
project(rosy)

set(CMAKE_CXX_STANDARD 20)
set(LIBTYPE STATIC)
set(HAVE_LIBC TRUE)

execute_process(COMMAND python gl3w_gen.py --root "${PROJECT_BINARY_DIR}/ThirdParty/gl3w" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/gl3w")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glslang)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/fmt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/gl3w)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/openal-soft)

target_compile_options(fmt PUBLIC /EHsc)

add_executable(rosy-rc src/rc.cpp)

function(rosy_add_resources project)
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
            VERBATIM
        )
        list(APPEND result ${output})
    endforeach()

    add_custom_command(
        OUTPUT "${output_directory}/R.cpp" "${output_directory}/R.h"
        COMMAND rosy-rc ${output_directory} ${directory} ${ARGN}
        DEPENDS rosy-rc ${ARGN}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    add_library("${project}.rc" STATIC "${output_directory}/R.cpp" "${output_directory}/R.h" ${result})
    target_link_libraries(${project} PUBLIC "${project}.rc")
endfunction()

add_library(rosy STATIC
    src/modules/event/event.h
    src/modules/graphics/graphics.cpp
    src/modules/graphics/graphics.h
    src/modules/graphics/shader.h
    src/modules/input/input.h
    src/modules/timer/timer.cpp
    src/modules/timer/timer.h
    src/modules/window/window.cpp
    src/modules/window/window.h
    src/modules/audio/audio.cpp
    src/modules/audio/audio.h
    src/modules/resources/resources.cpp
    src/modules/resources/resources.h
    src/boot.cpp
    src/module.h
    src/property.h
    src/rosy.h
)

target_compile_definitions(rosy PUBLIC
    -DGLFW_INCLUDE_NONE
    -DIMGUI_DEFINE_MATH_OPERATORS
    -DGLM_FORCE_XYZW_ONLY
    -DNOMINMAX
    -DSDL_MAIN_HANDLED
)

target_link_libraries(rosy PUBLIC
    glm
    fmt
    SDL2-static
    gl3w
    OpenAL
)

target_include_directories(rosy PUBLIC src)
target_include_directories(rosy PUBLIC
    ThirdParty/SDL/include
    ThirdParty/gl3w
)

add_executable(example examples/main.cpp examples/transform.h examples/camera.h examples/camera.cpp)
target_link_libraries(example PRIVATE rosy)