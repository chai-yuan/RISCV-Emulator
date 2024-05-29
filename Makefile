CC = gcc
LD = $(CC)

WORK_DIR 	= $(shell pwd)
BUILD_DIR 	= $(WORK_DIR)/build
TAR_DIR 	= $(BUILD_DIR)/obj

INC_PATH 	= $(WORK_DIR)/include
INCLUDE 	= $(addprefix -I, $(INC_PATH))

CFLAGS 	:= -O2 -MMD -Wall -Werror $(INCLUDE)
LDFLAGS := $(LDFLAGS)

SRCS =  $(shell find src -name "*.c")
OBJS = $(addprefix $(TAR_DIR)/, $(addsuffix .o, $(basename $(SRCS))))

SHARE ?= 0

ifeq ($(SHARE),1)
BINARY = $(BUILD_DIR)/emulator-so
CFLAGS  += -fPIC -fvisibility=hidden -DCONFIG_REF
LDFLAGS += -shared -fPIC
else 
BINARY = $(BUILD_DIR)/emulator
LIBS += -lSDL2 -ldl -pie
endif

-include $(OBJS:.o=.d)

$(TAR_DIR)/%.o: %.c
	@mkdir -p $(dir $@) && echo "[CC] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(BINARY)

$(BINARY): $(OBJS) 
	@mkdir -p $(dir $@) && echo "[LD] build/$(notdir $@)"
	@$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

ARGS ?= 

run: $(BINARY)
	@echo "[RUN] build/$(notdir $<)"
	@$(BINARY) $(ARGS)

clean:
	rm -rf $(BUILD_DIR)
