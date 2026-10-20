cmake_minimum_required(VERSION 3.10.0)
project(PAINRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" AND ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

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
        stl_loader.cpp
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
        ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs
        ${CMAKE_SOURCE_DIR}/lib/lib3mf/include
    )

    target_link_directories(PAINRenderer PRIVATE ${CMAKE_SOURCE_DIR}/lib/glfw/lib-mingw-w64)

    add_library(tinyfiledialogs STATIC
        ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs/tinyfiledialogs.c
        ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs/tinyfiledialogs.h
    )

    target_link_libraries(PAINRenderer PRIVATE 
        "${CMAKE_SOURCE_DIR}/lib/glfw/lib-mingw-w64/libglfw3.a" 
        "${CMAKE_SOURCE_DIR}/lib/freetype/lib/libfreetype_stripped.a"
        "${CMAKE_SOURCE_DIR}/lib/freetype/lib/libz.a"
        "${CMAKE_SOURCE_DIR}/lib/freetype/lib/libbz2.a" 
        "${CMAKE_SOURCE_DIR}/lib/lib3mf/lib/lib3mf.dll.a"
        tinyfiledialogs
        opengl32 
        gdi32 
        user32 
        kernel32 
        bcrypt
        mingw32 
        moldname
        comdlg32
        ole32
        shell32
    )
else()
    find_package(glfw3 CONFIG REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(lib3mf REQUIRED)

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
        stl_loader.cpp
    )

    add_library(tinyfiledialogs STATIC
        ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs/tinyfiledialogs.c
        ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs/tinyfiledialogs.h
    )

    target_link_libraries(PAINRenderer PRIVATE glfw OpenGL::GL Freetype::Freetype tinyfiledialogs lib3mf::lib3mf)

    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        target_link_libraries(PAINRenderer PRIVATE comdlg32 ole32 shell32)
    endif()
endif()



target_include_directories(PAINRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs)

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