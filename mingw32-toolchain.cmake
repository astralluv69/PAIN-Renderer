# Identify the target operating system
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the cross-compilers (adjust names if using 32-bit: i686-w64-mingw32-gcc)
set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
set(CMAKE_RC_COMPILER i686-w64-mingw32-windres)

# Define where the target environment roots are located
# (Common default path for Ubuntu/Debian MinGW packages)
set(CMAKE_FIND_ROOT_PATH /usr/i686-w64-mingw32)

# Adjust the find program behavior
# Search for host tools (like bison, flex, glslc) on the Linux host
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for headers, libraries, and packages ONLY in the target MinGW root
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)