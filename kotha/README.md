# 🐯 Kotha - The Royal Bengal Tiger of Programming Languages

![Kotha Logo](kotha_logo.jpg)

**Kotha** is a Banglish (Bengali + English) programming language with a complete compiler toolchain and a retro 90s-style IDE. Designed for educational purposes and compiler design courses, Kotha demonstrates the full compilation pipeline from lexical analysis to executable code generation.

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 📋 Table of Contents
- [Features](#-features)
- [Installation](#-installation)
- [Quick Start](#-quick-start)
- [Language Syntax](#-language-syntax)
- [CLI Usage](#-cli-usage)
- [IDE Usage](#-ide-usage)
- [Examples](#-examples)
- [Project Structure](#-project-structure)
- [For CSE314 Course](#-for-cse314-course)

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## ✨ Features

### Compiler Features
- ✅ **Full Transpiler**: Kotha → C → Executable
- ✅ **Lexical Analysis**: Flex-based lexer with regex patterns
- ✅ **Syntax Analysis**: Bison-based parser with CFG
- ✅ **Code Generation**: Syntax-directed translation to C
- ✅ **Type Support**: Integers, floats, strings, arrays (1D & 2D)

### Language Features
- **Variables**: `dhoro` (mutable), `sthir` (constant)
- **Control Flow**: `jodi` (if), `noyto` (else if), `othoba` (else)
- **Loops**: `cholbe` (for), `jotokkhon` (while)
- **Functions**: `kaj` (function), `void kaj` (void function), `ferot` (return)
- **Arrays**: `talika` (1D and 2D arrays)
- **I/O**: `dekhaw` (print), `nao` (input)
- **Operators**: Arithmetic, relational, logical, compound assignments
- **Game Features**: `random()`, `porishkar()` (clear screen), `wait()` (delay)

### Development Tools
- **CLI**: Modern command-line interface with colorful output
- **IDE**: Windows 95-style web-based IDE with live compilation
- **Games**: 4 playable games (Tic-Tac-Toe, Dice, Snake, Number Guess)

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 🚀 Installation

### Prerequisites
- **Flex** (lexer generator)
- **Bison** (parser generator)
- **GCC** (C compiler)
- **Make**
- **Python 3** (for IDE)

### Build the Compiler
```bash
cd kotha
make
```

This generates the `kotha` compiler binary.

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## ⚡ Quick Start

### 1. Write Your First Program
Create `hello.kotha`:
```kotha
main function {
    dekhaw("Hello, Kotha!");
    dekhaw("The Royal Bengal Tiger of Programming!");
}
```

### 2. Run It
```bash
./kotha-cli run hello.kotha
```

### 3. Or Build an Executable
```bash
./kotha-cli build hello.kotha
./hello
```

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 📖 Language Syntax

### Variables
```kotha
dhoro x = 10;           // Mutable variable
sthir PI = 3;           // Constant
dhoro name = "Tiger";   // String
```

### Control Flow
```kotha
jodi (x > 10) {
    dekhaw("Greater");
} noyto (x == 10) {
    dekhaw("Equal");
} othoba {
    dekhaw("Less");
}
```

### Loops
```kotha
// For loop
cholbe (i theke 1 porjonto 5) {
    dekhaw(i);
}

// While loop
dhoro count = 0;
jotokkhon (count < 5) {
    dekhaw(count);
    count = count + 1;
}
```

### Functions
```kotha
kaj add(a, b) {
    ferot a + b;
}

void kaj greet() {
    dekhaw("Hello!");
}

main function {
    dhoro sum = add(5, 3);
    dekhaw(sum);
    greet();
}
```

### Arrays
```kotha
talika numbers[5];
numbers[0] = 10;
numbers[1] = 20;
dekhaw(numbers[0]);

// 2D Arrays
talika matrix[3][3];
matrix[0][0] = 1;
```

### Keyword Reference

| Kotha | English | Usage |
|-------|---------|-------|
| `dhoro` | declare | Mutable variable |
| `sthir` | constant | Constant variable |
| `jodi` | if | If statement |
| `noyto` | else if | Else if clause |
| `othoba` | else | Else clause |
| `cholbe` | for | For loop |
| `theke` | from | Loop start |
| `porjonto` | to | Loop end |
| `jotokkhon` | while | While loop |
| `kaj` | function | Function declaration |
| `void` | void | Void function |
| `ferot` | return | Return statement |
| `talika` | array | Array declaration |
| `dekhaw` | print | Print output |
| `nao` | input | Read input |
| `random` | random | Random number |
| `porishkar` | clear | Clear screen |
| `wait` | wait | Delay/sleep |

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 💻 CLI Usage

### Commands

```bash
kotha run <file>        # Compile and run
kotha build <file>      # Build executable
kotha compile <file>    # Transpile to C only
kotha new <name>        # Create new project
kotha game <name>       # Play built-in games
kotha test              # Run test suite
kotha clean             # Clean generated files
kotha version           # Show version
kotha help              # Show help
```

### Examples
```bash
# Quick run
kotha run program.kotha

# Build for distribution
kotha build myapp.kotha
./myapp

# Play games
kotha game tictactoe
kotha game snake
kotha game dice
kotha game guess
```

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 🎨 IDE Usage

### Launch the IDE
```bash
python3 kotha-ide/server.py
```

Then open: **http://localhost:8080**

### IDE Features
- **90s Retro UI**: Windows 95 aesthetic with teal background
- **Live Compilation**: Run code instantly with the ▶ button
- **File Management**: New, Save buttons
- **Keyboard Shortcuts**:
  - `Ctrl/Cmd + Enter`: Run code
  - `Ctrl/Cmd + S`: Save file
  - `Ctrl/Cmd + N`: New file
- **Example Snippets**: Click "Help" for code examples
- **Console Output**: Green terminal-style output window

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 📚 Examples

### Example 1: Variables and Constants
```kotha
main function {
    sthir YEAR = 1995;
    dhoro age = 30;
    dhoro birth_year = YEAR - age;
    
    dekhaw("Birth year:");
    dekhaw(birth_year);
}
```

### Example 2: Fibonacci Sequence
```kotha
kaj fibonacci(n) {
    jodi (n <= 1) {
        ferot n;
    }
    ferot fibonacci(n - 1) + fibonacci(n - 2);
}

main function {
    cholbe (i theke 0 porjonto 10) {
        dekhaw(fibonacci(i));
    }
}
```

### Example 3: Array Operations
```kotha
main function {
    talika numbers[5];
    
    cholbe (i theke 0 porjonto 4) {
        numbers[i] = i * 10;
    }
    
    cholbe (i theke 0 porjonto 4) {
        dekhaw(numbers[i]);
    }
}
```

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 📁 Project Structure

```
kotha/
├── lexer.l                 # Flex lexer specification
├── parser.y                # Bison parser specification
├── Makefile                # Build configuration
├── kotha                   # Compiler binary
├── kotha-cli               # CLI wrapper script
├── README.md               # This file
├── CLI_GUIDE.md            # CLI documentation
├── GAMES_README.md         # Games documentation
├── game_*.kotha            # Game source files
├── game_*                  # Game executables
├── test_*.kotha            # Test files
└── kotha-ide/              # Web-based IDE
    ├── server.py           # Python backend
    └── public/
        ├── index.html      # IDE interface
        ├── style.css       # 90s retro styling
        └── script.js       # IDE logic
```

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 🎓 For CSE314 Course

This project demonstrates:
- **Lexical Analysis**: Regular expressions and token recognition
- **Syntax Analysis**: Context-free grammars and parsing
- **Semantic Analysis**: Symbol tables and type checking (basic)
- **Code Generation**: Transpilation and executable generation
- **Compiler Tools**: Flex and Bison usage

### Course Outcomes Mapping
- **CO1**: Lexer implementation with regex
- **CO2**: Parser with CFG rules
- **CO3**: Symbol table and scope management
- **CO4**: C code generation
- **CO5**: Industry-standard tools (Flex, Bison, GCC)

See `project_report.md` for detailed CO/PO mapping.

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 🛠️ Development

### Rebuild Compiler
```bash
make clean
make
```

### Run Tests
```bash
./kotha-cli test
```

### Add New Features
1. Update `lexer.l` for new tokens
2. Update `parser.y` for new grammar rules
3. Rebuild with `make`

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 📝 License

Educational use - Compiler Design Course (CSE314)

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


## 🐯 About

**Kotha** means "word" or "language" in Bengali. This project celebrates Bangladesh's rich linguistic heritage while teaching fundamental compiler design concepts.

**The Royal Bengal Tiger** is the national animal of Bangladesh, symbolizing strength and elegance - qualities we've built into this language.

---

## 👥 Team

- **Labony Sur** - [@labonysur-cloud](https://github.com/labonysur-cloud)
- **Aupurba Sarker** - [@aupurbasarker-cloud](https://github.com/aupurbasarker-cloud)


**Made with 🧡 for CSE314 - Compiler Design Lab**

*Let your code roar!* 🐯
