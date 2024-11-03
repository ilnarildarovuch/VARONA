# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -I./include -I. -fPIC -pthread -O3 -s -fvisibility=hidden

# Библиотеки
LIBS = -lssh

# Директории
SRC_DIR = .
BRUTEFORCE_DIR = bruteforce
BRUTEFORCE_FUNCTIONS_DIR = bruteforce_functions
OBJ_DIR = obj
BIN_DIR = bin

# Исходные файлы
SRCS = main.c include/credentials.c find_ips/finder.c socks5/proxy.c

# Объектные файлы
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Создание подкаталогов для объектных файлов
OBJ_DIRS = $(OBJ_DIR) $(OBJ_DIR)/include $(OBJ_DIR)/socks5 $(OBJ_DIR)/find_ips

# Имя исполняемого файла
TARGET = $(BIN_DIR)/bruteforce

# Правило по умолчанию
all: directories $(TARGET)

# Создание директорий
directories:
	@mkdir -p $(OBJ_DIRS)
	@mkdir -p $(BIN_DIR)

# Компиляция программы
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Пересборка
rebuild: clean all

.PHONY: all clean rebuild directories