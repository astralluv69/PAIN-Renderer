cmake_minimum_required(VERSION 3.10.0)
project(PAINRenderer VERSION 0.1.0 LANGUAGES C CXX)

find_package(glfw3 REQUIRED)

find_package(OpenGL REQUIRED)

add_executable(PAINRenderer main.cpp)

target_include_directories(PAINRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(PAINRenderer PRIVATE glfw OpenGL::GL)