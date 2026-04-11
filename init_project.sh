#!/bin/bash

# 1. 建立目錄結構
mkdir -p src include build tests .vscode

# 2. 建立基本的檔案
touch src/main.c
touch src/cpu.c
touch include/cpu.h
touch include/common.h
touch Makefile

# 3. 寫入一個簡單的 main.c 範例
cat <<EOF > src/main.c
#include <stdio.h>
#include "cpu.h"

int main() {
    printf("RISC-V Emulator Initializing...\\n");
    return 0;
}
EOF

# 4. 配置 VS Code 編譯任務 (tasks.json)
# 這裡預設使用 gcc，請確保你已安裝 MinGW 或相關工具鏈
cat <<EOF > .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: gcc build active file",
            "command": "gcc",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "-I\${workspaceFolder}/include",
                "\${workspaceFolder}/src/*.c",
                "-o",
                "\${workspaceFolder}/build/emulator.exe"
            ],
            "options": {
                "cwd": "\${workspaceFolder}"
            },
            "problemMatcher": [
                "\$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "編譯當前所有 src 檔案並輸出至 build 目錄"
        }
    ]
}
EOF

# 5. 配置 VS Code 偵錯工具 (launch.json)
cat <<EOF > .vscode/launch.json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Emulator",
            "type": "cppdbg",
            "request": "launch",
            "program": "\${workspaceFolder}/build/emulator.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "\${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: gcc build active file"
        }
    ]
}
EOF

echo "專案結構與 VS Code 配置已完成！"