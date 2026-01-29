cmake_minimum_required(VERSION 3.16)
project(renderer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    include/GL/gl3w.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/GL
    ${CMAKE_SOURCE_DIR}/include/GLFW
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/KHR
    ${CMAKE_SOURCE_DIR}/src
)

if(WIN32)
    set(GLFW_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/glfw-3.4.bin.WIN64)
    
    if(MSVC)
        target_link_directories(${PROJECT_NAME} PRIVATE ${GLFW_LIB_DIR}/lib-vc2022)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw3 opengl32)
    elseif(MINGW)
        target_link_directories(${PROJECT_NAME} PRIVATE ${GLFW_LIB_DIR}/lib-mingw-w64)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw3 opengl32)
    endif()
else()
    find_package(glfw3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    
    if(APPLE)
        find_library(OPENGL_LIBRARY OpenGL REQUIRED)
        find_library(COCOA_LIBRARY Cocoa REQUIRED)
        find_library(IOKIT_LIBRARY IOKit REQUIRED)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${OPENGL_LIBRARY}
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
        )
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE GL dl)
    endif()
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/objs
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/objs
)