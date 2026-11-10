cmake_minimum_required(VERSION 3.10.0)
project(PAINRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASSET_PATHS 
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/elementDefault.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/elementDefault.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/objModel.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/objModel.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/text.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/text.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts/default.ttf"
)

set(ASSET_VARS
    "elementDefaultFrag"
    "elementDefaultVert"
    "objModelFrag"
    "objModelVert"
    "textFrag"
    "textVert"
    "defaultFont"
)

set(GENERATED_SOURCES "")

foreach(ASSET_PATH ASSET_VAR IN ZIP_LISTS ASSET_PATHS ASSET_VARS)
    get_filename_component(FILE_NAME "${ASSET_PATH}" NAME_WE)
    set(OUT_CPP "${CMAKE_CURRENT_BINARY_DIR}/embedded_${FILE_NAME}_${ASSET_VAR}.cpp")

    add_custom_command(
        OUTPUT "${OUT_CPP}"
        COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/embed_file.py"
            "${ASSET_PATH}"
            "${OUT_CPP}"
            "${ASSET_VAR}"
        DEPENDS "${ASSET_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/embed_file.py"
        COMMENT "Baking asset via Python: ${FILE_NAME} -> ${ASSET_VAR}"
    )

    list(APPEND GENERATED_SOURCES "${OUT_CPP}")
endforeach()


if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" AND ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared library" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")

    add_definitions(-DGLFWSTATIC)

    add_executable(PAINRenderer 
        ${GENERATED_SOURCES}
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
        3mf_loader.cpp
    )

    set_target_properties(PAINRenderer PROPERTIES ENABLE_EXPORTS TRUE)

    target_include_directories(PAINRenderer PUBLIC 
        ${CMAKE_SOURCE_DIR}/lib/glfw/include 
        ${CMAKE_SOURCE_DIR}/lib/freetype/include/
        ${CMAKE_SOURCE_DIR}/lib/freetype/include/freetype2
        ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs
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
        "${CMAKE_SOURCE_DIR}/lib/lib3mf/lib/lib3mf.a"
        "${CMAKE_SOURCE_DIR}/lib/lib3mf/lib/libzip.a"
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
        shlwapi
        rpcrt4
        uuid
    )

    target_compile_definitions(PAINRenderer PRIVATE LIB3MF_STATIC ZIP_STATIC)  
else()
    find_package(glfw3 CONFIG REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(OpenGL REQUIRED)

    add_library(lib3mf::lib3mf UNKNOWN IMPORTED)
    set_target_properties(lib3mf::lib3mf PROPERTIES
        IMPORTED_LOCATION "/lib/x86_64-linux-gnu/lib3MF.so"
    )

    add_executable(PAINRenderer 
        ${GENERATED_SOURCES}
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
        3mf_loader.cpp
    )

    add_library(tinyfiledialogs STATIC
        ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs/tinyfiledialogs.c
        ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs/tinyfiledialogs.h
    )

    target_link_libraries(PAINRenderer PRIVATE glfw OpenGL::GL Freetype::Freetype tinyfiledialogs lib3mf::lib3mf)
endif()

target_include_directories(PAINRenderer PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/lib/tinyfiledialogs 
    ${CMAKE_SOURCE_DIR}/lib/lib3mf/include/
)

add_custom_command(
    TARGET PAINRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/models"
            "$<TARGET_FILE_DIR:PAINRenderer>/models"   
)