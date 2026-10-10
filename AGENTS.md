# AGENTS.md — VerbLang Technical Specification & Architecture Reference
Language: VerbLang (`.verb` source files)
Maintained by: Verbund Labs
Specification: 1.0.0

This specification document outlines the grammar, execution model, standard library, and module system for VerbLang.

---

## 1. Execution Model & Architecture

VerbLang is compiled Ahead-Of-Time (AOT) to modern C++20 and linked against `libverbrt`, the native runtime engine developed by Verbund Labs.

- Source Files: `.verb`
- Toolchain: `verbc` (compiler, module resolver, C++20 transpiler, and runner)
- Runtime: `libverbrt` (provides `verbrt::Value` variant, arena memory management, virtual DOM, CSS cascade engine, and asynchronous event loop)
- Output: Standalone native ELF executable (or transpiled C++20 via `verbc emit-cxx`)
- Target: x86_64 / arm64 Linux, macOS, BSD

---

## 2. Syntax & Grammar

### 2.1 Declarations
```verb
let count = 42;
const max_connections = 1024;

# Optional type annotations
let port: int = 8080;
let host: str = "127.0.0.1";
```

### 2.2 Comments
```verb
# Python-style line comment
// C++/JavaScript line comment
/* Multi-line block comment */
```

### 2.3 Functions & Closures
```verb
fn compute_checksum(data, seed) {
    return data * 31 + seed;
}

# Pythonic syntax also accepted
def calculate_ratio(a, b): {
    return a / b;
}

# Arrow functions and expressions
let square = x => x * x;
let add = (a, b) => a + b;

# Closures
fn create_accumulator(start) {
    let current = start;
    return (delta) => {
        current += delta;
        return current;
    };
}

# Asynchronous functions
async fn fetch_telemetry(channel) {
    return `telemetry-stream-${channel}`;
}
```

### 2.4 Control Flow
```verb
# Conditionals
if (threshold > 100) {
    print("Warning: High load");
} elif (threshold > 50) {
    print("Status: Moderate");
} else {
    print("Status: Normal");
}

# Iteration
while (retries > 0) {
    retries -= 1;
}

for item in queue {
    process(item);
}

for i in range(0, 10, 2) {
    print("Index:", i);
}

# Pattern Matching
match status_code {
    200 => print("OK")
    404 => print("Not Found")
    500 => print("Internal Server Error")
    _   => print("Unknown status:", status_code)
}
```

### 2.5 Collections & Comprehensions
```verb
let numbers = [10, 20, 30, 40, 50, 60];
let config = {
    "host": "localhost",
    "port": 9000,
    "ssl": true
};

# Slicing
let subset = numbers[1:4]; # [20, 30, 40]

# List Comprehensions: [expression for variable in iterable (if condition)?]
let filtered = [x * 2 for x in numbers if x > 25];
```

### 2.6 Module System & Imports
```verb
# Import other VerbLang files relative to current file or working directory
import "math_utils.verb";
import "./components/layout.verb";
```

### 2.7 Virtual DOM & CSS Styles
```verb
# CSS Rule Declarations
style .container {
    color: "cyan";
    border: "1px solid #45475a";
    font-weight: "bold";
}

style .title {
    color: "magenta";
    font-weight: "bold";
}

style button {
    color: "green";
}

# First-class Virtual DOM AST
let panel = <div class="container" id="main-panel">
    <h1 class="title">"System Overview"</h1>
    <p>`Active workers: ${len(numbers)}`</p>
    <button>"Run Diagnostics"</button>
</div>;

# Render targets:
let html = render_html(panel); # Standard HTML5 string
let tui = render_tui(panel);   # ANSI terminal UI with cascaded styles
```

### 2.8 Low-Level Memory & Native C++ Blocks
```verb
# Raw heap memory allocation
let buffer = alloc(128);

# Deterministic RAII cleanup
defer {
    free(buffer);
}

# Direct 64-bit memory reads and writes
poke64(buffer, 0x123456789ABCDEF0);
let val = peek64(buffer);

# Embedded native C++ block
native cxx {
    #include <chrono>
    inline uint64_t get_host_nanos() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
}
```

---

## 3. Standard Library Reference

### Core & I/O
- `print(...args)`: Write space-separated values to stdout with a newline.
- `len(val)`: Returns integer length of string, list, or dict.
- `range(stop)` or `range(start, stop, [step])`: Generates list of integers.
- `type(val)`: Returns type name string (`"int"`, `"float"`, `"string"`, `"list"`, `"dict"`, `"pointer"`, `"node"`, `"nil"`).
- `args()`: Returns list of command-line arguments passed to the program.
- `read_file(path)`: Reads entire file as string.
- `write_file(path, content)`: Writes string to file. Returns boolean.
- `file_exists(path)`: Returns true if file exists.
- `file_remove(path)`: Deletes file from filesystem.
- `exec(cmd)`: Executes system command and returns stdout as string.
- `env_get(key)`: Gets environment variable value.
- `env_set(key, val)`: Sets environment variable.
- `assert(condition, [msg])`: Aborts execution with error message if condition evaluates to false.

### Time & Control
- `now_ns()`: High-resolution timestamp in nanoseconds.
- `now_ms()`: Millisecond timestamp.
- `sleep(ms)`: Pauses execution thread for specified milliseconds.

### Memory & Low-Level
- `alloc(bytes)`: Allocates raw memory pointer.
- `free(ptr)`: Deallocates memory pointer.
- `peek64(ptr)`: Reads 64-bit integer from pointer.
- `poke64(ptr, value)`: Writes 64-bit integer to pointer.

### Math
- `abs(n)`: Absolute value of number.
- `min(...args)`: Smallest value in arguments.
- `max(...args)`: Largest value in arguments.
- `floor(n)`: Floor of float value.
- `ceil(n)`: Ceiling of float value.
- `round(n)`: Rounds float value to nearest integer.
- `sqrt(n)`: Square root.
- `pow(base, exp)`: Power calculation.
- `sin(rad)` / `cos(rad)`: Trigonometric functions.

### String & List Utilities
- `str_upper(s)` / `str_lower(s)`: Convert string case.
- `str_trim(s)`: Strip leading and trailing whitespace.
- `str_split(s, delim)`: Split string into list.
- `str_join(list, delim)`: Join list into string.
- `str_contains(s, substr)`: Check substring presence.
- `str_starts_with(s, prefix)`: Test string prefix.
- `str_ends_with(s, suffix)`: Test string suffix.
- `str_replace(s, target, repl)`: Replace substring occurrences.
- `list_push(list, val)`: Append value to list.
- `list_pop(list)`: Remove and return last element.
- `list_reverse(list)`: Reverse list copy.
- `list_sort(list)`: Sort list copy.
- `list_contains(list, item)`: Test list membership.
- `list_slice(list, start, end)`: Extract list slice.

### Data Interchange & Reactivity
- `json_stringify(val)`: Serialize value to JSON string.
- `json_parse(str)`: Parse JSON string into value.
- `create_signal(initial)`: Creates reactive signal `{ "get", "set", "subscribe" }`.
- `render_html(node)`: Serialize virtual DOM node to HTML5 string.
- `render_tui(node)`: Render virtual DOM node to ANSI terminal UI with cascaded styles.

---

## 4. Compiler CLI (`verbc`)

```bash
# Run source file directly
verbc run <file.verb> [args...]

# Compile to standalone native binary
verbc compile <file.verb> -o <binary_name>

# Syntax and type check without compiling
verbc check <file.verb>

# Discover and run tests
verbc test [directory]

# Transpile to modern C++20 code
verbc emit-cxx <file.verb> -o output.cpp

# Scaffold a new VerbLang project
verbc new <project_name>

# Format source file
verbc fmt <file.verb>

# Start interactive REPL
verbc repl

# Print version and target info
verbc version
```

---

## 5. Engineering Standards

1. Use `.verb` extension for all source files.
2. Pair every `alloc()` call with a `defer { free(ptr); }` block for deterministic RAII cleanup.
3. Write hexadecimal integer literals without suffix (`0xCAFEBABE1234`), as VerbLang natively operates on 64-bit integers.
4. Express HTML markup directly as AST nodes without string escaping.
5. Verify changes with `verbc check <file.verb>`, `verbc test`, or `make test`.

---
Copyright (c) 2026 Verbund Labs.
)AGENTS_GUIDE";

} // namespace verb
