# VerbLang Compiler & Runtime Architecture 🏛️
**Verbund Labs Engineering Documentation**

```
+-----------------------------------------------------------------+
|                       VerbLang Architecture                    |
+-----------------------------------------------------------------+
                                │
               Source Code (.verb file)
                                │
                                ▼
                       +-----------------+
                       |      Lexer      |  -> Token Stream
                       +-----------------+
                                │
                                ▼
                       +-----------------+
                       |     Parser      |  -> Abstract Syntax Tree (AST)
                       +-----------------+
                                │
                                ▼
                       +-----------------+
                       |  C++20 Codegen  |  -> Optimized C++20 Source
                       +-----------------+
                                │
                                ▼
                       +-----------------+
                       |  Clang++ / LLVM |  -> Links libverbrt.a
                       +-----------------+
                                │
                                ▼
                       Standalone ELF / Mach-O Native Binary
```

---

## 1. Compiler Subsystem (`verblang`)

The compiler executable `verbc` is organized into modular phases:

1. **`Lexer` (`src/lexer.cpp`)**:
   - Tokenizes comments (`#`, `//`, `/* */`), numeric literals (int, hex, float), string and template literals (`` `...` ``), native C++ blocks (`native cxx { ... }`), HTML tags (`<tag>`, `</tag>`, `/>`), CSS rules (`style`), and language keywords.
2. **`Parser` (`src/parser.cpp`)**:
   - Recursive descent parser with operator precedence parsing.
   - Constructs an AST (`include/verb/ast.hpp`) representing expressions, statements, markup nodes, style rules, closures, and control flow.
3. **`Code Generator` (`src/codegen_cxx.cpp`)**:
   - AST visitor (`CxxCodeGen`) that emits standards-compliant C++20 code.
   - Generates type-safe bindings to `verbrt::Value` and standard library functions (`verbrt::builtin_*`).
4. **`CLI Driver` (`src/main.cpp`)**:
   - Manages compilation pipelines, temporary files, optimization flags (`-O3`), and REPL evaluation loops.

---

## 2. Runtime Subsystem (`verb-runtime`)

The native engine is packaged as `libverbrt`:

- **Value Representation (`include/verbrt/value.hpp`)**:
  - Unified variant holding primitives (`int64_t`, `double`, `bool`, `std::string`), refcounted heap collections (`ValueList`, `ValueDict`), function closures, raw pointers, and DOM nodes.
  - Operator overloading for dynamic arithmetic, comparisons, slicing, and string concatenation.
- **Memory Subsystem (`include/verbrt/memory.hpp`)**:
  - Raw memory pointers (`raw_alloc`, `raw_free`, `raw_peek64`, `raw_poke64`).
  - Bump/Arena allocator (`ArenaAllocator`) for zero-fragmentation bulk allocations.
  - RAII `ScopeGuard` macro (`VERB_DEFER`) for deterministic lifetime management.
- **DOM & CSS Engine (`include/verbrt/dom.hpp`, `include/verbrt/css.hpp`)**:
  - `VerbNode`: Tree node containing tag, attributes, and children.
  - `VerbStyleRule`: Parsed CSS rules with selector indexing.
  - `StyleSheet`: Global cascade matcher that maps selectors (`tag`, `.class`, `#id`) to computed styles.
  - Multi-target renderers: HTML5 generator and ANSI colored terminal box renderer.
- **Concurrency & Event Loop (`include/verbrt/event_loop.hpp`)**:
  - Microtask and macrotask queues.
  - Native thread pool with `Promise` resolution.

---
© 2026 Verbund Labs.
