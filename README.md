# TinyWork

A cross-platform build system and runtime for size-coded demos and 256-byte productions. Supports DOS (COM), DOS (DJGPP), Windows (MinGW), and Linux.

## Overview

_TinyWork_ is a build framework for developing x86 assembly demos with cross-platform compilation support. It allows the same effect code to compile to multiple targets: a tiny `.com` binary for final production, and full 32-bit executables for Linux, Windows, and DOS with debugging capabilities.

_TinyWork_ is designed to be included as a `git` submodule. Your project repository contains only the effect-specific code, the framework is responsible for the build system, platform abstraction, and runtime components, but also generation of the final flattened `.asm` file.

## Installation

Add _TinyWork_ to your project as a git submodule:

```bash
git submodule add https://github.com/JakimPL/TinyWork.git tinywork
git submodule update --init --recursive
```

The framework expects your project to follow a specific structure with a `core/` directory containing your effect code. See the `tinywork/example/` directory for a complete working example demonstrating the minimal required files and structure.

## Project Structure

_TinyWork_ uses a contract-based approach where you provide specific assembly files in a `core/` directory. The framework includes these files at designated points during compilation, handling platform differences and function wrappers automatically.

Your `demo` project should be of the following form:

```
demo/
├── Makefile
├── core/
│   ├── frame.asm
│   ├── palette.asm
│   ├── consts.asm
│   ├── includes.asm
│   └── info.asm
└── tinywork/
    ├── Makefile.inc
    ├── main.asm
    ├── tiny/
    │   ├── consts.asm
    │   ├── dos.asm
    │   ├── frame.asm
    │   ├── includes.asm
    │   ├── m32.asm
    │   └── palette.asm
    ├── runtime/
    ├── scripts/
    └── video/
```

### Contract

The project is defined by a `Makefile` in your project root that sets two required variables and includes the framework's build system:

```makefile
PROJECT_NAME = demo
TINYWORK_DIR = tinywork

include $(TINYWORK_DIR)/Makefile.inc
```

The `PROJECT_NAME` determines the output binary names (e.g., `demo.com`, `demo-linux`). The `TINYWORK_DIR` points to the framework submodule. Including `Makefile.inc` brings in the complete build system with targets for all platforms.

Your project must provide a `core/` directory containing five assembly files that _TinyWork_ includes at specific points during compilation:

**`core/frame.asm`** — The main effect algorithm executed once per frame. Write your rendering code here.

**`core/palette.asm`** — The palette generation loop body. This code runs 256 times to build the color palette.

**`core/consts.asm`** — Project-specific constants and definitions.

**`core/includes.asm`** — Additional assembly files your effect needs (helper functions, lookup tables, utilities). Can be empty.

**`core/info.asm`** — Comment-only file for project metadata (title, author/group, ASCII art). No executable code.

The framework provides default entry points (`tinywork/main.c` and `tinywork/main.asm`) that work with this structure. These handle video mode setup, palette initialization, the main loop, and platform-specific details.

These files can be overriden by setting `MAIN_SRC` or `COM_SRC` in your `Makefile` if custom initialization is needed.

## Building

The build system provides targets for all supported platforms. Running `make` without arguments builds for your current platform:

```bash
make          # Builds for current platform (Linux or Windows)
make linux    # Build 32-bit Linux executable
make windows  # Build 32-bit Windows executable
make dos      # Build 32-bit DOS executable (requires DJGPP)
make com      # Build tiny COM binary
```

The `all-targets` target builds all platforms at once:

```bash
make all-targets
```

By default, builds are optimized (`-O2`). Enable debug mode for development with full debugging symbols and no optimization:

```bash
DEBUG=1 make linux
```

Debug builds include DWARF symbols on Linux, CodeView symbols on Windows, and allow stepping through both C runtime code and assembly effect code with `gdb` or other debuggers.

Additional utility targets:

```bash
make sizes    # Display size breakdown of built binaries
make run      # Build and run the executable for current platform
make clean    # Remove all build artifacts
make help     # Display all available targets
```

## Framework Components

### COM Entry Point (`main.asm`)

The COM binary entry point handles the complete initialization and main loop for DOS. The structure follows a simple pattern:

```asm
org 0x0100

%include "tiny/consts.asm"

start:
    ; Initialize graphical mode

__set_palette:
    %include "tiny/palette.asm"    ; Generate palette (calls core/palette.asm)

main_loop:
    ; Set video memory segment

    %ifndef NO_VSYNC
vsync:
    ; Wait for vertical retrace (optional)
    %endif

__frame:
    %include "tiny/frame.asm"      ; Main demo code (calls core/frame.asm)

    %ifndef SKIP_CHECK_INPUT
check_input:
    ; Check for the keyboard input
    jnz main_loop
    %else
continue:
    jmp main_loop
    %endif

    %ifdef RETURN_TO_DOS
return_to_dos:
    ; Gracefully return to DOS (optional)
    %endif
```

The framework includes your effect code at the marked points. Three optional flags control the behavior:

**`NO_VSYNC`** — By default, the demo waits for vertical retrace to prevent screen tearing. Set `NO_VSYNC=1` when building to skip this wait and maximize frame rate at the cost of potential visual artifacts.

**`SKIP_CHECK_INPUT`** — By default, the demo polls the keyboard each frame and exits when keyboard key is pressed. Set `SKIP_CHECK_INPUT=1` to remove this check and run in an infinite loop.

**`RETURN_TO_DOS`** — When the demo exits, this flag determines whether to restore text mode before returning to DOS. Disabled by default to save bytes. Only meaningful when `SKIP_CHECK_INPUT=0`.

These flags are passed on the command line (e.g., `NO_VSYNC=1 make com`) or set in your Makefile before including `Makefile.inc`.
