# An example project using TinyWork Framework

# Project name and structure (required)
PROJECT_NAME = example
FRAMEWORK_DIR = ..

# Source files
MAIN_SRC = main.c
M32_SRC = demo.asm
COM_SRC = main.asm

# Build directories (leave empty to use defaults `bin` and `build`)
BIN_DIR =
BUILD_DIR =

# Custom flags (optional)
EXTRA_INCLUDES =
CFLAGS_EXTRA =
ASMFLAGS_OPTIONS =

include $(FRAMEWORK_DIR)/Makefile.inc

# Project-specific targets
.PHONY: hello
hello:
	@echo "This is the $(PROJECT_NAME) project!"
	@echo "Using TinyWork framework from: $(FRAMEWORK_DIR)"
