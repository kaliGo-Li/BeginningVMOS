# 指定编译器
CC=gcc

# 指定编译选项
CFLAGS=-Wall -pthread

# 可执行文件名
TARGET=BeginningVM_Client

# 默认目标
all: $(TARGET)

# 目标文件依赖于源文件
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

# 清理目标
clean:
	rm -f $(TARGET)