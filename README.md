# VerbLang
**A multi-paradigm systems programming language combining modern C++20 execution with Python, JavaScript, HTML, and CSS ergonomics.**

Maintained by **Verbund Labs**.

[![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
[![Backend](https://img.shields.io/badge/backend-Clang%20%2F%20LLVM-black.svg)](https://llvm.org/)

VerbLang (`.verb`) is designed for systems engineering, tooling, and reactive interfaces. It compiles Ahead-Of-Time to optimized C++20 and links against `libverbrt` to produce standalone native binaries with zero runtime dependencies.

---

## Language Highlights

- **Systems Performance**: Direct 64-bit memory operations (`alloc`, `free`, `peek64`, `poke64`), RAII `defer` cleanup, bump arena allocation, and inline `native cxx { ... }` blocks.
- **Python-Style Data Ergonomics**: Dynamic lists, dictionary literals, array slicing (`arr[1:4]`), comprehensions (`[x * 2 for x in items if x > 10]`), `len()`, and `range()`.
- **First-Class Closures & Async**: JavaScript-style arrow lambdas (`(a, b) => a + b`), lexical closures, template strings (`` `Value: ${x}` ``), and async functions.
- **First-Class DOM & CSS**: Express HTML virtual DOM trees and declarative CSS rules directly in grammar, with multi-target rendering to HTML5 markup and styled ANSI terminal interfaces.
- **Modular Architecture**: Transitive file and module imports (`import "math_utils.verb";`) with cycle detection.
- **Rich Diagnostics**: Clang/Rust-style compiler diagnostics with source line excerpts and column carets.

---

## Quickstart

### Prerequisites
- Modern C++ compiler (`clang++` >= 15 or `g++` >= 11) supporting C++20.
- POSIX environment (Linux, macOS, BSD).
- GNU Make.

### Building the Toolchain
```bash
git clone git@github.com:Verbund-Labs/verb-runtime.git
git clone git@github.com:Verbund-Labs/verblang.git

cd verblang
make
```

### Running Source Files
```bash
# Execute directly
./bin/verbc run examples/01_hello_world.verb

# Syntax & type check without compiling
./bin/verbc check examples/01_hello_world.verb

# Run built-in test suite
./bin/verbc test tests/

# Compile to standalone native binary
./bin/verbc compile examples/06_full_stack_app.verb -o app
./app
```

---

## Code Example

```verb
import "./utils.verb";

# Declarative CSS Stylesheet
style .dashboard {
    color: "cyan";
    border: "1px solid #45475a";
}

style .title {
    color: "magenta";
    font-weight: "bold";
}

# Pythonic filtering and comprehension
let telemetry = [12, 45, 88, 102, 14, 99, 134, 7, 60, 210];
let high_load = [x * 2 for x in telemetry if x > 50];

# Direct heap allocation and RAII cleanup
fn probe_hardware() {
    let buffer = alloc(128);
    defer {
        free(buffer);
    }
    poke64(buffer, 0x56455242554E44);
}

# First-class Virtual DOM element
let view = <div class="dashboard" id="main-panel">
    <h1 class="title">"System Telemetry Dashboard"</h1>
    <p>`Monitored channels: ${len(telemetry)}`</p>
    <button>"Run Diagnostics"</button>
</div>;

# Render to styled ANSI terminal UI
print(render_tui(view));
```

---

## Toolchain CLI (`verbc`)

```bash
verbc run <file.verb> [args...]     Compile and execute a .verb source file
verbc compile <file.verb> [-o out]  Compile to standalone native binary (-O3 optimized)
verbc check <file.verb>             Typecheck and parse file without invoking native compilation
verbc test [path]                   Discover and run test suites
verbc emit-cxx <file.verb>          Transpile .verb to readable C++20
verbc new <project-name>            Scaffold a new project with manifest & AGENTS.md
verbc fmt <file.verb>               Format source file
verbc repl                          Start interactive REPL session
verbc version                       Print compiler and runtime target info
```

---

## Repository Layout

```
verblang/
├── src/
│   ├── lexer.cpp         # Tokenizer
│   ├── parser.cpp        # AST parser and diagnostic reporter
│   ├── ast.cpp           # AST node definitions
│   ├── codegen_cxx.cpp   # C++20 transpiler
│   └── main.cpp          # CLI driver, module resolver, runner
├── include/verb/
│   ├── token.hpp         # Token definitions
│   ├── lexer.hpp         # Lexer interface
│   ├── parser.hpp        # Parser interface
│   ├── ast.hpp           # AST definitions and visitor pattern
│   ├── codegen_cxx.hpp   # Code generator interface
│   └── agents_guide.hpp  # Built-in AGENTS.md specification template
├── examples/             # 7 end-to-end showcase programs
├── tests/                # Automated test suites
├── extensions/vscode/    # VS Code extension with syntax highlighting
├── Makefile
├── README.md
└── AGENTS.md             # Technical guide and language specification
```

---

## Related Repositories

- [Verbund-Labs/verb-runtime](https://github.com/Verbund-Labs/verb-runtime): Native C++20 runtime engine (`libverbrt`), virtual DOM, CSS layout engine, memory allocators, and event loop.

---

## License

Apache License 2.0. Copyright (c) 2026 Verbund Labs.
