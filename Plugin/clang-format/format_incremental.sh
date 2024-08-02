#!/bin/bash

# 获取当前脚本的目录
SCRIPT_DIR=$(dirname "$(realpath "$0")")

# 设置clang-format和配置文件的路径
CLANG_FORMAT_PATH="$SCRIPT_DIR/clang-format"
CONFIG_PATH="$SCRIPT_DIR/.clang-format"

# 根目录相对于当前目录 ../../
ROOT_DIR=$(realpath "$SCRIPT_DIR/../../")

# 设置允许的文件夹，基于根目录
FOLDERS=("$ROOT_DIR/Component" "$ROOT_DIR/DAL" "$ROOT_DIR/DML" "$ROOT_DIR/Project" "$ROOT_DIR/Protocol" "$ROOT_DIR/Utilities")

# 添加Platform目录下所有包含"driver"的子文件夹
for d in "$ROOT_DIR/Driver/"*/ ; do
    if [ -d "$d/driver" ]; then
        FOLDERS+=("$d/driver")
    fi
done

# 获取暂存的文件列表
CHANGED_FILES=($(git diff --name-only --cached --diff-filter=ACM | grep -E "\.(cpp|hpp|c|h)$"))

if [ ${#CHANGED_FILES[@]} -eq 0 ]; then
    exit 0
fi

# 遍历每个暂存的文件，并在必要时格式化
for FILE_PATH in "${CHANGED_FILES[@]}"; do
    FILE_PATH_ABS="$ROOT_DIR/$FILE_PATH"

    # 检查文件是否在允许的文件夹中
    FORMAT_FILE=0
    for FOLDER in "${FOLDERS[@]}"; do
        if [[ "$FILE_PATH_ABS" == "$FOLDER"* ]]; then
            FORMAT_FILE=1
            break
        fi
    done

    # 如果文件在允许的文件夹中且不在build或release目录中，则格式化文件
    if [ $FORMAT_FILE -eq 1 ] && [[ "$FILE_PATH" != *"build/"* ]] && [[ "$FILE_PATH" != *"release/"* ]]; then
        echo "格式化文件: $FILE_PATH"
        "$CLANG_FORMAT_PATH" -style=file -i "$FILE_PATH"
        if [ $? -ne 0 ]; then
            echo "格式化 $FILE_PATH 失败"
        fi
    fi
done

echo "增量格式化完成。"
