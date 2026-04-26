# TinyWork

A cross-platform build system and runtime for size-coded demos and 256-byte productions. Supports DOS COM binaries and 32-bit executables for DOS (DJGPP), Windows (MinGW), and Linux.

Windows and Linux builds use SDL2 for video output. The 32-bit DOS target requires the `CWSDPMI.EXE` extender.

## Overview

_TinyWork_ is a build framework for developing x86 assembly demos with cross-platform compilation support. It allows the same effect code to compile to multiple targets: a tiny `.com` binary for final production, and full 32-bit executables for Linux, Windows, and DOS with debugging capabilities.

_TinyWork_ is designed to be included as a `git` submodule. Your project repository contains only the effect-specific code, the framework is responsible for the build system, platform abstraction, and runtime components, but also generation of the final flattened `.asm` file.

## Installation

Add _TinyWork_ to your project as a git submodule:

```bash
git submodule add https://github.com/JakimPL/TinyWork.git tinywork
git submodule update --init --recursive
```

The framework expects your project to follow a specific structure with a source directory (`core/` by default) containing your effect code. See the `tinywork/example/` directory for a complete working example demonstrating the minimal required files and structure.

## Project Structure

_TinyWork_ uses a contract-based approach where you provide specific assembly files in the declared source directory. The framework includes these files at designated points during compilation, handling platform differences and function wrappers automatically.

Your `demo` project should be of the following form:

```
demo/
├── Makefile               ; Project definition
├── core/                  ; Source directory, `core` by default
│   ├── frame.asm
│   ├── palette.asm
│   ├── consts.asm
│   ├── includes.asm
│   └── info.asm
└── tinywork/              ; TinyWork submodule
    ├── Makefile.inc       ; Framework compilation tools
    ├── main.asm           ; COM entry point
    ├── main.c             ; 32-bit targets entry point
    ├── tiny/              ; TinyWork assembly code
    │   ├── consts.asm
    │   ├── dos.asm
    │   ├── frame.asm
    │   ├── includes.asm
    │   ├── m32.asm
    │   └── palette.asm
    ├── runtime/           ; 32-bit targets application logic
    ├── scripts/           ; Utilities
    └── video/             ; SDL2/DOS VGA C interfaces
```

### Contract

The project is defined by a `Makefile` in your project root that sets three required variables and includes the framework's build system:

```makefile
PROJECT_NAME = demo
TINYWORK_DIR = tinywork
SOURCE_DIR = core

include $(TINYWORK_DIR)/Makefile.inc
```

The `PROJECT_NAME` determines the output binary names (e.g., `demo.com`, `demo-linux`). The `TINYWORK_DIR` points to the framework submodule (usually `tinywork`). The `SOURCE_DIR` specifies the directory containing your project's assembly source files (defaults to `core` if not specified).

Including `Makefile.inc` brings in the complete build system with targets for all platforms.

Your project must provide the following source assembly files that _TinyWork_ includes at specific points during compilation:

**`frame.asm`** — The main effect algorithm executed once per frame. Write your rendering code here.

**`palette.asm`** — The palette generation code.

**`init.asm`** — Custom initialization logic for the COM entry point. Can be empty.

**`consts.asm`** — Project-specific constants and definitions.

**`includes.asm`** — Additional assembly files your effect needs (data, helper functions, declared variables etc.). Can be empty.

**`info.asm`** — Comment-only file for project metadata (title, author/group, ASCII art). No executable code.

The framework provides default entry points (`tinywork/main.c` and `tinywork/main.asm`) that work with this structure. These handle video mode setup, initialization logic including palette construction, the main loop, and platform-specific details.

These files can be overridden by setting `MAIN_C` or `MAIN_ASM` in your `Makefile` if custom initialization is needed.

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

The COM binary entry point handles the complete initialization and main loop. This file is used exclusively for the COM target. The structure follows a simple pattern:

```asm
org 0x0100

%include "tiny/consts.asm"

start:
    %ifndef SKIP_SET_VIDEO_MODE
; Initialize graphical mode (optional)
    %endif

__initialize:
    %include "tiny/init.asm"       ; Custom initialization (calls init.asm)

__set_palette:
    %include "tiny/palette.asm"    ; Generate palette (calls palette.asm)

main_loop:
    %ifndef SKIP_SET_VIDEO_MEMORY_SEGMENT
; Set video memory segment (optional)
    %endif

    %ifndef NO_VSYNC
vsync:
; Wait for vertical retrace (optional)
    %endif

__frame:
    %include "tiny/frame.asm"      ; Main demo code (calls frame.asm)

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

    %include "tiny/includes.asm"   ; Include other source files
```

The framework includes your effect code at the marked points. Five optional flags control the behavior:

**`SKIP_SET_VIDEO_MODE`** — The initialization sequence sets VGA mode 13h (320×200 256-color graphics mode) on startup. Set `SKIP_SET_VIDEO_MODE=1` to omit this if your demo handles video mode setup manually or assumes it's already configured.

**`SKIP_SET_VIDEO_MEMORY_SEGMENT`** — By default, the demo sets the `es` segment register to `0xA000` (VGA video memory) before each frame. Set `SKIP_SET_VIDEO_MEMORY_SEGMENT=1` to skip this if your frame code doesn't use `es` for video memory access or sets it manually.

**`NO_VSYNC`** — The demo uses VSYNC for vertical retrace to prevent screen tearing. Set `NO_VSYNC=1` when building to skip this wait and maximize frame rate at the cost of potential visual artifacts.

**`SKIP_CHECK_INPUT`** — By default, the demo polls the keyboard each frame and exits when keyboard key is pressed. Set `SKIP_CHECK_INPUT=1` to remove this check and run in an infinite loop.

**`RETURN_TO_DOS`** — When the demo exits, this flag determines whether to restore text mode before returning to DOS. Disabled by default to save bytes. Only meaningful when `SKIP_CHECK_INPUT=0`.

These flags are passed on the command line (e.g., `NO_VSYNC=1 make com`) or set in your Makefile before including `Makefile.inc`.

### Initialization (`tiny/init.asm`)

The initialization wrapper allows you to execute custom setup code before the main loop begins. This code runs once after video mode setup but before palette generation, and works for both COM and 32-bit targets.

The framework includes `init.asm` through the wrapper:

```asm
section .text
initialize:
    %ifndef COM
    pusha
    %endif

    %include "init.asm"

    %ifndef COM
    popa
    ret
    %endif
```

For COM builds, your initialization code is included inline. For 32-bit builds, it's wrapped in an `initialize()` function with automatic register preservation.

Use `init.asm` for one-time setup tasks such as calculating lookup tables, initializing variables, or preparing data structures. This file can be empty if no initialization is needed.

### Palette Generation (`tiny/palette.asm`)

The palette generation wrapper sets up the necessary infrastructure and calls your `palette.asm` code to generate the 256-color VGA palette. For 32-bit targets (Linux, Windows, DOS executable), `set_palette` is exported as a callable function that the C runtime invokes during initialization.

The framework defines a `PALETTE_OUT` macro that abstracts platform differences:

```asm
%ifdef DOS
    %macro PALETTE_OUT 0
    out dx, al          ; Output to VGA I/O port
    %endmacro
%else
    %macro PALETTE_OUT 0
    stosb               ; Write to palette buffer
    %endmacro
%endif
```

Your `palette.asm` should set up the loop and destination registers, then generate palette entries by calling `PALETTE_OUT` for each RGB component:

```asm
palette:
    %ifdef DOS
    mov dx, PALETTE_DATA_PORT    ; VGA palette data port
    mov cx, 0x100                ; 256 colors
    %else
    mov edi, palette_data        ; Palette buffer
    mov ecx, 0x100
    %endif

; Common palette code: grayscale
.palette_loop:
    mov al, bl
    shr al, 2                    ; VGA uses 6-bit data per channel

    PALETTE_OUT                  ; R
    PALETTE_OUT                  ; G
    PALETTE_OUT                  ; B

    inc bx
    loop .palette_loop
```

For 32-bit platforms, the framework treats `set_palette` as a function called by C routines. On DOS (both COM and 32-bit target), colors are written directly to VGA hardware. On SDL2 targets, the code populates the buffer that the video subsystem uses to configure SDL2's palette.

Note that for COM builds, you can leave the palette generation empty to use the VGA hardware's default palette without any overhead. However, for SDL2 targets, the palette must be explicitly initialized as there is no default.

### Frame Rendering (`tiny/frame.asm`)

The frame rendering wrapper calls your `frame.asm` code once per iteration of the main loop. The framework handles the platform-specific differences between COM and 32-bit builds automatically.

```asm
    section .text
frame:
    %ifndef COM
    pusha
    %endif

    %include "frame.asm"

    %ifndef COM
    popa
    ret

    %include "tiny/includes.asm"

    %endif
```

For COM builds, your code is included inline within the main loop. You can write directly to VGA video memory through the `es` segment, which is set to `0xA000` before your code executes by default.

For 32-bit builds, you write to an `image` buffer defined in the `.bss` section. The C runtime calls this function each frame and transfers the buffer contents to the display.

Your `frame.asm` contains the actual rendering algorithm. The framework defines platform conditionals (`COM`, `DOS`) that you use to handle register and memory access differences:

```asm
; Example from tinywork/example/core/frame.asm
set_registers:
    %ifdef COM
    xor di, di                    ; 16-bit: write to es:di (VGA memory)
    %else
    mov edi, image                ; 32-bit: write to buffer
    %endif

; Common demo code
draw:
    mov bx, SCREEN_HEIGHT
.draw_loop:
    mov cx, SCREEN_WIDTH
    mov ax, bx
    shr ax, 1
    rep stosb                     ; Works on both platforms
    dec bx
    jnz .draw_loop
```

The framework preserves all registers across the function call on 32-bit targets.

Note that for 32-bit builds, the wrapper includes `tiny/includes.asm` after the frame function definition. This mechanism allows you to add additional code beyond the frame rendering. See the next section for details.

### Additional Code (`tiny/includes.asm`)

The `includes.asm` file lets you include additional assembly files that your effect needs beyond the main frame and palette code. Use `%include` directives to bring in helper files with functions, data sections, or variable declarations.

The framework includes your `include.asm` file through `tiny/includes.asm`:

```asm
%include "tiny/consts.asm"
%include "includes.asm"
```

Example usage in `includes.asm`:

```asm
%include "data.asm"       ; Data sections
%include "vars.asm"       ; Variable declarations
```

For COM builds, `tiny/includes.asm` is included at the end of `main.asm`. For 32-bit builds, it's included within `tiny/frame.asm` after the frame function definition, making it part of the frame compilation unit. This means that **included source files are bound to `frame.asm`**.

### Project Metadata (`info.asm`)

The `info.asm` file is not used during compilation. Instead, it's prepended to the final flattened assembly code generated by `make code`. Use this file to document your project with comments containing the title, author information, maybe some implementation details if you wish, or even ASCII art.

Example:

```asm
; =================
; My Glamorous Demo
;     by X / Y
; =================
```

This file should contain only comments. Any executable code or directives will appear at the top of the flattened output but won't affect the binaries compiled through `make com`.

## C Runtime

For 32-bit builds (Linux, Windows, DOS executables), _TinyWork_ provides a C runtime with the entry point, main loop, input handling, and video subsystem. COM builds are pure assembly and don't use any C code.

The runtime calls your assembly `initialize()` and `frame()` functions and handles platform-specific details like SDL2 on Linux/Windows or VGA on DOS.

### Framework Structure

The C runtime consists of three components:

```
tinywork/
├── main.c              # Entry point for 32-bit builds
├── runtime/
│   ├── runtime.h       # Runtime interface
│   └── runtime.c       # Main loop and input handling
└── video/
    ├── video.h         # Video subsystem interface
    ├── video_sdl.c     # SDL2 implementation (Linux/Windows)
    └── video_dos.c     # VGA implementation (DOS executable)
```

### Main Entry Point (`main.c`)

The default entry point for 32-bit builds follows a simple initialization sequence:

```c
int main(int, char **) {
    if (video_init() != 0) {
        return 1;
    }

    initialize();
    run();
    video_cleanup();
    return 0;
}
```

This initializes the video subsystem, calls your assembly `initialize()` and `frame()` functions through `run()`, then cleans up on exit. Override this by setting `MAIN_C` in your Makefile if you need custom startup logic.

### Runtime (`runtime/`)

The runtime provides the `run()` function that implements the main loop and calls your `frame()` function repeatedly. The implementation differs between DOS (DJGPP) and SDL2 platforms:

**DOS (DJGPP)**: Runs at 60 FPS with busy-wait timing, polls keyboard for exit, calls `frame()` which writes directly to VGA memory.

**SDL2 (Linux/Windows)**: Processes SDL events (quit, keyboard, window resize), handles fullscreen toggling (F11/F key) and escape key behavior, calls `frame()` which renders to the `image` buffer that's then transferred to the window.

The runtime expects your assembly code to provide `initialize()` (possibly empty) and `frame()` functions and an `image` buffer. These are defined in your assembly files through the framework's wrappers.

### Video Subsystem (`video/`)

The video subsystem abstracts platform differences in display and palette handling. Both implementations require your assembly code to provide `set_palette()` and the `image` buffer.

**`video_sdl.c`** (Linux/Windows): Creates an SDL2 window, calls `set_palette()` to populate a palette buffer, converts indexed color from the `image` buffer to RGB for display.

**`video_dos.c`** (DOS executable): Sets VGA mode 13h, calls `set_palette()` which writes to VGA palette registers, copies the `image` buffer to VGA memory each frame.

The key difference is that DOS writes directly to hardware while SDL2 uses intermediate buffers. The video subsystem is selected automatically based on the build target.

## Code Generation

The `make code` target generates a flattened assembly file from your project by resolving all `%include` directives, inlining macros, and removing framework-specific markers. This produces a single-file `.asm` suitable for final submission or sharing.

```bash
make code
```

The flattener processes `MAIN_ASM` (typically `tinywork/main.asm`) and outputs a complete assembly file containing all your effect code with framework infrastructure removed.

### Processing Pipeline

The code generator performs several transformations:

**Include Resolution**: Recursively processes all `%include` directives, distinguishing between framework files (prefixed with `tiny/`) and project files. Framework files are located in the `TINYWORK_DIR`, while project files are located in the `SOURCE_DIR`. During this step, all 32-bit code is removed, including target-specific preprocessor directives.

**Include Guard Deduplication**: Detects `%ifndef`/`%define` guard patterns and skips duplicate guard blocks when files are included multiple times. Guard `%define` statements are removed from output.

**Macro Inlining**: Expands specified macros inline, replacing invocations with their definitions.

**Label Cleanup**: Removes double-underscore labels (`__initialize:`, `__frame:`) used as framework markers. Removes optional labels (`initialize:`, `set_palette:`) if they contain no code.

**Output Formatting**: Strips `global` directives, removes redundant empty lines, and ensures consistent spacing.

**Header Prepending**: Adds `info.asm` contents to the top of the output for project metadata and documentation.

### Configuration

Three Makefile variables control the code generation:

**`FLATTENED_ASM`** — Output filename (default: `$(PROJECT_NAME).asm`)

**`FLATTEN_MACROS`** — Space-separated macro names to inline (default: `PALETTE_OUT`)

**`FLATTEN_OPTIONAL_LABELS`** — Space-separated labels to remove if empty (default: `initialize set_palette`)

Example customization in your Makefile:

```makefile
FLATTENED_ASM = submission.asm
FLATTEN_MACROS = PALETTE_OUT PIXEL_WRITE VSYNC_WAIT
FLATTEN_OPTIONAL_LABELS = initialize set_palette cleanup

include $(TINYWORK_DIR)/Makefile.inc
```

The generated file is a standalone assembly source that can be assembled directly with NASM to produce the same COM binary as `make com`.

## Advanced Configuration

The framework provides optional Makefile variables for customizing build directories, source files, and compiler flags. Set these in your project's Makefile before including `Makefile.inc`.

### Build Directories

**`BIN_DIR`** — Output directory for executables (default: `bin`)

**`BUILD_DIR`** — Directory for intermediate build artifacts (default: `build`)

Example:

```makefile
PROJECT_NAME = demo
TINYWORK_DIR = tinywork
SOURCE_DIR = core

BIN_DIR = output
BUILD_DIR = .build

include $(TINYWORK_DIR)/Makefile.inc
```

### Source Files

**`MAIN_C`** — Entry point for 32-bit builds (default: `tinywork/main.c`)

**`MAIN_ASM`** — Entry point for COM builds (default: `tinywork/main.asm`)

Override these when you need custom initialization logic or entry points:

```makefile
MAIN_C = src/custom_main.c
MAIN_ASM = src/custom_com.asm

include $(TINYWORK_DIR)/Makefile.inc
```

### Compiler Flags

**`ASMFLAGS_OPTIONS`** — Custom NASM flags applied to all assembly targets

**`CFLAGS_EXTRA`** — Additional C compiler flags for all 32-bit builds

**`EXTRA_INCLUDES`** — Additional include directories for C compilation

Example:

```makefile
ASMFLAGS_OPTIONS = -DUSE_CUSTOM_PALETTE -DENABLE_EFFECTS
CFLAGS_EXTRA = -O3 -march=i686
EXTRA_INCLUDES = -I../shared -I../libs

include $(TINYWORK_DIR)/Makefile.inc
```

The framework automatically combines these with platform-specific flags. Assembly flags affect both COM and 32-bit assembly compilation.




