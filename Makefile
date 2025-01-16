# 构建配置
PROJECT_NAME = cremu
SRCS_DIR = src
HEADS_DIR = include
SRCS = $(shell find $(SRCS_DIR) -name '*.c')
HEADS = $(shell find $(HEADS_DIR) -name '*.h')

SRCS += test/main.c
#SRCS += test/difftest.c

CC = gcc
CFLAGS = -Wall -Werror -pedantic -O2 -I$(HEADS_DIR)
LDFLAGS =

IMG  ?= 
ARGS ?= 

# 构建命令
OBJS = $(patsubst %.c, build/%.o, $(SRCS))
LIB_NAME = lib$(PROJECT_NAME).so

$(PROJECT_NAME): $(OBJS)
	@echo "[LINK] Linking final executable: $@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: %.c $(HEADS)
	@echo "[CC] Compiling $< -> $@"
	@mkdir -p $(dir $@)  
	@$(CC) $(CFLAGS) -c $< -o $@

run: $(PROJECT_NAME)
	@echo "[RUN] Running the program: ./$(PROJECT_NAME) $(ARGS) $(IMG)"
	@./$(PROJECT_NAME) $(ARGS) $(IMG)

lib: CFLAGS += -fPIC
lib: $(OBJS)
	@echo "[LINK] Linking dynamic library: $(LIB_NAME)"
	@$(CC) -shared -o $(LIB_NAME) $^ $(LDFLAGS)

clean:
	@echo "[CLEAN] Cleaning up build artifacts"
	@rm -f $(PROJECT_NAME) $(LIB_NAME) $(OBJS)
	@rm -rf build

.PHONY: clean run
