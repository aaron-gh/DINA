# DINA Architecture

This document outlines the architecture of DINA (DINA Is Not Aesthetics), a window manager for blind users.

## Modular Architecture

DINA's code is organized into the following modules:

### Core Modules
- `core/dina.c` - Main entry point and core functionality
- `core/dina.h` - Core declarations and structures
- `core/config.c` - Configuration loading and validation
- `core/event.c` - Event handling and dispatching

### Window Management
- `wm/window.c` - Window handling and manipulation
- `wm/window.h` - Window-related structures and functions
- `wm/monitor.c` - Monitor detection and management
- `wm/monitor.h` - Monitor-related structures
- `wm/tag.c` - Tag (workspace) management
- `wm/tag.h` - Tag-related structures and functions
- `wm/rules.c` - Window rule application

### UI and Drawing
- `ui/drw.c` - Drawing functions (from original dwm)
- `ui/drw.h` - Drawing declarations

### Accessibility
- `a11y/notify.c` - Notifications and audio feedback
- `a11y/notify.h` - Audio notification interfaces
- `a11y/workspace_memory.c` - Workspace persistence and memory
- `a11y/workspace_memory.h` - Workspace memory interfaces

### Utility Functions
- `util/util.c` - General utility functions
- `util/util.h` - Utility declarations

## Design Principles

1. **Modularity** - Each module focuses on a specific concern
2. **Accessibility** - All features prioritize screen reader compatibility
3. **Simplicity** - Keep interfaces clean and straightforward
4. **Documentation** - Extensive comments and explanations

## Build System

The build system uses a modular Makefile structure:
- `Makefile` - Main Makefile
- `config.mk` - Configuration variables

## Configuration

User configuration:
- `config.def.h` - Default configuration
- `config.h` - User-specific configuration