#!/bin/bash

# 获取当前脚本的目录
SCRIPT_DIR="$(dirname "$(realpath "$0")")"

# 设置clang-format和配置文件的路径
CLANG_FORMAT_PATH="$SCRIPT_DIR/clang-format"
CONFIG_PATH="$SCRIPT_DIR/.clang-format"

# 根目录相对于当前目录 ../../
ROOT_DIR="$(realpath "$SCRIPT_DIR/../../")"


# 设置要检查的目录
FOLDERS=("$ROOT_DIR/Component" "$ROOT_DIR/HAL" "$ROOT_DIR/Project" "$ROOT_DIR/Protocol" "$ROOT_DIR/Utilities")

# 添加所有board层代码文件夹
for d in "$ROOT_DIR/Platform/"*/ 
do
    if [ -d "$d/Board" ]; then  # 存在Board目录
        FOLDERS+=("$d/Board")  # 加到FOLDERS
    fi
done

# 遍历每个文件夹并格式化所有的C和C++文件，排除build和release文件夹
for d in "${FOLDERS[@]}"; do
    echo "正在格式化文件夹: $d"
    find "$d" -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) ! -path "*/build/*" ! -path "*/release/*" | while read -r file; do
        echo "正在格式化文件: $file"
        "$CLANG_FORMAT_PATH" -style=file -i "$file"
        if [ $? -ne 0 ]; then
            echo "格式化 $file 失败"
        fi
    done
done
