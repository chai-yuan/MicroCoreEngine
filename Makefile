PROJECT_NAME = MicroCoreEngine.elf

GAME_NAME ?= game_1

SRCS_DIR = engine
GAME_DIR = games
HEADS_DIR = engine
BUILD_DIR = build

SRCS 	=	$(wildcard $(SRCS_DIR)/*.c)
SRCS 	+=	$(wildcard $(GAME_DIR)/$(GAME_NAME)/*.c)
HEADS 	=	$(wildcard $(SRCS_DIR)/*.h)
HEADS 	+=	$(wildcard $(GAME_DIR)/$(GAME_NAME)/*.h)

PLATFORM ?= sdl2_mini
PLATFORM_DIR = platform/$(PLATFORM)
include $(PLATFORM_DIR)/Makefile
