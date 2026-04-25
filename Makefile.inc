# TinyWork Framework - Generic cross-platform Makefile
# Include this from your project's Makefile after setting required variables

# Required variables (must be set before including):
#   PROJECT_NAME    - Name of your project (used for executable names)
#   TINYWORK_DIR   - Path to this framework directory
#
# Optional variables (with defaults):
#   MAIN_SRC        - Main C source (default: main.c)
#   M32_SRC         - 32-bit assembly source (default: not built)
#   COM_SRC         - COM binary source (default: not built)
#   ASMFLAGS_OPTIONS - Custom assembly flags (default: empty)
#   EXTRA_INCLUDES  - Additional include paths (default: empty)
#
# Framework assembly flags (0 by default, can be overridden):
#   SKIP_SET_VIDEO_MODE           - Skip setting video mode
#   SKIP_SET_VIDEO_MEMORY_SEGMENT - Skip setting ES to video memory segment
#   NO_VSYNC                      - Disable VSYNC waiting
#   SKIP_CHECK_INPUT              - Skip keyboard input check (infinite loop)
#   RETURN_TO_DOS                 - Enable clean DOS exit

# Validate required variables
ifndef PROJECT_NAME
$(error PROJECT_NAME must be defined before including framework Makefile)
endif

ifndef TINYWORK_DIR
$(error TINYWORK_DIR must be defined before including framework Makefile)
endif

# Detect platform
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM := Linux
    endif
endif

# ANSI color codes (disabled for Windows)
ifneq ($(PLATFORM),Windows)
    COLOR_RESET   := \033[0m
    COLOR_BOLD    := \033[1m
    COLOR_BLUE    := \033[1;34m
    COLOR_CYAN    := \033[1;36m
    COLOR_MAGENTA := \033[1;35m
    COLOR_YELLOW  := \033[1;33m
    COLOR_GREEN   := \033[1;32m
else
    COLOR_RESET   :=
    COLOR_BOLD    :=
    COLOR_BLUE    :=
    COLOR_CYAN    :=
    COLOR_MAGENTA :=
    COLOR_YELLOW  :=
    COLOR_GREEN   :=
endif

# Directories (relative to project root)
BIN_DIR := $(or $(strip $(BIN_DIR)),bin)
BUILD_DIR := $(or $(strip $(BUILD_DIR)),build)
VIDEO_DIR = $(TINYWORK_DIR)/video
RUNTIME_DIR = $(TINYWORK_DIR)/runtime

LINUX_BUILD = $(BUILD_DIR)/linux
WINDOWS_BUILD = $(BUILD_DIR)/windows
DOS_BUILD = $(BUILD_DIR)/dos

# Executables
LINUX_OUT = $(BIN_DIR)/$(PROJECT_NAME)-linux
WINDOWS_OUT = $(BIN_DIR)/$(PROJECT_NAME)-windows.exe
DOS_OUT = $(BIN_DIR)/$(PROJECT_NAME).exe
COM_OUT = $(BIN_DIR)/$(PROJECT_NAME).com
LST_OUT = $(PROJECT_NAME).lst
FLATTENED_ASM := $(or $(strip $(FLATTENED_ASM)),$(PROJECT_NAME).asm)

# Default source files
MAIN_SRC := $(or $(strip $(MAIN_SRC)),$(TINYWORK_DIR)/main.c)
COM_SRC := $(or $(strip $(COM_SRC)),$(TINYWORK_DIR)/main.asm)
M32_SRC := $(TINYWORK_DIR)/tiny/m32.asm
RUNTIME_SRC = $(RUNTIME_DIR)/runtime.c
SDL_SRC = $(VIDEO_DIR)/video_sdl.c
DOS_VIDEO_SRC = $(VIDEO_DIR)/video_dos.c

# Flattening configuration
FLATTEN_MACROS := $(or $(strip $(FLATTEN_MACROS)),PALETTE_OUT)

# Object files (in build subdirectories)
ifdef M32_SRC
M32_OBJ_LINUX = $(LINUX_BUILD)/m32.o
M32_OBJ_WINDOWS = $(WINDOWS_BUILD)/m32.o
M32_OBJ_DOS = $(DOS_BUILD)/m32.o
endif

# Compilers
CC_LINUX = gcc
CC_WINDOWS = i686-w64-mingw32-gcc
CC_DOS = i586-pc-msdosdjgpp-gcc
ASM = nasm

# Scripts
SHOW_SIZES = $(TINYWORK_DIR)/scripts/show-sizes.sh
FLATTEN_ASM = $(TINYWORK_DIR)/scripts/flatten-asm.py

# Check if DOS compiler is available
DJGPP_AVAILABLE := $(shell command -v $(CC_DOS) 2>/dev/null)

# Base flags
CFLAGS_BASE = -Wall -Wextra -mno-sse -mfpmath=387 -ffast-math -fno-math-errno
ASMFLAGS_BASE =

# Debug mode (set DEBUG=1 to enable)
DEBUG ?= 0

# Debug flags
ifeq ($(DEBUG),1)
    CFLAGS_DEBUG = -g -O0
    ASMFLAGS_DEBUG_LINUX = -g -F dwarf
    ASMFLAGS_DEBUG_WINDOWS = -g -F cv8
    ASMFLAGS_DEBUG_DOS = -g
else
    CFLAGS_DEBUG = -O2
    ASMFLAGS_DEBUG_LINUX =
    ASMFLAGS_DEBUG_WINDOWS =
    ASMFLAGS_DEBUG_DOS =
endif

# Custom flags (provided by including Makefile)
ASMFLAGS_OPTIONS ?=
CFLAGS_EXTRA ?=
EXTRA_INCLUDES ?=

# Framework assembly flags (can be overridden by user)
# Set 1 to enable, 0 to disable (default: 0)
SKIP_SET_VIDEO_MODE ?= 0
SKIP_SET_VIDEO_MEMORY_SEGMENT ?= 0
NO_VSYNC ?= 0
SKIP_CHECK_INPUT ?= 0
RETURN_TO_DOS ?= 0

ifeq ($(SKIP_SET_VIDEO_MODE),1)
    ASMFLAGS_OPTIONS += -DSKIP_SET_VIDEO_MODE
endif
ifeq ($(SKIP_SET_VIDEO_MEMORY_SEGMENT),1)
    ASMFLAGS_OPTIONS += -DSKIP_SET_VIDEO_MEMORY_SEGMENT
endif
ifeq ($(NO_VSYNC),1)
    ASMFLAGS_OPTIONS += -DNO_VSYNC
endif
ifeq ($(SKIP_CHECK_INPUT),1)
    ASMFLAGS_OPTIONS += -DSKIP_CHECK_INPUT
endif
ifeq ($(RETURN_TO_DOS),1)
    ASMFLAGS_OPTIONS += -DRETURN_TO_DOS
endif

# Platform-specific flags
CFLAGS_LINUX = $(CFLAGS_BASE) $(CFLAGS_DEBUG) $(CFLAGS_EXTRA) -I$(VIDEO_DIR) -I$(RUNTIME_DIR) $(EXTRA_INCLUDES) -m32
CFLAGS_WINDOWS = $(CFLAGS_BASE) $(CFLAGS_DEBUG) $(CFLAGS_EXTRA) -I$(VIDEO_DIR) -I$(RUNTIME_DIR) $(EXTRA_INCLUDES) -m32
CFLAGS_DOS = $(CFLAGS_BASE) $(CFLAGS_DEBUG) $(CFLAGS_EXTRA) -I$(VIDEO_DIR) -I$(RUNTIME_DIR) $(EXTRA_INCLUDES)
ASMFLAGS_LINUX = -f elf32 $(ASMFLAGS_DEBUG_LINUX) $(ASMFLAGS_OPTIONS) -I$(TINYWORK_DIR) -I. -DLINUX
ASMFLAGS_WINDOWS = -f win32 --prefix _ $(ASMFLAGS_DEBUG_WINDOWS) $(ASMFLAGS_OPTIONS) -I$(TINYWORK_DIR) -I. -DWINDOWS
ASMFLAGS_DOS = -f coff --prefix _ $(ASMFLAGS_DEBUG_DOS) $(ASMFLAGS_OPTIONS) -I$(TINYWORK_DIR) -I. -DDOS
ASMFLAGS_COM = -f bin $(ASMFLAGS_OPTIONS) -I$(TINYWORK_DIR) -I. -DDOS -DCOM

# Libraries
LDFLAGS_LINUX = -lSDL2 -lm -Wl,-z,noexecstack -no-pie
LDFLAGS_WINDOWS = -lmingw32 -lSDL2main -lSDL2 -lm -mwindows
LDFLAGS_DOS = -lm

# Default target (builds for current platform + optional targets)
.PHONY: all
ifeq ($(PLATFORM),Windows)
all: clean windows
else ifeq ($(PLATFORM),Linux)
all: clean linux
else
all: clean linux windows
endif

# Append DOS if available
ifdef DJGPP_AVAILABLE
all: dos
endif

# Append COM if source is defined
ifdef COM_SRC
all: com
endif

# All targets (cross-platform)
.PHONY: all-targets
all-targets: clean linux windows dos com

# Create build directories
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(LINUX_BUILD):
	@mkdir -p $(LINUX_BUILD)

$(WINDOWS_BUILD):
	@mkdir -p $(WINDOWS_BUILD)

$(DOS_BUILD):
	@mkdir -p $(DOS_BUILD)

# Assembly object file rules (only if M32_SRC is defined)
ifdef M32_SRC
$(M32_OBJ_LINUX): $(M32_SRC) | $(LINUX_BUILD)
	@echo "$(COLOR_BOLD)Assembling $(M32_SRC) for Linux...$(COLOR_RESET)"
	$(ASM) $(ASMFLAGS_LINUX) -o $@ $<

$(M32_OBJ_WINDOWS): $(M32_SRC) | $(WINDOWS_BUILD)
	@echo "$(COLOR_BOLD)Assembling $(M32_SRC) for Windows...$(COLOR_RESET)"
	$(ASM) $(ASMFLAGS_WINDOWS) -o $@ $<

$(M32_OBJ_DOS): $(M32_SRC) | $(DOS_BUILD)
	@echo "$(COLOR_BOLD)Assembling $(M32_SRC) for DOS...$(COLOR_RESET)"
	$(ASM) $(ASMFLAGS_DOS) -o $@ $<
endif

# Linux build (SDL2)
.PHONY: linux
linux: $(LINUX_OUT)

$(LINUX_OUT): $(MAIN_SRC) $(RUNTIME_SRC) $(SDL_SRC) $(M32_OBJ_LINUX) | $(BIN_DIR)
	@echo "$(COLOR_BLUE)==> Building for Linux (SDL2)$(COLOR_RESET)"
	$(CC_LINUX) $(CFLAGS_LINUX) -o $@ $(MAIN_SRC) $(RUNTIME_SRC) $(SDL_SRC) $(M32_OBJ_LINUX) $(LDFLAGS_LINUX)
	@echo "$(COLOR_GREEN)Linux build complete: $(LINUX_OUT)$(COLOR_RESET)"
	@echo

# Windows build (MinGW32)
.PHONY: windows
windows: $(WINDOWS_OUT)

$(WINDOWS_OUT): $(MAIN_SRC) $(RUNTIME_SRC) $(SDL_SRC) $(M32_OBJ_WINDOWS) | $(BIN_DIR)
	@echo "$(COLOR_CYAN)==> Building for Windows (MinGW32 + SDL2)$(COLOR_RESET)"
	$(CC_WINDOWS) $(CFLAGS_WINDOWS) -o $@ $(MAIN_SRC) $(RUNTIME_SRC) $(SDL_SRC) $(M32_OBJ_WINDOWS) $(LDFLAGS_WINDOWS)
	@echo "$(COLOR_GREEN)Windows build complete: $(WINDOWS_OUT)$(COLOR_RESET)"
	@echo

# DOS build (DJGPP), optional
.PHONY: dos
dos:
ifeq ($(DJGPP_AVAILABLE),)
	@echo "$(COLOR_YELLOW)Warning: DJGPP compiler ($(CC_DOS)) not found$(COLOR_RESET)"
	@echo "Skipping DOS build. Install DJGPP cross-compiler to build DOS target."
	@echo
else
	@$(MAKE) $(DOS_OUT)
endif

$(DOS_OUT): $(MAIN_SRC) $(RUNTIME_SRC) $(DOS_VIDEO_SRC) $(M32_OBJ_DOS) | $(BIN_DIR)
	@echo "$(COLOR_MAGENTA)==> Building for DOS (DJGPP)$(COLOR_RESET)"
	$(CC_DOS) $(CFLAGS_DOS) -o $@ $(MAIN_SRC) $(RUNTIME_SRC) $(DOS_VIDEO_SRC) $(M32_OBJ_DOS) $(LDFLAGS_DOS)
	@echo "$(COLOR_GREEN)DOS build complete: $(DOS_OUT)$(COLOR_RESET)"
	@echo

# COM build (Assembly only, optional)
ifdef COM_SRC
.PHONY: com
com: $(COM_OUT)

$(COM_OUT): $(COM_SRC) | $(BIN_DIR)
	@echo "$(COLOR_YELLOW)==> Building COM file$(COLOR_RESET)"
	$(ASM) $(ASMFLAGS_COM) $< -o $@ -l $(BIN_DIR)/$(LST_OUT)
	@if [ -x "$(SHOW_SIZES)" ]
	@echo "$(COLOR_GREEN)COM build complete: $(COM_OUT) ($$(stat -c%s $@ 2>/dev/null || stat -f%z $@) bytes)$(COLOR_RESET)"
	@echo "$(COLOR_BOLD)Listing: $(BIN_DIR)/$(LST_OUT)$(COLOR_RESET)"
	@echo
else
.PHONY: com
com:
	@echo "$(COLOR_YELLOW)COM build not configured (COM_SRC not defined)$(COLOR_RESET)"
endif

# Show instruction sizes from listing
.PHONY: sizes
sizes:
ifdef COM_SRC
	@if [ -x "$(SHOW_SIZES)" ]; then \
		$(SHOW_SIZES) $(BIN_DIR)/$(LST_OUT); \
	else \
		echo "show-sizes.sh script not found"; \
	fi
else
	@echo "$(COLOR_YELLOW)sizes target requires COM_SRC to be defined$(COLOR_RESET)"
endif

# Generate flattened assembly code
.PHONY: code
code:
	@echo "$(COLOR_BOLD)Generating flattened assembly code...$(COLOR_RESET)"
	@python3 $(FLATTEN_ASM) \
		--project-directory . \
		--framework-directory $(TINYWORK_DIR) \
		--input $(COM_SRC) \
		--output $(FLATTENED_ASM) \
		--inline-macros $(FLATTEN_MACROS)
	@echo

# Run platform-specific build
.PHONY: run
ifeq ($(PLATFORM),Windows)
run: windows
	$(WINDOWS_OUT)
else
run: linux
	$(LINUX_OUT)
endif

# Clean build artifacts
.PHONY: clean
clean:
	@echo "$(COLOR_BOLD)Cleaning build artifacts...$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR)
	@rm -f $(LINUX_OUT) $(WINDOWS_OUT) $(DOS_OUT) $(COM_OUT) $(BIN_DIR)/$(LST_OUT)
	@echo "Cleaned build artifacts"
	@echo

# Help
.PHONY: help
help:
	@echo "$(COLOR_BOLD)$(PROJECT_NAME) - TinyWork Framework Build System$(COLOR_RESET)"
	@echo ""
	@echo "Available targets:"
	@echo "  make linux       - Build for Linux with SDL2"
	@echo "  make windows     - Build for Windows with MinGW32 and SDL2"
	@echo "  make dos         - Cross-compile for DOS with DJGPP"
ifdef COM_SRC
	@echo "  make com         - Assemble standalone .com file"
	@echo "  make sizes       - Show instruction sizes from .lst file"
endif
	@echo "  make code        - Generate flattened assembly code"
	@echo "  make run         - Build and run platform-specific executable"
	@echo "  make clean       - Remove build artifacts"
	@echo "  make all         - Build for current platform"
	@echo "  make all-targets - Build all targets (cross-platform)"
	@echo ""
	@echo "Debug mode:"
	@echo "  make DEBUG=1 linux      - Build Linux version with debug symbols"
	@echo "  make DEBUG=1 windows    - Build Windows version with debug symbols"
	@echo "  make DEBUG=1 all        - Build with debug symbols"
	@echo ""
	@echo "Framework flags:"
	@echo "  make NO_VSYNC=1 com     - Disable VSYNC waiting (faster, may tear)"
	@echo "  make SKIP_CHECK_INPUT=1 com - Skip keyboard check (infinite loop)"
	@echo "  make RETURN_TO_DOS=1 com - Enable clean DOS exit (costs bytes)"
	@echo ""
	@echo "Project: $(PROJECT_NAME)"
	@echo "Framework: $(TINYWORK_DIR)"
