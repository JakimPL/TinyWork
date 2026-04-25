# An example project using TinyWork Framework

# Project name and structure (required)
PROJECT_NAME = example
TINYWORK_DIR = ..

# Source files
MAIN_SRC = # using default: tinywork/main.c
COM_SRC = # using default: tinywork/main.asm

# Build directories (leave empty to use defaults `bin` and `build`)
BIN_DIR =
BUILD_DIR =

# Custom flags (optional)
EXTRA_INCLUDES =
CFLAGS_EXTRA =

# Project-specific assembly defines (e.g., -DMY_FLAG)
ASMFLAGS_OPTIONS =

# Framework flags are handled automatically
# You can change some of the defaults to 1:
# NO_VSYNC = 1
# SKIP_CHECK_INPUT = 1
# RETURN_TO_DOS = 1

include $(TINYWORK_DIR)/Makefile.inc

# Project-specific targets
.PHONY: hello
hello:
	@echo "This is the $(PROJECT_NAME) project!"
	@echo "Using TinyWork framework from: $(TINYWORK_DIR)"
