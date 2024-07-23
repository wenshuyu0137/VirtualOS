# VirtualOS 框架简介

VirtualOS 是一个基于前后台调度为核心搭建的嵌入式开发框架，适用于无操作系统的裸机平台，实现基本的异步任务调度机制。本框架开发的核心思想就是基于异步实现 由于无操作系统,因此称为VirtualOS 框架介绍:

- **Component**：包含了许多优秀的开源组件
  - FAL : RT-Thread的FLASH抽象层
  - FlashDB : 基于FAL实现的以K-V键值对方式存储的FLASH操作组件
  - LittleFs : 一个轻量级的文件系统 支持擦写均衡与掉电保护
  - RTT : Jlink官方驱动中提供的日志组件 可在串口不足时使用
- **HAL**：硬件抽象层，提供通用接口规范，供应用层调用。
- **Platform** : 维护着各种芯片厂家提供的SDK
  - Board : 基于HAL层接口规范实现的硬件操作代码
  - 其他 : 芯片厂家提供的其他文件,如启动文件等
- **Plugin**：框架使用的一些插件
  - clang-format：自动格式化代码工具,其中的pre=commit文件复制到.git/hooks下即可实现提交时自动格式化所有暂存区的.c .cpp .h .hpp文件
- **Project**：具体的项目
  - App：应用区
    - app: 所有应用层代码
    - build: 编译产生的文件
    - sys: 中断函数文件,链接文件等
    - CmakeList.txt工程配置文件
  - Boot：bootloader区 
- **Protocol**: 协议层
  - AT: AT指令的客户端协议与服务端协议
  - Modbus: 标准的ModBus协议
- **Utilities**:框架提供的一些小组件
  - button: 按键组件 支持单击,双击,长按
  - crc:CRC校验组件
  - Hash:字符串哈希Map
  - list:双向循环链表
  - queue:循环队列
  - slog 日志组件
  - soft_iic 软件IIC
  - stimer 框架核心调度组件
- **Virtual.cmake**: 框架的顶层工程配置文件,所有的项目都需要包含此文件

注：
1. 本框架的编译工程使用免费开源工具进行编译、下载和调试（如需在 Keil 上应用，请自行配置.
2. 编译环境配置流程如下（建议将所有下载资源放在一个统一文件夹下管理.

## 工具下载和配置

1. **下载 CMake 工具**  
   [CMake 下载页面](https://cmake.org/download/)  
   选择 cmake-3.30.0-windows-x86_64.msi（或最新版）下载安装，并将 bin 目录加入系统 Path 环境变量。

2. **下载 ARM 交叉编译工具链(根据需求任选其一)**  
   [ARMCLANG(AC6) 下载页面(商业付费)](https://developer.arm.com/downloads/view/ACOMPE)

   [ARMGCC 下载页面(免费)](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
   选择 arm-gnu-toolchain-13.3.rel1-mingw-w64-i686-arm-none-eabi.zip（或最新版）下载安装

3. **下载 Ninja**  
   [Ninja 下载页面](https://github.com/ninja-build/ninja/releases/v1.12.1)  
   选择 ninja-win.zip 下载解压，完成后将目录加入系统 Path 环境变量。

3. **下载 MinGW-w64**
  [MinGW-w64 下载页面](https://sourceforge.net/projects/mingw-w64/files/)

    x86_64-win32-sjlj 版本下载解压，完成后将 bin 目录加入系统 Path 环境变量。

4. **下载 J-Link 驱动**  
   [J-Link 驱动下载页面](https://www.segger.com/downloads/jlink/)  
   选择最新版 Windows 平台下的安装软件。

5. **下载 VSCode**  
   [VSCode 下载页面](https://code.visualstudio.com/)  

6. **安装 VSCode 插件**  
   在 VSCode 左侧扩展商店里分别下载以下插件：
   - C/C++（实现语法检查、代码补全、高亮等功能）
   - C/C++ Extension Pack
   - Cortex-Debug（调试代码）

确保 CMake、ARM GCC、Ninja 的 bin 目录都已经加入系统的 Path 环境变量中。

## 项目结构说明

假设你已经有了一个写好的代码以及芯片相关的SDK,为了实现代码的编译,下载,调试还需要编写相关的任务脚本


以本框架提供的案例驱动为例:

在 `Platform` 文件夹下，有一个 `ARM_CORE` 文件夹，请不要删除，里面包含了 CMSIS 提供的 ARM 内核相关的头文件（可以从 [CMSIS_5](https://github.com/ARM-software/CMSIS_5) 下载）。`GD32L23x` 文件夹是使用 GD 芯片搭建一个案例工程，其中 `Board` 文件夹为部分基于 HAL 层接口规范实现的函数。`CMSIS` 为 GD 官方 SDK 提供的文件夹，注意，其中的 .S 启动文件为 Linux 下的汇编文件语法（非官方例程提供的启动文件），具体需要从官网下载获取（[GD32 Embedded Builder](https://www.gd32mcu.com/cn/download)）。

解压后的路径为 `D:\Keil_Pro\EmbeddedBuilder_v1.4.1.23782\EmbeddedBuilder\plugins\com.gigadevice.templatefwlib.arm.gd32l23x_1.0.0.202406140948\Firmware\gcc_startup`。同样需要找到链接文件 `gd32l23x_flash.ld`。
注: 不同的芯片需要从对应的资料中下载寻找。

在 `Project` 目录下有一个具体的应用层代码案例：
- `APP` 文件夹包含一个具体案例，点亮一个 LED 灯。
- `Boot` 文件夹用于实现 Bootloader，暂未提供。
- `App` 文件夹下提供了一个 `CMakeLists.txt` 案例，已经包含了详细注释，如果确保只在app目录下新增文件则不需要修改包含部分，其中关于框架提供的组件通过 include 顶层 `Virtual.cmake` 实现

在 `CMakeLists.txt` 文件中，需要自己修改启动文件和链接文件的路径：
```cmake
# 启动文件和链接脚本（从官方 SDK 获取，注意使用 Linux 版）这两行的路径,替换为自己的路径
set(STARTUP_FILE ${FRAMEWORK_ROOT_DIR}/Platform/GD32L23X/CMSIS/GD32L23x/Source/ARM/startup_gd32l233.S)
set(LINKER_SCRIPT ${FRAMEWORK_ROOT_DIR}/Project/GD32L23x/App/sys/gd32l23x_flash.ld)
```

脚本也完成后就需要配置编译环境了，进入到CMakeLists.txt所在的目录下
在git bash中输入以下命令
``` BASH
Cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -G Ninja
```
- -B build 表示编译生成的文件存放在build文件夹下
- -S . 表示的是CMakeLists.txt文件在当前目录
- -DCMAKE_BUILD_TYPE=Debug 表示编译选项为Debug模式(不开启优化)
- -G Ninja 表示使用Ninja编译工程

为了实现避免手动输入命令编译,在顶层.vscode文件夹下新建tasks.json文件,本案例已提供一个模板
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake Build",
            "type": "shell",
            "command": "cmake",
            "args": [
                "--build",
                "."
            ],
            "options": {
                "cwd": "${workspaceFolder}/Project/GD32L23x/App/build"
            },
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": []
        }
    ]
}
```

只需要修改"cwd": "${workspaceFolder}/Project/GD32L23x/App/build"为自己的build路径就行

编译只需要点击顶部Terminal->Run Task即可编译

为了能够通过烧录器下载还需要编写一个调试文件,在.vscode文件下下新建一个launch.json的文件,本案例已提供一个模板
```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Cortex Debug",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/Project/GD32L23x/App/build/bin/Test.elf",
            "request": "launch",
            "type": "cortex-debug",
            "runToEntryPoint": "main",
            "serverpath": "D:/JLink/JLinkGDBServerCL.exe",
            "servertype": "jlink",
            "device": "GD32L233RCT6",
            "svdPath": "D:/Keil_Pro/EmbeddedBuilder_v1.4.1.23782/EmbeddedBuilder/plugins/com.gigadevice.resources_1.0.0.202406140948/svd/GD32L23x.svd",
            "liveWatch": {
                "enabled": true,
                "samplesPerSecond": 1
            }
        }
    ]
}
```

- executable为编译后生成的elf路径
- serverpath为jlink文件夹下的JLinkGDBServerCL.exe路径
- device为具体的芯片
- svdPath可选,svd文件为官方SDK提供的文件,加入这个字段可以在调试时,看到具体外设寄存器的值
- 点击左侧调试按钮中的绿色三角形即可烧录下载,打端点调试等


