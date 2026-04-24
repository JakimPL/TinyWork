# TinyWork Framework - Example Project

This directory contains a minimal example demonstrating how to use the TinyWork framework for cross-platform 256-byte demos and size-coded productions.

## Files

- **`Makefile`** - Minimal setup example
- **`main.c`** - C entry point using framework runtime and video APIs
- **`demo.asm`** - Minimal 32-bit assembly implementing `draw()` function
- **`main.asm`** - Standalone COM file example (256-byte DOS demo)

## Quick Start

```bash
# Build for your platform
make

# Build specific targets
make linux       # Linux with SDL2
make windows     # Windows with MinGW32 + SDL2
make dos         # DOS with DJGPP
make com         # Standalone COM file

# Run
make run

# Build with debug symbols
make DEBUG=1 all
```

## Integration Guide

See `Makefile` in this directory for a complete working example.

### Minimal Setup

Your project needs:

1. Define `PROJECT_NAME` and `FRAMEWORK_DIR`
2. Include `$(FRAMEWORK_DIR)/Makefile.inc`
3. Optionally override variables like `ASMFLAGS_OPTIONS`, `CFLAGS`, `BIN_DIR`

### Framework Contract

Your code must provide:

- `void draw(void)` - Render one frame into image buffer
- `unsigned char image[BUFFER_SIZE]` - 65536-byte frame buffer (NOTE: framework will provide this in future)

The framework provides:

- Video API: `video_init()`, `video_update_from_buffer()`, `video_present()`, `video_cleanup()`
- Runtime API: `run(frame_callback)`

## Directory Structure

Recommended project layout when using TinyWork as submodule:

```
your-demo/
├── Makefile           # Your project config + include framework
├── main.c             # Your C entry point
├── main.asm           # Your COM source (optional)
├── demo.asm           # Your 32-bit assembly (optional)
├── framework/         # Git submodule → TinyWork
│   ├── Makefile.inc
│   ├── video/
│   ├── runtime/
│   └── asm/
├── bin/               # Output directory (created by make)
└── build/             # Object files (created by make)
```

## See Also

- Spirward demo for an example
- Framework API reference in `../runtime/` and `../video/` headers
