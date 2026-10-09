cmake_minimum_required(VERSION 3.10.0)
project(PAINRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    add_definitions(-DGLFWSTATIC)

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

    target_link_options(PAINRenderer PRIVATE
        "-static"
        "-static-libgcc"
        "-static-libstdc++"
    )

    target_include_directories(PAINRenderer PUBLIC 
        ${CMAKE_SOURCE_DIR}/lib/glfw/include 
        ${CMAKE_SOURCE_DIR}/lib/freetype/include/
        ${CMAKE_SOURCE_DIR}/lib/freetype/include/freetype2
    )

    target_link_directories(PAINRenderer PRIVATE ${CMAKE_SOURCE_DIR}/lib/glfw/lib-mingw-w64)

    target_link_libraries(PAINRenderer PRIVATE 
        "${CMAKE_SOURCE_DIR}/lib/glfw/lib-mingw-w64/libglfw3.a" 
        "${CMAKE_SOURCE_DIR}/lib/freetype/lib/libfreetype_stripped.a"
        "${CMAKE_SOURCE_DIR}/lib/freetype/lib/libz.a"
        "${CMAKE_SOURCE_DIR}/lib/freetype/lib/libbz2.a" 
        opengl32 
        gdi32 
        user32 
        kernel32 
        bcrypt
        mingw32 
        moldname
    )
else()
    find_package(glfw3 CONFIG REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(OpenGL REQUIRED)

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

    target_link_libraries(PAINRenderer PRIVATE glfw OpenGL::GL Freetype::Freetype)
endif()

target_include_directories(PAINRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

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