<div align="center">

  <img src="assets/kotha_logo_final.png" alt="Kotha Logo" width="200" />
  <br/>
  <br/>

  # 🇧🇩 Kotha (কথা) Programming Language
  
  **A Powerful "Banglish" Programming Language.**
  <br/>
  
  [![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/labonysur-cloud/Kotha0.2)
  [![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
  [![Version](https://img.shields.io/badge/version-0.2-orange)](https://github.com/labonysur-cloud/Kotha0.2)
  [![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux-lightgrey)]()
  
  [Features](#-features) • [Installation](#-installation) • [Usage](#-usage) • [Documentation](#-documentation) • [Examples](#-examples)

</div>

---

## 🚀 Introduction

**Kotha** is a unique programming language designed to bridge the gap between Bengali culture and modern computer science. By using "Banglish" (Bengali words written in English script) keywords, it makes coding more accessible, while retaining the power of C-style languages.

It features a custom compiler, a high-performance register-based Virtual Machine (VM), and a rich set of standard libraries.

<div align="center">
  <h3>✨ The Kotha IDE ✨</h3>
  <img src="screenshots/kotha-ide-demo.png" alt="Kotha IDE Screenshot" width="800" style="border-radius: 10px; box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);" />
</div>

---

## 🌟 Supported Features

Kotha 0.2 currently supports the following fully functional features:

### 🛠 Core Language
- **Banglish Syntax**: Familiar keywords like `purno` (int), `doshomik` (float), `bornona` (string), `dekhaw` (print), `jodi` (if), `noyto` (else), `jotokkhon` (while).
- **Data Types**: Support for Integers (`purno`), Floats (`doshomik`), Strings (`bornona`), and Booleans (`sotyo_mittha`).
- **C-Style Declarations**: Multiple variable declarations in one line: `purno x, y, z = 10;`
- **String Concatenation**: Use `+` operator to concatenate strings with automatic type conversion: `"Age: " + age`
- **Type Casting**: 12 conversion functions for explicit type conversions (e.g., `purno_to_doshomik()`, `bornona_to_purno()`)
- **Type Inspection**: `typeof()` operator for runtime type checking
- **Control Flow**: `if-else` conditionals and `while` loops.
- **Functions**: Define and call functions with arguments and return values.
- **Virtual Machine**: Code is compiled to bytecode and executed on a custom high-performance VM.

### 📚 Standard Library
- **String Library**: String manipulation functions like `length`, `concatenate`, `reverse`, etc.
- **Math Library**: Common mathematical functions including `sin`, `cos`, `power`, `sqrt`.
- **File I/O**: Read from and write to files directly from Kotha.
- **Array Support**: Create and manipulate arrays.

### 💻 Development Tools
- **Kotha IDE**: A web-based Integrated Development Environment with syntax highlighting and file management.
- **REPL**: interactive Read-Eval-Print Loop for quick coding sessions.
- **CLI**: Command-line interface for compiling and running scripts.

---

## 📦 Installation

To get started with Kotha, clone the repository and build the compiler:

```bash
# Clone the repository
git clone https://github.com/labonysur-cloud/Kotha0.2.git

# Navigate to the project directory
cd Kotha0.2

# Build the compiler and tools
make
```

---

## 🖥 Usage

### Running the IDE
Experience Kotha through our modern web-based IDE:

```bash
# Start the IDE server
./start_ide.sh
```
Then open your browser at `http://localhost:8081`.

### Command Line
You can also compile and run Kotha files directly from the terminal:

```bash
# Run a Kotha program
./run_kotha.sh your_program.kotha
```

### Interactive REPL
To start the interactive shell:
```bash
./kotha/kotha
```
(Running without arguments starts the REPL)

---

## 🏗 System Architecture

The Kotha compiler follows a robust multi-stage pipeline, transforming Banglish source code into executable actions via a custom Virtual Machine.

<div align="center">
  <img src="assets/system_architecture.png" alt="Kotha System Architecture" width="800" style="border-radius: 10px; box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);" />
</div>

**Pipeline Flow:**
1.  **Lexer/Parser**: Tokenizes Banglish code and builds an AST.
2.  **IR Generator**: Converts the AST into 3-Address Code (Intermediate Representation).
3.  **Optimizer**: Optimizes the IR for better performance.
4.  **Code Generator**: Translates IR into Bytecode for the VM.
5.  **Virtual Machine**: Executes the bytecode, managing stack and memory.

---

## 📂 Project Structure

```bash
Kotha0.2/
├── assets/                 # Images for documentation
├── kotha/                  # Core compiler source code
│   ├── kotha               # Compiled executable
│   ├── parser.y            # Bison parser grammar
│   ├── lexer.l             # Flex lexer definitions
│   ├── vm.c                # Virtual Machine implementation
│   ├── ir.c                # Intermediate Representation generation
│   ├── codegen_vm.c        # Bytecode generation
│   └── *_lib.c             # Standard libraries (math, string, file, array)
├── kotha-ide/              # Web-based IDE
│   ├── examples/           # Sample Kotha programs
│   ├── public/             # Frontend assets (HTML, CSS, JS)
│   └── server.py           # Backend server (Python)
├── PROJECT_REPORT.md       # Detailed technical documentation
├── README.md               # Main documentation
└── run_kotha.sh            # Helper script to execute programs
```

---

## 📝 Examples

Here is a glimpse of working Kotha code:

<details open>
<summary><b>Hello World</b></summary>

```c
main function {
    dekhaw("Hello, Kotha World!");
}
```
</details>

<details>
<summary><b>Variables & Logic</b></summary>

```c
main function {
    dhoro a = 10;
    dhoro b = 20;
    
    jodi (a < b) {
        dekhaw("B is larger");
    } nabor {
        dekhaw("A is larger");
    }
}
```
</details>

<details>
<summary><b>String Manipulation</b></summary>

```c
main function {
    dhoro name = "Kotha";
    dekhaw("Length is: ");
    // Simulated length check (using internal lib)
    dekhaw(name); 
}
```
</details>

---

## 📚 Documentation

For more detailed information, check out the following resources included in the repository:

- 📄 **[Project Report](PROJECT_REPORT.md)**: Architectural overview.
- 📁 **[Examples](kotha-ide/examples/)**: A collection of working Kotha programs.

---

## 🤝 Contributing

We welcome contributions! Please feel free to submit a Pull Request.

1. Fork the Project
2. Create your Feature Branch
3. Commit your Changes
4. Push to the Branch
5. Open a Pull Request

---

<div align="center">
  Developed with  by <b>Labony Sur</b>
  <br/>
  <i>Empowering coders in their native tongue.</i>
</div>
