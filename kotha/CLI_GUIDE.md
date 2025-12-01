# Kotha CLI - User Guide

## Overview
The Kotha CLI provides a user-friendly interface for the Kotha programming language, similar to modern languages like Python, Node.js, or Go.

## Installation

Add the CLI to your PATH:
```bash
# Option 1: Create symlink in /usr/local/bin
sudo ln -s $(pwd)/kotha-cli /usr/local/bin/kotha

# Option 2: Add to PATH in ~/.bashrc or ~/.zshrc
export PATH="$PATH:/path/to/kotha"
```

## Commands

### `kotha run <file>`
Compile and run a Kotha program immediately.

```bash
kotha run hello.kotha
```

This will:
1. Transpile Kotha to C
2. Compile C to executable
3. Run the program
4. Clean up temporary files

### `kotha build <file>`
Build an executable from Kotha code.

```bash
kotha build myapp.kotha
```

Creates:
- `myapp.c` - Generated C code
- `myapp` - Executable binary

### `kotha compile <file>`
Transpile Kotha to C only (no compilation).

```bash
kotha compile program.kotha
```

Generates: `program.c`

### `kotha new <name>`
Create a new Kotha project.

```bash
kotha new myproject
cd myproject
kotha run main.kotha
```

Creates:
- `myproject/main.kotha` - Main program file
- `myproject/README.md` - Project documentation

### `kotha game <name>`
Run built-in games.

```bash
kotha game tictactoe
kotha game dice
kotha game snake
kotha game guess
```

### `kotha test`
Run all test cases.

```bash
kotha test
```

### `kotha clean`
Remove generated files.

```bash
kotha clean
```

### `kotha version`
Show version information.

```bash
kotha version
```

### `kotha help`
Show help message.

```bash
kotha help
```

## Examples

### Quick Start
```bash
# Create new project
kotha new hello
cd hello

# Edit main.kotha
# ... add your code ...

# Run it
kotha run main.kotha
```

### Build and Distribute
```bash
# Build executable
kotha build myapp.kotha

# Run the executable
./myapp

# Distribute the executable (no Kotha needed)
```

### Development Workflow
```bash
# Write code
vim calculator.kotha

# Test quickly
kotha run calculator.kotha

# Build final version
kotha build calculator.kotha

# Clean up
kotha clean
```

## Tips

1. **Fast Testing**: Use `kotha run` for quick iterations
2. **Production Builds**: Use `kotha build` for final executables
3. **Debugging**: Use `kotha compile` to inspect generated C code
4. **Learning**: Use `kotha game` to see example programs

## Workflow Comparison

### Traditional Way
```bash
./kotha < program.kotha > program.c
gcc program.c -o program
./program
rm program.c program
```

### With CLI
```bash
kotha run program.kotha
```

Much simpler! 🎉
