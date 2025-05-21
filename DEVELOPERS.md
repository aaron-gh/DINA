# DINA Developer Guide

This document provides information for developers contributing to DINA.

## Architecture Overview

DINA has been restructured from a monolithic application into a modular architecture. This approach improves maintainability, readability, and makes the codebase more approachable for new contributors.

### Module Organization

The codebase is organized into the following modules:

- **Core Modules** (`core/`): Central components and entry points
  - `dina.c` - Main entry point and core functionality
  - `dina.h` - Core declarations and structures
  - `config.c` - Configuration loading and validation
  - `event.c` - Event handling and dispatching

- **Window Management** (`wm/`): Window and workspace handling
  - `window.c` - Window handling and manipulation
  - `window.h` - Window-related structures and functions
  - `monitor.c` - Monitor detection and management
  - `monitor.h` - Monitor-related structures
  - `tag.c` - Tag (workspace) management
  - `tag.h` - Tag-related structures and functions
  - `rules.c` - Window rule application

- **UI and Drawing** (`ui/`): Minimal UI components
  - `drw.c` - Drawing functions (from original dwm)
  - `drw.h` - Drawing declarations

- **Accessibility** (`a11y/`): Accessibility features
  - `notify.c` - Notifications and audio feedback
  - `notify.h` - Audio notification interfaces
  - `workspace_memory.c` - Workspace persistence and memory
  - `workspace_memory.h` - Workspace memory interfaces

- **Utility Functions** (`util/`): General purpose utilities
  - `util.c` - General utility functions
  - `util.h` - Utility declarations

## Building DINA

DINA uses a simple Makefile-based build system:

```sh
# Regular build
make

# Install
sudo make install

# User configuration installation
make userinstall

# Clean build artifacts
make clean
```

## Code Style

DINA follows a consistent code style:

- **Comments**: Use Doxygen-style comments for functions and structures
- **Indentation**: 4-space indentation (no tabs)
- **Function Declarations**: Return type on a separate line
- **Error Handling**: Check all return values and report errors
- **Memory Management**: Free all allocated memory
- **Naming**: Use snake_case for function names and variables

Example:

```c
/**
 * @brief Example function description
 * 
 * Detailed description of what the function does
 * 
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 */
int
example_function(int param1, char *param2)
{
    /* Function implementation */
    return 0;
}
```

## Adding Features

When adding new features to DINA:

1. **Identify the module**: Determine which module your feature belongs in
2. **Create/modify header files**: Define public interfaces in header files
3. **Implement functionality**: Create implementation files
4. **Update build system**: Add new files to the appropriate Makefile variables
5. **Document your changes**: Add comments and update documentation
6. **Test your changes**: Ensure your feature works as expected

## Accessibility First

DINA is designed with blind users in mind. Always consider accessibility when making changes:

- All features should work without visual feedback
- Provide audio notifications for important events
- Focus on keyboard-based interaction
- Ensure screen reader compatibility
- Follow the principle of least surprise

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Debugging

DINA includes debugging infrastructure:

- Use the `debug()` function for debug messages
- Enable debug mode by defining the `DEBUG` macro
- Debug logs are written to `dina-debug.log`

Example:

```c
debug("Window %lu created with class %s", window_id, class_name);
```