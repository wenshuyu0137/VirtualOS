# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.20)

# 设置 CMake 策略 CMP0123 的行为为 NEW
cmake_policy(SET CMP0123 NEW)

# 设置内核
set(CPU_CORE cortex-m23)
set(CMAKE_SYSTEM_PROCESSOR ${CPU_CORE})

# 导入VirtualOS框架
set(FRAMEWORK_ROOT_DIR ${CMAKE_SOURCE_DIR}/../../..)
include(${FRAMEWORK_ROOT_DIR}/Virtual.cmake)

# 设置项目名称为 Test，支持 C 和 ASM 源文件
project(Test C ASM)

# 设置可执行文件和库文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)

# 设置项目头文件路径
set(PROJECT_INCLUDE_DIRS
    ${FRAMEWORK_ROOT_DIR}/Project/GD32L23x/App/app/
    ${FRAMEWORK_ROOT_DIR}/Project/GD32L23x/App/sys/
    ${FRAMEWORK_ROOT_DIR}/Driver/GD32L23X/GD32L23x_standard_peripheral/Include/
    ${FRAMEWORK_ROOT_DIR}/Driver/GD32L23X/CMSIS/GD32L23x/Include/
    ${FRAMEWORK_ROOT_DIR}/Driver/GD32L23X/driver/
)
include_directories(${PROJECT_INCLUDE_DIRS})

# 设置项目源文件路径
file(GLOB_RECURSE SUBPROJECT_SOURCES
    ${FRAMEWORK_ROOT_DIR}/Project/GD32L23x/App/app/*.c
    ${FRAMEWORK_ROOT_DIR}/Project/GD32L23x/App/sys/*.c
    ${FRAMEWORK_ROOT_DIR}/Driver/GD32L23X/GD32L23x_standard_peripheral/Source/*.c
    ${FRAMEWORK_ROOT_DIR}/Driver/GD32L23X/CMSIS/GD32L23x/Source/*.c
    ${FRAMEWORK_ROOT_DIR}/Driver/GD32L23X/driver/*.c
)

if(USE_ARMGCC) # ARM GCC 编译器
    # ARM GCC 编译器的启动文件和链接脚本
    set(STARTUP_FILE ${FRAMEWORK_ROOT_DIR}/Driver/GD32L23X/CMSIS/GD32L23x/Source/ARM/startup_gd32l233.S)
    set(LINKER_SCRIPT ${FRAMEWORK_ROOT_DIR}/Project/GD32L23x/App/sys/gd32l23x_flash.ld)

    # ARM GCC 编译器的编译选项
    set(COMPILER_FLAGS
        -mcpu=${CPU_CORE} # 指定 CPU 类型
        -mthumb # 使用 Thumb 指令集
        -mthumb-interwork # 启用 Thumb 指令集和 ARM 指令集之间的互操作性
        -ffunction-sections # 将每个函数放到独立的节中
        -fdata-sections # 将每个数据放到独立的节中
        -fno-common # 禁止使用公共符号
        -fmessage-length=0 # 禁止编译器截断消息长度
    )

    # 设置链接选项
    add_link_options(
        -Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map # 生成链接映射文件
        -Wl,--print-memory-usage # 打印内存使用情况
        -Wl,--gc-sections # 垃圾回收无用的节
        -mcpu=${CPU_CORE} # 指定 CPU 类型
        -mthumb # 使用 Thumb 指令集
        -mthumb-interwork # 启用 Thumb 和 ARM 指令集互操作性
        -T ${LINKER_SCRIPT} # 指定链接脚本
    )

    # 添加可执行文件
    add_executable(${PROJECT_NAME} ${SHARED_SOURCES} ${SUBPROJECT_SOURCES} ${STARTUP_FILE})

    # 设置编译选项
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILER_FLAGS})

else() # ARM CLANG 编译器
    # ARM CLANG 编译器的启动文件和链接脚本
    set(STARTUP_FILE ${FRAMEWORK_ROOT_DIR}/Driver/GD32L23X/CMSIS/GD32L23x/Source/ARM/startup_gd32l233xx.s)
    set(LINKER_SCRIPT ${FRAMEWORK_ROOT_DIR}/Project/GD32L23x/App/sys/gd32l23x_flash.sct)

    # ARM CLANG 编译器的编译选项
    set(COMPILER_FLAGS
        --target=arm-arm-none-eabi
        -mcpu=${CPU_CORE}
        -masm=auto
        -Wa,armasm,--diag_suppress=A1950W
        -gdwarf-4
        -std=gnu11
        -c
        -fno-rtti
        # -funsigned-char
        # -fshort-enums
        # -fshort-wchar
        -ffunction-sections
    )

    # 设置链接选项
    add_link_options(
        --cpu=${CPU_CORE}
        --strict
        --scatter ${LINKER_SCRIPT}
        --info sizes,totals,unused,veneers
        --xref --callgraph --symbols
        --map
        --list ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.map

        --summary_stderr
        # --info summarysizes
        --load_addr_map_info
    )

    # 添加可执行文件
    add_executable(${PROJECT_NAME} ${SHARED_SOURCES} ${SUBPROJECT_SOURCES} ${STARTUP_FILE})

    # 设置编译选项
    target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILER_FLAGS})
endif()

# 设置生成的 HEX 和 BIN 文件路径
set(HEX_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.hex)
set(BIN_FILE ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.bin)

# 修改文件名的脚本路径
set(CONVERT_BIN ${CMAKE_SOURCE_DIR}/convert_hex_to_bin.exe)
set(VENDOR_ID 10973)
set(HW 10)
set(PID 149)
set(SW 20)

# 自定义命令生成 HEX 和 BIN 文件
if(USE_ARMGCC)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${HEX_FILE} # 生成 HEX 文件
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE} # 生成 BIN 文件
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}> # 打印 ELF 文件大小
        COMMAND ${CMAKE_OBJDUMP} -h $<TARGET_FILE:${PROJECT_NAME}> > ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.sections # 打印 ELF 文件节信息

        # COMMAND ${CONVERT_BIN} ${VENDOR_ID} ${HW} ${PID} ${SW} ${BIN_FILE}              # 修改BIN文件名
    )
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} --bin --output=${BIN_FILE} $<TARGET_FILE:${PROJECT_NAME}> # 生成 BIN 文件
        COMMAND ${CMAKE_OBJCOPY} --i32 --output=${HEX_FILE} $<TARGET_FILE:${PROJECT_NAME}> # 生成 HEX 文件
        COMMAND ${CMAKE_SIZE} --info=sizes $<TARGET_FILE:${PROJECT_NAME}> # 打印目标文件大小

        # COMMAND ${CONVERT_BIN} ${VENDOR_ID} ${HW} ${PID} ${SW} ${BIN_FILE}              # 修改BIN文件名
    )
endif()
