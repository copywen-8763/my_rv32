# 編譯器設定
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# 目錄設定
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = build

# 搜尋所有的 .c 檔案並轉換成 .o 檔名
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# 執行檔名稱
TARGET = $(BIN_DIR)/emulator

# 預設動作
all: $(TARGET)

# 連結所有 .o 檔案產生執行檔
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET)
	@echo "Build successful: $(TARGET)"

# 編譯每個 .c 檔案成 .o 檔案
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 清除編譯產生的檔案
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)
	@echo "Cleaned up build files."

# 執行程式
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run