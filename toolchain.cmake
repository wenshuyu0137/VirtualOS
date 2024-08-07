# 设置系统名称和处理器
set(CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set(CMAKE_SYSTEM_NAME "Generic" CACHE STRING "")

# 防止尝试编译目标类型
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 设置查找路径模式
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(COMPILER_ROOT_PATH "D:/Tool/compile_environment/gcc-arm-none-eabi-10.3-2021.10/bin")

# set(COMPILER_ROOT_PATH "C:/compile_environment/arm-gnu-toolchain-13.3.rel1-mingw-w64-i686-arm-none-eabi/bin")
set(CMAKE_C_COMPILER "${COMPILER_ROOT_PATH}/arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "${COMPILER_ROOT_PATH}/arm-none-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "${COMPILER_ROOT_PATH}/arm-none-eabi-gcc.exe")
set(CMAKE_AR "${COMPILER_ROOT_PATH}/arm-none-eabi-ar.exe")
set(CMAKE_OBJCOPY "${COMPILER_ROOT_PATH}/arm-none-eabi-objcopy.exe")
set(CMAKE_OBJDUMP "${COMPILER_ROOT_PATH}/arm-none-eabi-objdump.exe")
set(CMAKE_SIZE "${COMPILER_ROOT_PATH}/arm-none-eabi-size.exe")
set(CMAKE_LINKER "${COMPILER_ROOT_PATH}/arm-none-eabi-ld.exe")
