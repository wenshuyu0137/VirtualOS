# CMake的最低版本
cmake_minimum_required(VERSION 3.21)

# 设置系统名称和版本
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# C和C++的标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 如果没有指定构建类型，则默认为Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# 根据构建类型设置编译选项
string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_UPPER)

if(BUILD_TYPE_UPPER STREQUAL "DEBUG")
    add_compile_options(-Og -g)
else()
    add_compile_options(-Os)
endif()

# 框架头文件路径
set(FRAMEWORK_INCLUDE_DIRS
    ${FRAMEWORK_ROOT_DIR}/Driver/ARM_CORE/
    ${FRAMEWORK_ROOT_DIR}/Component/FAL/
    ${FRAMEWORK_ROOT_DIR}/Component/FlashDB/
    ${FRAMEWORK_ROOT_DIR}/Component/LittleFs/
    ${FRAMEWORK_ROOT_DIR}/Component/RTT/
    ${FRAMEWORK_ROOT_DIR}/DAL/
    ${FRAMEWORK_ROOT_DIR}/DML/
    ${FRAMEWORK_ROOT_DIR}/HAL/
    ${FRAMEWORK_ROOT_DIR}/Utilities/button/
    ${FRAMEWORK_ROOT_DIR}/Utilities/hash/string_hash/
    ${FRAMEWORK_ROOT_DIR}/Utilities/crc/
    ${FRAMEWORK_ROOT_DIR}/Utilities/list/
    ${FRAMEWORK_ROOT_DIR}/Utilities/qfsm/
    ${FRAMEWORK_ROOT_DIR}/Utilities/queue/
    ${FRAMEWORK_ROOT_DIR}/Utilities/slog/
    ${FRAMEWORK_ROOT_DIR}/Utilities/soft_iic/
    ${FRAMEWORK_ROOT_DIR}/Utilities/stimer/
    ${FRAMEWORK_ROOT_DIR}/Protocol/ModBUS/
    ${FRAMEWORK_ROOT_DIR}/Protocol/Protocol_4G/
    ${FRAMEWORK_ROOT_DIR}/Protocol/AT/at_client/
    ${FRAMEWORK_ROOT_DIR}/Protocol/AT/at_server/
    ${FRAMEWORK_ROOT_DIR}/Driver/syscall/
)

# 框架源文件
file(GLOB_RECURSE BASE_SOURCES
    # ${FRAMEWORK_ROOT_DIR}/Component/FAL/*.c
    # ${FRAMEWORK_ROOT_DIR}/Component/FlashDB/*.c
    ${FRAMEWORK_ROOT_DIR}/Component/LittleFs/*.c
    ${FRAMEWORK_ROOT_DIR}/Component/RTT/*.c
    ${FRAMEWORK_ROOT_DIR}/DAL/*.c
    ${FRAMEWORK_ROOT_DIR}/DML/*.c
    ${FRAMEWORK_ROOT_DIR}/HAL/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/button/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/hash/string_hash/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/crc/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/list/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/qfsm/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/queue/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/slog/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/soft_iic/*.c
    ${FRAMEWORK_ROOT_DIR}/Utilities/stimer/*.c
    ${FRAMEWORK_ROOT_DIR}/Protocol/ModBUS/*.c
    ${FRAMEWORK_ROOT_DIR}/Protocol/Protocol_4G/*.c
    ${FRAMEWORK_ROOT_DIR}/Protocol/AT/at_client/*.c
    ${FRAMEWORK_ROOT_DIR}/Protocol/AT/at_server/*.c
)

# 导入交叉编译工具链
set(CMAKE_TOOLCHAIN_FILE ${FRAMEWORK_ROOT_DIR}/toolchain.cmake)

# 将所有源文件添加到目标中
if(USE_ARMGCC)
    file(GLOB_RECURSE SHARED_SOURCES ${BASE_SOURCES} ${FRAMEWORK_ROOT_DIR}/Driver/syscall/syscall.c)
else()
    file(GLOB_RECURSE SHARED_SOURCES ${BASE_SOURCES})
endif()

# 包含目录
include_directories(${FRAMEWORK_INCLUDE_DIRS})
