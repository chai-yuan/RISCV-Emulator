# 构建配置
PROJECT_NAME = cremu
SRCS_DIR = src
HEADS_DIR = include
SRCS = $(shell find $(SRCS_DIR) -name '*.c')
HEADS = $(shell find $(HEADS_DIR) -name '*.h')
OBJS = $(patsubst $(SRCS_DIR)/%.c, build/%.o, $(SRCS))

SRCS += test/main.c
OBJS += build/main.c

CC = gcc
CFLAGS = -Wall -Werror -I$(HEADS_DIR)
LDFLAGS =

IMG  ?= 
ARGS ?= 

# 构建命令
$(PROJECT_NAME): $(OBJS)
	@echo "[LINK] Linking final executable: $@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): build/%.o : src/%.c $(HEADS)
	@echo "[CC] Compiling $< -> $@"
	@mkdir -p $(dir $@)  
	@$(CC) $(CFLAGS) -c $< -o $@

run: $(PROJECT_NAME)
	@echo "[RUN] Running the program: ./$(PROJECT_NAME) $(ARGS) $(IMG)"
	@./$(PROJECT_NAME) $(ARGS) $(IMG)

clean:
	@echo "[CLEAN] Cleaning up build artifacts"
	@rm -f $(PROJECT_NAME) $(OBJS)

.PHONY: clean run
