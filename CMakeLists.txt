cmake_minimum_required(VERSION 3.10.0)
project(PAINRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PAINRenderer 
    main.cpp
    glad.c
    shader.cpp
    vbo.cpp
    ebo.cpp
    vao.cpp
    camera.cpp
    mesh.cpp
    text_element.cpp
    ui_element.cpp
    element.cpp
    obj_mesh.cpp
    obj_loader.cpp
    button_element.cpp
)

add_custom_command(
    TARGET PAINRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
            "$<TARGET_FILE_DIR:PAINRenderer>/shaders"   
)

add_custom_command(
    TARGET PAINRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
            "$<TARGET_FILE_DIR:PAINRenderer>/fonts"   
)

add_custom_command(
    TARGET PAINRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/models"
            "$<TARGET_FILE_DIR:PAINRenderer>/models"   
)

target_include_directories(PAINRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(GLFW_DIR "${CMAKE_SOURCE_DIR}/lib/glfw")

    add_library(glfw_windows STATIC IMPORTED GLOBAL)

    set_target_properties(glfw_windows PROPERTIES
        IMPORTED_LOCATION "${GLFW_DIR}/lib-mingw-w64/libglfw3.a"
        INTERFACE_INCLUDE_DIRECTORIES "${GLFW_DIR}/include"    
    )

    set(FREETYPE_DIR "${CMAKE_SOURCE_DIR}/lib/freetype")

    add_library(freetype_windows STATIC IMPORTED GLOBAL)

    set_target_properties(freetype_windows PROPERTIES
        IMPORTED_LOCATION "${FREETYPE_DIR}/lib/libfreetype.a"
        INTERFACE_INCLUDE_DIRECTORIES "${FREETYPE_DIR}/include/freetype2"    
    )

    target_link_libraries(PAINRenderer PRIVATE glfw_windows freetype_windows opengl32 gdi32)
else()
    find_package(glfw3 CONFIG REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(PAINRenderer PRIVATE glfw OpenGL::GL Freetype::Freetype)
endif()