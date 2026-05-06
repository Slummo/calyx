cmake_minimum_required(VERSION 4.0)
project(calyx VERSION 1.0.0 LANGUAGES C)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_SELF TRUE)
else()
    set(IS_SELF FALSE)
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if(IS_SELF)
    # Default to Debug build
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    endif()

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

add_subdirectory(extern)

add_library(${PROJECT_NAME} "")
target_sources(${PROJECT_NAME} PRIVATE
    src/ks_impl.c
    src/core.c
    src/physics.c
    src/render.c
)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
add_library(${PROJECT_NAME}::lib ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

if (IS_SELF)
    if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=)
    endif()

    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wsign-conversion

        # Debug flags
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-fsanitize=address,leak,undefined>
        $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
        $<$<CONFIG:Debug>:-DDEBUG>

        # Release flags
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-DNDEBUG>
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,leak,undefined>
    )
endif()

# Dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC kitsune::lib calyx::glad PRIVATE OpenGL::GL glfw)