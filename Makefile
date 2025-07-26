PROJECT_NAME = MicroCoreEngine.elf

SRCS_DIR = engine
HEADS_DIR = engine
BUILD_DIR = build

SRCS = $(shell find $(SRCS_DIR) -name '*.c')
SRCS += $(shell find games -name '*.c')
HEADS = $(shell find $(SRCS_DIR) -name '*.h')

PLATFORM ?= sdl2_mini
PLATFORM_DIR = platform/$(PLATFORM)
include $(PLATFORM_DIR)/Makefile
