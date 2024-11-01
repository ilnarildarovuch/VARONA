# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -Wextra -I./include -I.

# Библиотеки
LIBS = -lssh

# Директории
SRC_DIR = .
BRUTEFORCE_DIR = bruteforce
BRUTEFORCE_FUNCTIONS_DIR = bruteforce_functions
OBJ_DIR = obj
BIN_DIR = bin

# Исходные файлы
SRCS = main.c \
       $(BRUTEFORCE_DIR)/ssh_bruteforce.c \
       $(BRUTEFORCE_DIR)/telnet_bruteforce.c \
       $(BRUTEFORCE_FUNCTIONS_DIR)/passwords.c \
       $(BRUTEFORCE_FUNCTIONS_DIR)/passwords.c \
       $(BRUTEFORCE_FUNCTIONS_DIR)/wait_for_prompt.c \
       $(BRUTEFORCE_FUNCTIONS_DIR)/socket_things.c \
       $(BRUTEFORCE_FUNCTIONS_DIR)/login_things.c

# Объектные файлы
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Создание подкаталогов для объектных файлов
OBJ_DIRS = $(OBJ_DIR) \
           $(OBJ_DIR)/$(BRUTEFORCE_DIR) \
           $(OBJ_DIR)/$(BRUTEFORCE_FUNCTIONS_DIR)

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
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

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