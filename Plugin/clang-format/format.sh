#!/bin/bash

# Get the directory of the current script
SCRIPT_DIR="$(dirname "$(realpath "$0")")"

# Debug: Print the script directory
echo "SCRIPT_DIR=$SCRIPT_DIR"

# Set the path to clang-format and the configuration file
CLANG_FORMAT_PATH="$SCRIPT_DIR/clang-format"
CONFIG_PATH="$SCRIPT_DIR/.clang-format"

# Debug: Print clang-format path
echo "CLANG_FORMAT_PATH=$CLANG_FORMAT_PATH"
echo "CONFIG_PATH=$CONFIG_PATH"

# Root directory is relative to the current directory ../../
ROOT_DIR="$(realpath "$SCRIPT_DIR/../../")"

# Debug: Print root directory
echo "ROOT_DIR=$ROOT_DIR"

# Set allowed folders, based on root directory
FOLDERS=("$ROOT_DIR/Component" "$ROOT_DIR/HAL" "$ROOT_DIR/Project" "$ROOT_DIR/Protocol" "$ROOT_DIR/Utilities")

# Add all subfolders containing "Board" under Platform
for d in "$ROOT_DIR/Platform/"*/ ; do
    if [ -d "$d/Board" ]; then
        echo "Adding $d/Board to FOLDERS"
        FOLDERS+=("$d/Board")
    fi
done

echo "Folders to be formatted: ${FOLDERS[@]}"

# Iterate over each folder and format all C and C++ files, excluding build and release folders
for d in "${FOLDERS[@]}"; do
    echo "Formatting folder: $d"
    find "$d" -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) ! -path "*/build/*" ! -path "*/release/*" | while read -r file; do
        echo "Formatting file: $file"
        "$CLANG_FORMAT_PATH" -style=file -i "$file"
        if [ $? -ne 0 ]; then
            echo "Failed to format $file"
        fi
    done
done

echo "All folders formatted"
