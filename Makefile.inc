# TinyWork Framework - Generic cross-platform Makefile
# Include this from your project's Makefile after setting required variables

# Required variables (must be set before including):
#   PROJECT_NAME    - Name of your project (used for executable names)
#   TINYWORK_DIR    - Path to this framework directory
#   SOURCE_DIR      - Directory containing your project's assembly source files
#
# Optional variables (with defaults):
#   MAIN_C           - Main C source (default: main.c)
#   MAIN_ASM         - COM binary source (default: main.asm)
#   ASMFLAGS_OPTIONS - Custom assembly flags (default: empty)
#   EXTRA_INCLUDES   - Additional include paths (default: empty)
#
# Framework assembly flags (0 by default, can be overridden):
#   SKIP_SET_VIDEO_MODE           - Skip setting video mode
#   SKIP_SET_VIDEO_MEMORY_SEGMENT - Skip setting ES to video memory segment
#   NO_VSYNC                      - Disable VSYNC waiting
#   SKIP_CHECK_INPUT              - Skip keyboard input check (infinite loop)
#   RETURN_TO_DOS                 - Enable clean DOS exit
#
# Framework features (0 by default):
#   PCM_PLAYBACK                  - Play pcm_buffer through the host sound device
#                                   (32-bit SDL targets; DOS demos drive the card themselves)

# Validate required variables
ifndef PROJECT_NAME
$(error PROJECT_NAME must be defined before including framework Makefile)
endif

ifndef TINYWORK_DIR
$(error TINYWORK_DIR must be defined before including framework Makefile)
endif

ifndef SOURCE_DIR
$(error SOURCE_DIR must be defined before including framework Makefile)
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
AUDIO_DIR = $(TINYWORK_DIR)/audio
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
MAIN_C := $(or $(strip $(MAIN_C)),$(TINYWORK_DIR)/main.c)
MAIN_ASM := $(or $(strip $(MAIN_ASM)),$(TINYWORK_DIR)/main.asm)
M32_SRC := $(TINYWORK_DIR)/tiny/m32.asm
RUNTIME_SRC = $(RUNTIME_DIR)/runtime.c
SDL_SRC = $(VIDEO_DIR)/video_sdl.c
DOS_VIDEO_SRC = $(VIDEO_DIR)/video_dos.c
SDL_PCM_SRC = $(AUDIO_DIR)/pcm_sdl.c

# Flattening configuration
FLATTEN_MACROS := $(or $(strip $(FLATTEN_MACROS)),PALETTE_OUT)
FLATTEN_OPTIONAL_LABELS := $(or $(strip $(FLATTEN_OPTIONAL_LABELS)),initialize set_palette)

# Symbols the flattener treats as defined. This mirrors ASMFLAGS_COM, so the
# flattened source matches the binary the COM target actually produced.
FLATTEN_DEFINE_NAMES = $(foreach flag,$(filter -D%,$(ASMFLAGS_OPTIONS)),\
	$(firstword $(subst =, ,$(patsubst -D%,%,$(flag)))))
FLATTEN_DEFINES = DOS COM $(FLATTEN_DEFINE_NAMES)

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

# Assembly source paths
ASMINCLUDE_OPTIONS = -I$(TINYWORK_DIR) -I$(SOURCE_DIR)

# Everything the entry points pull in with %include. Assemblers resolve those themselves, so
# make is told about them here or an edit to a source file rebuilds nothing.
ASM_INCLUDED_SOURCES = $(wildcard $(SOURCE_DIR)/*.asm) $(wildcard $(TINYWORK_DIR)/tiny/*.asm)

# The assembly options a build ran with. They arrive on the command line, where make sees no
# timestamp to compare, so they are kept in a file: changing PIXEL_STEP or any other option
# rewrites the stamp and rebuilds what it feeds, while an unchanged one leaves the build alone.
ASM_FLAGS_STAMP = $(BUILD_DIR)/asmflags

# Scripts
SHOW_SIZES = $(TINYWORK_DIR)/scripts/show-sizes.sh
FLATTEN_ASM = $(TINYWORK_DIR)/scripts/flatten_asm.py

# Check if DOS compiler is available
DJGPP_AVAILABLE := $(shell command -v $(CC_DOS) 2>/dev/null)

# Base flags
CFLAGS_BASE = -Wall -Wextra -mno-sse -mfpmath=387 -ffast-math -fno-math-errno -DWINDOW_TITLE='"$(PROJECT_NAME)"'
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
CFLAGS_EXTRA ?=
EXTRA_INCLUDES ?=
ASMFLAGS_OPTIONS ?=

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

# PCM playback (opt-in, so demos without it link and assemble nothing extra)
PCM_PLAYBACK ?= 0

ifeq ($(PCM_PLAYBACK),1)
    CFLAGS_EXTRA += -DPCM_PLAYBACK
    ASMFLAGS_OPTIONS += -DPCM_PLAYBACK
    SDL_PCM_OBJ = $(SDL_PCM_SRC)
endif

# Platform-specific flags
CFLAGS_LINUX = $(CFLAGS_BASE) $(CFLAGS_DEBUG) $(CFLAGS_EXTRA) -I$(VIDEO_DIR) -I$(RUNTIME_DIR) $(EXTRA_INCLUDES) -m32
CFLAGS_WINDOWS = $(CFLAGS_BASE) $(CFLAGS_DEBUG) $(CFLAGS_EXTRA) -I$(VIDEO_DIR) -I$(RUNTIME_DIR) $(EXTRA_INCLUDES) -m32
CFLAGS_DOS = $(CFLAGS_BASE) $(CFLAGS_DEBUG) $(CFLAGS_EXTRA) -I$(VIDEO_DIR) -I$(RUNTIME_DIR) $(EXTRA_INCLUDES)
ASMFLAGS_LINUX = -f elf32 $(ASMFLAGS_DEBUG_LINUX) $(ASMFLAGS_OPTIONS) $(ASMINCLUDE_OPTIONS) -DLINUX
ASMFLAGS_WINDOWS = -f win32 --prefix _ $(ASMFLAGS_DEBUG_WINDOWS) $(ASMFLAGS_OPTIONS) $(ASMINCLUDE_OPTIONS) -DWINDOWS
ASMFLAGS_DOS = -f coff --prefix _ $(ASMFLAGS_DEBUG_DOS) $(ASMFLAGS_OPTIONS) $(ASMINCLUDE_OPTIONS) -DDOS
ASMFLAGS_COM = -f bin $(ASMFLAGS_OPTIONS) $(ASMINCLUDE_OPTIONS) -DDOS -DCOM

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
ifdef MAIN_ASM
all: com
endif

# All targets (cross-platform)
.PHONY: all-targets
all-targets: clean linux windows dos com

# Create build directories
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(ASM_FLAGS_STAMP): FORCE | $(BUILD_DIR)
	@printf '%s\n' '$(ASMFLAGS_OPTIONS)' | cmp -s - $@ || printf '%s\n' '$(ASMFLAGS_OPTIONS)' > $@

.PHONY: FORCE
FORCE: ;

$(LINUX_BUILD):
	@mkdir -p $(LINUX_BUILD)

$(WINDOWS_BUILD):
	@mkdir -p $(WINDOWS_BUILD)

$(DOS_BUILD):
	@mkdir -p $(DOS_BUILD)

# Assembly object file rules (only if M32_SRC is defined)
ifdef M32_SRC
$(M32_OBJ_LINUX): $(M32_SRC) $(ASM_INCLUDED_SOURCES) $(ASM_FLAGS_STAMP) | $(LINUX_BUILD)
	@echo "$(COLOR_BOLD)Assembling $(M32_SRC) for Linux...$(COLOR_RESET)"
	$(ASM) $(ASMFLAGS_LINUX) -o $@ $<

$(M32_OBJ_WINDOWS): $(M32_SRC) $(ASM_INCLUDED_SOURCES) $(ASM_FLAGS_STAMP) | $(WINDOWS_BUILD)
	@echo "$(COLOR_BOLD)Assembling $(M32_SRC) for Windows...$(COLOR_RESET)"
	$(ASM) $(ASMFLAGS_WINDOWS) -o $@ $<

$(M32_OBJ_DOS): $(M32_SRC) $(ASM_INCLUDED_SOURCES) $(ASM_FLAGS_STAMP) | $(DOS_BUILD)
	@echo "$(COLOR_BOLD)Assembling $(M32_SRC) for DOS...$(COLOR_RESET)"
	$(ASM) $(ASMFLAGS_DOS) -o $@ $<
endif

# Linux build (SDL2)
.PHONY: linux
linux: $(LINUX_OUT)

$(LINUX_OUT): $(MAIN_C) $(RUNTIME_SRC) $(SDL_SRC) $(SDL_PCM_OBJ) $(M32_OBJ_LINUX) | $(BIN_DIR)
	@echo "$(COLOR_BLUE)==> Building for Linux (SDL2)$(COLOR_RESET)"
	$(CC_LINUX) $(CFLAGS_LINUX) -o $@ $(MAIN_C) $(RUNTIME_SRC) $(SDL_SRC) $(SDL_PCM_OBJ) $(M32_OBJ_LINUX) $(LDFLAGS_LINUX)
	@echo "$(COLOR_GREEN)Linux build complete: $(LINUX_OUT)$(COLOR_RESET)"
	@echo

# Windows build (MinGW32)
.PHONY: windows
windows: $(WINDOWS_OUT)

$(WINDOWS_OUT): $(MAIN_C) $(RUNTIME_SRC) $(SDL_SRC) $(SDL_PCM_OBJ) $(M32_OBJ_WINDOWS) | $(BIN_DIR)
	@echo "$(COLOR_CYAN)==> Building for Windows (MinGW32 + SDL2)$(COLOR_RESET)"
	$(CC_WINDOWS) $(CFLAGS_WINDOWS) -o $@ $(MAIN_C) $(RUNTIME_SRC) $(SDL_SRC) $(SDL_PCM_OBJ) $(M32_OBJ_WINDOWS) $(LDFLAGS_WINDOWS)
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

$(DOS_OUT): $(MAIN_C) $(RUNTIME_SRC) $(DOS_VIDEO_SRC) $(M32_OBJ_DOS) | $(BIN_DIR)
	@echo "$(COLOR_MAGENTA)==> Building for DOS (DJGPP)$(COLOR_RESET)"
	$(CC_DOS) $(CFLAGS_DOS) -o $@ $(MAIN_C) $(RUNTIME_SRC) $(DOS_VIDEO_SRC) $(M32_OBJ_DOS) $(LDFLAGS_DOS)
	@echo "$(COLOR_GREEN)DOS build complete: $(DOS_OUT)$(COLOR_RESET)"
	@echo

# COM build (Assembly only, optional)
ifdef MAIN_ASM
.PHONY: com
com: $(COM_OUT)

$(COM_OUT): $(MAIN_ASM) $(ASM_INCLUDED_SOURCES) $(ASM_FLAGS_STAMP) | $(BIN_DIR)
	@echo "$(COLOR_YELLOW)==> Building COM file$(COLOR_RESET)"
	$(ASM) $(ASMFLAGS_COM) $< -o $@ -l $(BIN_DIR)/$(LST_OUT)
	@echo "$(COLOR_GREEN)COM build complete: $(COM_OUT) ($$(stat -c%s $@ 2>/dev/null || stat -f%z $@) bytes)$(COLOR_RESET)"
	@echo "$(COLOR_BOLD)Listing: $(BIN_DIR)/$(LST_OUT)$(COLOR_RESET)"
	@echo
else
.PHONY: com
com:
	@echo "$(COLOR_YELLOW)COM build not configured (MAIN_ASM not defined)$(COLOR_RESET)"
endif

# Show instruction sizes from listing
.PHONY: sizes
sizes:
ifdef MAIN_ASM
	@if [ -x "$(SHOW_SIZES)" ]; then \
		$(SHOW_SIZES) $(BIN_DIR)/$(LST_OUT); \
	else \
		echo "show-sizes.sh script not found"; \
	fi
else
	@echo "$(COLOR_YELLOW)sizes target requires MAIN_ASM to be defined$(COLOR_RESET)"
endif

# Generate flattened assembly code
.PHONY: code
code:
	@echo "$(COLOR_BOLD)Generating flattened assembly code...$(COLOR_RESET)"
	@python3 $(FLATTEN_ASM) \
		--project-directory . \
		--framework-directory $(TINYWORK_DIR) \
		--src-directory $(SOURCE_DIR) \
		--input $(MAIN_ASM) \
		--output $(FLATTENED_ASM) \
		--inline-macros $(FLATTEN_MACROS) \
		--optional-labels $(FLATTEN_OPTIONAL_LABELS) \
		--defines $(FLATTEN_DEFINES)
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
ifdef MAIN_ASM
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
	@echo "  make PCM_PLAYBACK=1 linux - Play pcm_buffer through the host sound device"
	@echo "  make NO_VSYNC=1 com     - Disable VSYNC waiting (faster, may tear)"
	@echo "  make SKIP_CHECK_INPUT=1 com - Skip keyboard check (infinite loop)"
	@echo "  make RETURN_TO_DOS=1 com - Enable clean DOS exit (costs bytes)"
	@echo ""
	@echo "Project: $(PROJECT_NAME)"
	@echo "Framework: $(TINYWORK_DIR)"
