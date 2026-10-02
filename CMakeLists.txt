cmake_minimum_required(VERSION 3.10.0)
project(PAINRenderer VERSION 0.1.0 LANGUAGES C CXX)

find_package(glfw3 REQUIRED)

find_package(OpenGL REQUIRED)

add_executable(PAINRenderer 
    main.cpp
    glad.c
    shader.cpp
    vbo.cpp
    ebo.cpp
    vao.cpp
)

add_custom_command(
    TARGET PAINRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.vert"
            "$<TARGET_FILE_DIR:PAINRenderer>/shaders/test.vert"   
)

add_custom_command(
    TARGET PAINRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.frag"
            "$<TARGET_FILE_DIR:PAINRenderer>/shaders/test.frag"   
)

target_include_directories(PAINRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(PAINRenderer PRIVATE glfw OpenGL::GL)