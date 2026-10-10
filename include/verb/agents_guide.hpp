#pragma once

namespace verb {

inline constexpr const char* AGENTS_GUIDE_TEMPLATE = R"AGENTS_GUIDE(# AGENTS.md — VerbLang Engineering Guide for AI Agents ⚡
**Language:** VerbLang (`.verb` source files)  
**Created By:** Verbund Labs  
**Specification Version:** 1.0.0  

This guide provides complete technical rules, grammar patterns, standard library references, and best practices for AI coding assistants and autonomous agents modifying or generating VerbLang codebases.

---

## 1. Architecture & Execution Model

VerbLang code is compiled Ahead-Of-Time (AOT) to optimized modern **C++20** and linked against the high-performance native engine `libverbrt` developed by **Verbund Labs**.

- **File Extension**: All VerbLang files must use `.verb`.
- **Compiler Executable**: `verbc` (orchestrates lexing, parsing, C++20 emission, and Clang++/LLVM compilation).
- **Core Runtime**: `verb-runtime` (`libverbrt`), providing unified variant values (`verbrt::Value`), virtual DOM trees, CSS selector resolvers, event loop, and memory allocators.
- **Performance Characteristics**: Zero-overhead native execution, deterministic RAII cleanup, throughput exceeding 2.4 million operations per second on standard hardware.

---

## 2. Syntax & Grammar Reference

### 2.1 Variables & Constants
```verb
let mutable_val = 100;
const immutable_val = 3.14159;

# Optional type annotations
let count: int = 42;
let name: str = "Verbund";
```

### 2.2 Comments
```verb
# Python-style line comment
// C++/JavaScript-style line comment
/* Multi-line
   block comment */
```

### 2.3 Functions & Closures
```verb
# Standard function definition (fn or def)
fn add(a, b) {
    return a + b;
}

def multiply(a, b): {
    return a * b;
}

# JavaScript-style arrow functions & lambdas
let square = x => x * x;
let compute = (a, b) => a * b + 10;

# Lexical closures
fn make_counter(start) {
    let current = start;
    return () => {
        current += 1;
        return current;
    };
}

# Asynchronous functions
async fn fetch_telemetry(channel) {
    print(`Polling telemetry on channel ${channel}...`);
    return "OK";
}
```

### 2.4 Control Flow
```verb
# If / Elif / Else
if (score > 90) {
    print("Grade: A");
} elif (score > 75) {
    print("Grade: B");
} else {
    print("Grade: C");
}

# While loop
while (count > 0) {
    count -= 1;
}

# For-In loop (Pythonic iteration)
for item in items {
    print("Item:", item);
}

for i in range(5) {
    print("Index:", i);
}

# Pattern Matching (match statement)
match status_code {
    200 => print("HTTP 200: OK")
    404 => print("HTTP 404: Not Found")
    500 => print("HTTP 500: Server Error")
    _   => print(`Unknown Status: ${status_code}`)
}
```

### 2.5 Pythonic Data Structures, Slices & Comprehensions
```verb
# Lists and Dictionaries
let numbers = [10, 20, 30, 40, 50, 60];
let user = {
    "name": "KiyokoDev",
    "org": "Verbund Labs",
    "role": "Architect"
};

# Slicing: arr[start:end] or arr[start:end:step]
let subset = numbers[1:4]; # [20, 30, 40]

# List Comprehensions: [expr for var in iterable (if condition)?]
let squares = [x * x for x in numbers if x > 25];
```

### 2.6 Template Literals
```verb
let app_name = "VerbLang Engine";
let latency = 0.42;
print(`System ${app_name} running at ${latency} ms latency.`);
```

### 2.7 First-Class HTML Markup & CSS Styling
```verb
# CSS Rule Declarations (supports .class, #id, tag, and kebab-case properties)
style .card {
    color: "cyan";
    border: "1px solid #ffffff";
    font-weight: "bold";
}

style .title {
    color: "magenta";
    font-weight: "bold";
}

style button {
    color: "green";
    font-weight: "bold";
}

# HTML Virtual DOM Elements (first-class AST expressions)
let dashboard = <div class="card" id="main-panel">
    <h1 class="title">"Verbund Labs Control Center"</h1>
    <p>`Nominal metrics: ${len(numbers)} nodes`</p>
    <button>"Run Hardware Benchmark"</button>
</div>;

# Render targets:
let html_output = render_html(dashboard); # Standard HTML5 markup
let tui_output = render_tui(dashboard);   # Styled ANSI Terminal UI
print(tui_output);
```

### 2.8 C++ Capabilities, Direct Pointers & Defer
```verb
# Direct heap allocation and raw pointer access
let ptr = alloc(64); # Allocate 64 bytes

# Deterministic RAII cleanup
defer {
    print("Releasing raw pointer buffer!");
    free(ptr);
}

# Raw 64-bit integer peek & poke
poke64(ptr, 0xCAFEBABE1234);
let read_back = peek64(ptr);

# Embedded Native C++ Blocks
native cxx {
    #include <chrono>
    inline uint64_t get_host_nanos() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
}
```

---

## 3. Standard Library Builtins Reference

| Function | Signature | Description |
|---|---|---|
| `print(...)` | `print(...values)` | Prints arguments space-separated with a newline. |
| `len(val)` | `len(collection): int` | Returns length of string, list, or dictionary. |
| `range(...)` | `range([start], stop, [step]): list` | Generates integer sequence list. |
| `type(val)` | `type(value): str` | Returns runtime type name (`"int"`, `"float"`, `"string"`, `"list"`, `"dict"`, `"node"`, `"pointer"`). |
| `now_ns()` | `now_ns(): int` | High-precision nanosecond timestamp. |
| `now_ms()` | `now_ms(): int` | Millisecond timestamp. |
| `sleep(ms)` | `sleep(milliseconds)` | Pauses execution for given milliseconds. |
| `alloc(bytes)` | `alloc(bytes: int): pointer` | Allocates raw heap memory. |
| `free(ptr)` | `free(pointer)` | Deallocates raw memory. |
| `poke64(ptr, val)` | `poke64(ptr, value: int)` | Writes 64-bit integer to address. |
| `peek64(ptr)` | `peek64(ptr): int` | Reads 64-bit integer from address. |
| `json_stringify(val)` | `json_stringify(value): str` | Serializes value to JSON string. |
| `json_parse(str)` | `json_parse(json_str: str): Value` | Deserializes JSON string into VerbLang value. |
| `str_upper(s)` | `str_upper(s: str): str` | Uppercases string. |
| `str_lower(s)` | `str_lower(s: str): str` | Lowercases string. |
| `str_trim(s)` | `str_trim(s: str): str` | Strips leading/trailing whitespace. |
| `str_split(s, sep)` | `str_split(s: str, sep: str): list` | Splits string by delimiter. |
| `str_join(list, sep)`| `str_join(list, sep: str): str` | Joins list of strings by delimiter. |
| `str_contains(s, sub)`| `str_contains(s: str, sub: str): bool`| Tests if substring is present. |
| `list_push(l, val)` | `list_push(list, value)` | Appends item to list. |
| `list_pop(l)` | `list_pop(list): Value` | Removes and returns last item of list. |
| `list_reverse(l)` | `list_reverse(list): list` | Returns reversed list. |
| `list_sort(l)` | `list_sort(list): list` | Returns sorted list. |
| `create_signal(init)`| `create_signal(initial_val): dict`| Creates reactive signal `{ "get", "set", "subscribe" }`. |
| `render_html(node)` | `render_html(node): str` | Generates HTML5 string from virtual DOM node. |
| `render_tui(node)` | `render_tui(node): str` | Computes cascading CSS styles and renders ANSI terminal UI. |

---

## 4. CLI Workflow & Toolchain (`verbc`)

```bash
# Run a script directly
verbc run <file.verb> [args...]

# Compile to standalone native binary with Clang++ -O3
verbc compile <file.verb> -o <binary_name>

# Transpile to inspect generated C++20 code
verbc emit-cxx <file.verb> -o output.cpp

# Scaffold a new Verb project with manifest and agent instructions
verbc new <project_name>

# Format and beautify .verb source files
verbc fmt <file.verb>

# Interactive REPL
verbc repl

# Version & author info
verbc version
```

---

## 5. Guidelines for AI Agents Working on VerbLang

1. **Maintain File Extension**: Always create `.verb` files for VerbLang source code.
2. **Deterministic Cleanup**: Whenever calling `alloc()`, always pair with a `defer { free(ptr); }` block.
3. **No Hex Suffixes**: Write hexadecimal literals as `0x1234ABCD` directly; do NOT append `LL` or `ULL` (VerbLang integers are 64-bit natively).
4. **HTML Expressions**: Write HTML tags directly as AST expressions without enclosing them in strings or quotes.
5. **CSS Selectors**: Use `style .class-name { prop: "val"; }` or `style tag { ... }`.
6. **Reactive State**: Prefer `create_signal(initial)` when managing state across UI components or asynchronous event listeners.
7. **Verification**: Always run `verbc run <file.verb>` or `make test` to verify syntax and execution before finishing tasks.

---
© 2026 Verbund Labs.
)AGENTS_GUIDE";

} // namespace verb
