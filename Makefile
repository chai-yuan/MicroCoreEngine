PROJECT_NAME = MicroCoreEngine.elf

GAME ?= image

SRCS_DIR = engine
HEADS_DIR = engine
BUILD_DIR = build

SRCS = $(shell find $(SRCS_DIR) -name '*.c')
SRCS += $(shell find games/$(GAME) -name '*.c')
HEADS = $(shell find $(SRCS_DIR) -name '*.h')

PLATFORM ?= linux_sdl2
PLATFORM_DIR = platform/$(PLATFORM)
include $(PLATFORM_DIR)/Makefile
