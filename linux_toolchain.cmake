# 设置系统名称和处理器
set(CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set(CMAKE_SYSTEM_NAME "Generic" CACHE STRING "")

# 防止尝试编译目标类型
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 设置查找路径模式
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

option(USE_ARMGCC "Use ARM GCC toolchain" ON)

if(USE_ARMGCC)
    set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
    set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
    set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
    set(CMAKE_AR "arm-none-eabi-ar")
    set(CMAKE_OBJCOPY "arm-none-eabi-objcopy")
    set(CMAKE_OBJDUMP "arm-none-eabi-objdump")
    set(CMAKE_SIZE "arm-none-eabi-size")
    set(CMAKE_LINKER "arm-none-eabi-ld")
else()
    set(COMPILER_ROOT_PATH "C:/Keil_v5/ARM/ARMCLANG/bin")
    set(CMAKE_C_COMPILER "armclang")
    set(CMAKE_ASM_COMPILER "armclang")
    set(CMAKE_AR "armar")
    set(CMAKE_OBJCOPY "fromelf")
    set(CMAKE_OBJDUMP "fromelf")
    set(CMAKE_SIZE "fromelf")
    set(CMAKE_LINKER "armlink")
endif()
