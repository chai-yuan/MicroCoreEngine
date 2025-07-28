PROJECT_NAME = MicroCoreEngine.elf

SRCS_DIR = engine
HEADS_DIR = engine
BUILD_DIR = build
$(wildcard *.c)

SRCS 	=	$(wildcard $(SRCS_DIR)/*.c)
SRCS 	+=	$(wildcard games/*.c)
HEADS 	=	$(wildcard $(SRCS_DIR)/*.h)

PLATFORM ?= sdl2_mini
PLATFORM_DIR = platform/$(PLATFORM)
include $(PLATFORM_DIR)/Makefile
