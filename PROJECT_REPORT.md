# Kotha Compiler Project Report

**Project Name:** Kotha Programming Language Compiler & IDE  
**Version:** 0.2.0  
**Date:** December 08, 2025  

---

## 1. Introduction

Language is the primary medium of human expression, yet in the realm of computer science, the majority of programming languages are rooted in English. This creates a significant barrier for non-English speakers, particularly in regions like Bangladesh, where native language proficiency far exceeds English fluency. **Kotha** is a stack-based, statically-typed programming language designed with Bengali syntax to bridge this gap. 

This project entails the development of a complete compiler toolchain for Kotha, including a Lexer, Parser, Intermediate Representation (IR) generator, and a custom Virtual Machine (VM) with Garbage Collection. Additionally, to facilitate ease of use, a web-based Integrated Development Environment (IDE) has been developed, allowing users to write, debug, and execute Kotha code seamlessly. This report details the design, implementation, architecture, and future trajectory of the Kotha project.

## 2. Background

Compiler design is a complex domain involving the translation of high-level human-readable code into low-level machine instructions. Traditional compilers (like GCC or Clang) and interpreters (like Python) have evolved over decades. 

The motivation behind Kotha addresses two main areas:
1.  **Educational:** To provide a localized learning tool for computer science students in Bengali-speaking regions.
2.  **Technical Exploration:** To build a full-featured compiler from scratch, implementing core concepts such as 3-address code generation, stack machine architecture, and manual memory management logic.

Existing Bengali programming languages often function as simple transpilers (mapping Bengali keywords directly to C/C++). Kotha distinguishes itself by being a full compiler with its own bytecode and virtual machine, offering greater control over execution and memory.

## 3. Method

The development of the Kotha project utilized a robust set of standard industry tools and languages:
*   **Core Systems:** C Language (C99 Standard) for high performance and low-level memory control.
*   **Lexical Analysis:** Flex (Fast Lexical Analyzer Generator) for tokenizing Bengali input.
*   **Parsing:** Bison (GNU Parser Generator) for defining the context-free grammar.
*   **Scripting & IDE Backend:** Python 3 for the local server and automation scripts.
*   **IDE Frontend:** HTML5, CSS3, and JavaScript for a responsive web-based editor.
*   **Build System:** Makefiles for automated compilation and linking.

## 4. Methodology & System Architecture

The Kotha compiler follows a multi-stage pipeline architecture, transforming raw source code into executable actions.

### 4.1 System Architecture Overview

The system is creating using a modular pipeline design. The flow of data proceeds as follows:

```mermaid
graph TD
    Source[Kotha Source Code (.kotha)] --> Lexer
    Lexer -- Tokens --> Parser
    Parser -- AST --> Semantic[Semantic Analysis]
    Semantic --> IR[IR Generator]
    IR -- 3-Address Code --> Optimizer
    Optimizer --> CodeGen[Code Generator]
    CodeGen --> Exec{Execution Mode}
    Exec -- "Flag: --vm" --> VM[Virtual Machine]
    Exec -- "Flag: default" --> CTrans[C Transpiler]
    VM --> Output
    CTrans --> GCC[GCC Compiler] --> Binary[Native Binary]
```

### 4.2 Detailed Component Implementation

#### A. Lexical Analysis (Lexer)
*   **File:** `kotha/lexer.l`
*   **Function:** Reads the input character stream and groups them into meaningful tokens.
*   **Details:** Handles Bengali keywords (e.g., `dhoro`, `jodi`, `ghuro`), operators, and literals. It uses regular expressions to identify identifiers, numbers, and string literals, discarding whitespace and comments (`//`).

#### B. Syntax Analysis (Parser)
*   **File:** `kotha/parser.y`
*   **Function:** Checks the stream of tokens against the grammatical rules of the language.
*   **Details:** Implements a Context-Free Grammar (CFG) using Bison. It constructs an **Abstract Syntax Tree (AST)** used for further processing. Error handling is integrated here to report syntax errors with line numbers.

#### C. Abstract Syntax Tree (AST)
*   **Files:** `kotha/ast.c`, `kotha/ast.h`
*   **Function:** Represents the hierarchical structure of the program.
*   **Details:** Nodes represent constructs like `NODE_VAR_DECL`, `NODE_IF`, `NODE_WHILE`, etc. This tree is the central data structure used by both the interpreter and the code generator.

#### D. Intermediate Representation (IR)
*   **Files:** `kotha/ir.c`, `kotha/ir.h`
*   **Function:** Converts the AST into a linear sequence of instructions (3-Address Code).
*   **Details:** This simplifies the complex tree structure into instructions like `IR_ADD t1, a, b` or `IR_LABEL L1`. This abstraction makes it easier to optimize code and generate machine-specific bytecode.

#### E. Virtual Machine (VM)
*   **Files:** `kotha/vm.c`, `kotha/vm.h`, `kotha/codegen_vm.c`
*   **Function:** Executes the compiled bytecode.
*   **Architecture:**
    *   **Stack-Based:** Operands are pushed/popped from a data stack.
    *   **Heap with GC:** Implements a Mark-and-Sweep Garbage Collector to manage dynamic memory automatically.
    *   **Instruction Set:** Supports 265+ opcodes including arithmetic, logic, control flow, and function calls.

#### F. Standard Libraries
To make the language useful, a comprehensive standard library was implemented in C:
*   **Math (`math_lib.c`):** Trigonometry, logarithms, and basic arithmetic.
*   **String (`string_lib.c`):** Manipulation functions like `kotha_strlen`, `kotha_strcat`, `kotha_substr`, etc.
*   **Array (`array_lib.c`):** Dynamic array support with push/pop and sorting capabilities.
*   **File I/O (`file_io.c`):** Reading and writing files directly from Kotha programs.

#### G. Integrated Development Environment (IDE)
*   **Frontend:** A web interface providing syntax highlighting and code editing features.
*   **Backend (`server.py`):** Captures code from the frontend, saves it to a temporary file, invokes the Kotha compiler, and streams the output (Standard Output/Error) back to the user interface in real-time.

## 5. Discussion

### Completed Features
*   [x] Full compilation pipeline (Lexer -> Parser -> AST -> IR -> VM).
*   [x] Native C transpilation support for high-performance executable generation.
*   [x] Custom Virtual Machine with extensive opcode set.
*   [x] Control structures: `jodi` (if), `othoba` (else), `jotokkhon` (while), `cholbe` (for).
*   [x] Functions with parameters and return values.
*   [x] Complete standard library (File, Math, String, Array).
*   [x] Exception handling keywords (`try`, `catch`, `throw`).
*   [x] Web-based IDE with real-time feedback.

### Limitation & Challenges
*   **Type System:** Currently, the type system is primarily dynamic or loosely static. Stricter type checking during the semantic phase is a target for improvement.
*   **Optimization:** The IR optimizer is in early stages. Advanced optimizations like dead code elimination or constant folding are not fully exhaustive.
*   **Error Messages:** While syntax errors are reported, runtime errors in the VM could provide more detailed stack traces.

**Challenges Faced:**
1.  **Memory Management:** Implementing a Garbage Collector from scratch in C was technically demanding, requiring precise tracking of object references.
2.  **Parser Conflicts:** resolving Shift/Reduce conflicts in Bifon for complex nested grammar rules (like nested `if-else` blocks).
3.  **Cross-Platform I/O:** Ensuring the file I/O and system calls worked consistently across different operating systems (macOS vs. Linux/Windows).

## 6. Future Plans

The Kotha project has a clear roadmap for future expansion:
1.  **Static Analysis Tooling:** Building a linter to catch potential bugs before compilation.
2.  **Language Server Protocol (LSP):** Implementing LSP to allow Kotha to plug into VS Code or other major editors with full autocompletion support.
3.  **Foreign Function Interface (FFI):** Allowing Kotha to call C libraries directly, enabling access to graphics (SDL/OpenGL) and networking libraries.
4.  **Mobile App:** Porting the IDE to a mobile application to allow coding on the go.

## 7. Conclusion

The Kotha project successfully demonstrates a full-stack compiler implementation. By abstracting complex programming concepts into a Bengali syntax, it democratizes access to computer science education. The robust architecture—featuring a custom VM, IR generator, and extensive libraries—proves that Kotha is not just a toy language but a platform capable of real computational tasks. 
