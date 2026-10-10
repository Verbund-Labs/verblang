# VerbLang ⚡
**The Unified Systems Programming Language Created by Verbund Labs**

[![GitHub License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-purple.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![LLVM](https://img.shields.io/badge/Backend-Clang%20%2F%20LLVM-brightgreen.svg)](https://llvm.org/)
[![Status](https://img.shields.io/badge/status-Production%20Ready-success.svg)](#)

> **VerbLang** combines the **raw speed, memory control, and pointer capabilities of C++** with the **intuitive syntax of Python**, the **reactivity and closures of JavaScript**, and **first-class HTML markup and CSS styling** directly in `.verb` files.

---

## 🌟 Key Features

| Paradigm | What VerbLang Delivers |
|---|---|
| **C++20 Speed & Control** | Direct pointer arithmetic (`alloc`, `free`, `peek64`, `poke64`), bump arena allocators, RAII `defer`, inline `native cxx { ... }` blocks, and native binary generation via Clang++/LLVM. |
| **Python Ease of Use** | Clean functions (`fn`/`def`), list comprehensions (`[x * 2 for x in items]`), array slices (`arr[1:4]`), `len()`, `range()`, and dictionary literals. |
| **JavaScript / TypeScript** | Modern arrow functions (`(a, b) => a + b`), lexical closures, template strings (`` `Hello, ${name}!` ``), and async functions. |
| **HTML First-Class Markup** | Direct declarative Virtual DOM elements (`<div class="card"><h1>"Title"</h1></div>`) in language AST. |
| **CSS First-Class Styling** | Direct stylesheet rules (`style .card { color: "cyan"; }`) with selector cascade and multi-target rendering (HTML5 and ANSI Terminal TUI). |

---

## 🚀 Quick Start

### 1. Requirements
- Linux or macOS
- Modern C++ compiler: `clang++` >= 15 or `g++` >= 11
- `make` or `cmake` >= 3.16

### 2. Building VerbLang
```bash
# Clone the compiler and runtime
git clone git@github.com:Verbund-Labs/verblang.git
git clone git@github.com:Verbund-Labs/verb-runtime.git

cd verblang
make
```

### 3. Running Your First `.verb` File
```bash
./bin/verbc run examples/01_hello_world.verb
```

### 4. Compiling to a Standalone Native ELF Binary
```bash
./bin/verbc compile examples/06_full_stack_app.verb -o my_app
./my_app
```

---

## 💻 Code Example: `06_full_stack_app.verb`

```verb
# 1. First-Class CSS Stylesheet
style .terminal-app {
    color: "cyan";
    border: "1px solid #585b70";
}

style .brand-header {
    color: "magenta";
    font-weight: "bold";
}

# 2. Pythonic Pipeline: Filtering & Comprehension
let telemetry = [12, 45, 88, 102, 14, 99, 134, 7, 60, 210];
let high_load = [m * 2 for m in telemetry if m > 50];

# 3. JS-Style Reactive Lambda
let compute_score = (used, total) => (used * 100) / total;

# 4. C++ Direct Memory Allocation & RAII Defer
fn probe_hardware() {
    let buffer = alloc(128);
    defer {
        print("Buffer automatically freed!");
        free(buffer);
    }
    poke64(buffer, 0x56455242554E44);
}

# 5. First-Class HTML Markup Component
let dashboard = <div class="terminal-app" id="main-panel">
    <h1 class="brand-header">"Verbund Labs - VerbLang Control Center"</h1>
    <p>`Telemetry Channels: ${len(telemetry)} active`</p>
    <button>"Execute Native Workload"</button>
</div>;

# Render directly to ANSI styled Terminal UI:
print(render_tui(dashboard));
```

---

## 🛠️ Compiler CLI (`verbc`) Reference

```bash
verbc run <file.verb>           # Compile and execute immediately
verbc compile <file.verb> -o app # Build standalone native binary (-O3 optimized)
verbc emit-cxx <file.verb>      # Transpile to readable C++20 code
verbc repl                      # Start interactive REPL
verbc version                   # Display Verbund Labs version info
```

---

## 📁 Repository Structure

```
verblang/
├── src/
│   ├── lexer.cpp         # Tokenizer (Python comments, HTML, CSS, C++ blocks)
│   ├── parser.cpp        # AST builder & operator precedence parser
│   ├── ast.cpp           # AST node dispatch
│   ├── codegen_cxx.cpp   # C++20 source code generator
│   └── main.cpp          # verbc compiler CLI & REPL driver
├── include/verb/
│   ├── token.hpp         # Token definitions
│   ├── lexer.hpp         # Lexer header
│   ├── parser.hpp        # Parser header
│   ├── ast.hpp           # AST node structures
│   └── codegen_cxx.hpp   # Code generator header
├── examples/
│   ├── 01_hello_world.verb
│   ├── 02_pythonic_data.verb
│   ├── 03_js_async_lambdas.verb
│   ├── 04_html_css_components.verb
│   ├── 05_cpp_speed_pointers.verb
│   └── 06_full_stack_app.verb
├── docs/
│   ├── SPECIFICATION.md
│   ├── SYNTAX_GUIDE.md
│   └── ARCHITECTURE.md
├── Makefile
├── CMakeLists.txt
├── README.md
└── LICENSE
```

---

## 🔗 Related Repositories

- [Verbund-Labs/verb-runtime](https://github.com/Verbund-Labs/verb-runtime): The C++20 native runtime engine, DOM/CSS layout engine, and allocator library.

---

## 📄 License

Licensed under the Apache License, Version 2.0. Copyright © 2026 Verbund Labs.
