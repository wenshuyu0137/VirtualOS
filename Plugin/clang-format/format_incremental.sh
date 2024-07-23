#!/bin/bash

# Get the directory of the current script
SCRIPT_DIR=$(dirname "$(realpath "$0")")

# Set the path to clang-format and the configuration file
CLANG_FORMAT_PATH="$SCRIPT_DIR/clang-format"
CONFIG_PATH="$SCRIPT_DIR/.clang-format"

# Root directory is relative to the current directory ../../
ROOT_DIR=$(realpath "$SCRIPT_DIR/../../")


# Set allowed folders, based on root directory
FOLDERS=("$ROOT_DIR/Component" "$ROOT_DIR/HAL" "$ROOT_DIR/Project" "$ROOT_DIR/Protocol" "$ROOT_DIR/Utilities")

# Add all subfolders containing "Board" under Platform
for d in "$ROOT_DIR/Platform/"*/ ; do
    if [ -d "$d/Board" ]; then
        FOLDERS+=("$d/Board")
    fi
done

# Get the list of staged files
CHANGED_FILES=($(git diff --name-only --cached --diff-filter=ACM | grep -E "\.(cpp|hpp|c|h)$"))

if [ ${#CHANGED_FILES[@]} -eq 0 ]; then
    exit 0
fi

# Iterate over each staged file and format if necessary
for FILE_PATH in "${CHANGED_FILES[@]}"; do
    FILE_PATH_ABS="$ROOT_DIR/$FILE_PATH"

    # Check if the file is in allowed folders
    FORMAT_FILE=0
    for FOLDER in "${FOLDERS[@]}"; do
        if [[ "$FILE_PATH_ABS" == "$FOLDER"* ]]; then
            FORMAT_FILE=1
            break
        fi
    done

    # Format the file if it is in an allowed folder and not in build or release directories
    if [ $FORMAT_FILE -eq 1 ] && [[ "$FILE_PATH" != *"build/"* ]] && [[ "$FILE_PATH" != *"release/"* ]]; then
        echo "Formatting file: $FILE_PATH"
        "$CLANG_FORMAT_PATH" -style=file -i "$FILE_PATH"
        if [ $? -ne 0 ]; then
            echo "Failed to format $FILE_PATH"
        fi
    fi
done

echo "Incremental formatting complete."
