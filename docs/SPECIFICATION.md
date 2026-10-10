# VerbLang Language Specification ⚡
**Version 1.0.0**  
**Author:** Verbund Labs  
**Standard Status:** Official Specification  

---

## 1. Introduction & Design Philosophy

**VerbLang** is a next-generation compiled systems language engineered by **Verbund Labs**. It is designed to resolve the historic tradeoff between developer ergonomics and execution speed by synthesizing five foundational computing syntaxes into one coherent paradigm:

1. **C++ Capabilities & Speed**: Direct pointer arithmetic, bump/arena memory allocators, RAII deterministic lifetime (`defer`), zero-cost abstractions, inline C++ code blocks, and native machine code compilation via Clang++/LLVM.
2. **Python Ergonomics**: Readable function definitions (`fn`/`def`), list comprehensions, slicing (`[start:end:step]`), multi-argument `print()`, `range()`, and dictionary mappings.
3. **JavaScript & TypeScript Reactivity**: Arrow functions (`=>`), closures, template literals (`` `...${expr}...` ``), async function declarations (`async fn`), and optional semicolons.
4. **HTML First-Class Markup**: Declarative Virtual DOM nodes (`<tag attr="val">children</tag>`) directly in the language grammar as first-class AST expressions.
5. **CSS First-Class Styles**: First-class stylesheet rules (`style .class { prop: val; }`) that cascade onto virtual DOM elements and render to HTML5 or terminal ANSI TUI layouts.

All VerbLang files use the **`.verb`** extension.

---

## 2. Lexical Structure

### 2.1 Character Set & Comments
- **Source Encoding**: UTF-8.
- **Line Comments**: `#` (Python-style) and `//` (C++/JS-style).
- **Block Comments**: `/* ... */`.

### 2.2 Identifiers & Literals
- **Identifiers**: Match `[a-zA-Z_][a-zA-Z0-9_]*`.
- **Integer Literals**: Decimal (`42`, `1000000`) and Hexadecimal (`0xCAFEBABE`). Represented as 64-bit signed integers (`int64_t`).
- **Float Literals**: IEEE 754 double precision (`3.14159`, `1.0e-5`).
- **String Literals**: Single quote `'...'` and double quote `"..."`. Supports standard escape sequences (`\n`, `\t`, `\"`, `\\`).
- **Template Literals**: Backtick syntax `` `Hello, ${name}!` ``. Evaluates interpolated expressions into the string.
- **Raw C++ Blocks**: `native cxx { ... }`. Scanned with brace matching and emitted verbatim into the generated C++ translation unit.

---

## 3. Type System & Value Model

VerbLang features a unified dynamic and static value model (`verbrt::Value`):

| Type | Underlying C++ Representation | Description |
|------|------------------------------|-------------|
| `Nil` | `nullptr` / unit | Equivalent to `nil`, `null`, `None` |
| `Bool` | `bool` | `true` or `false` |
| `Int` | `int64_t` | 64-bit signed integer |
| `Float` | `double` | 64-bit floating point |
| `String` | `std::string` | UTF-8 string |
| `List` | `std::shared_ptr<std::vector<Value>>` | Dynamic array |
| `Dict` | `std::shared_ptr<std::unordered_map<std::string, Value>>` | Hash table / object |
| `Function` | `std::function<Value(const std::vector<Value>&)>` | Callable closure |
| `Pointer` | `void*` | Raw memory address |
| `Node` | `std::shared_ptr<VerbNode>` | Virtual DOM tree node |
| `StyleRule` | `std::shared_ptr<VerbStyleRule>` | Cascading style rule |

---

## 4. Statements & Control Flow

### 4.1 Variable Binding
```verb
let mutable_var = 100;
const immutable_var = 3.14159;
let typed_var: int = 42;
```

### 4.2 Functions & Closures
```verb
fn add(a, b) {
    return a + b;
}

let multiply = (x, y) => x * y;
```

### 4.3 Conditionals & Loops
```verb
if (x > 10) {
    print("Large");
} elif (x > 5) {
    print("Medium");
} else {
    print("Small");
}

for item in items {
    print(item);
}

while (count > 0) {
    count -= 1;
}
```

### 4.4 Resource Management (`defer`)
Statements enclosed in `defer { ... }` execute in reverse order upon scope exit, guaranteeing deterministic cleanup.
```verb
let ptr = alloc(1024);
defer {
    free(ptr);
}
```

---

## 5. First-Class UI & Styling Engine

### 5.1 CSS Declarations
```verb
style .hero-card {
    color: "cyan";
    border: "1px solid #ffffff";
    font-weight: "bold";
}
```

### 5.2 HTML DOM Components
```verb
let view = <div class="hero-card" id="main">
    <h1>"VerbLang Native Engine"</h1>
    <button>"Click to Benchmark"</button>
</div>;
```

### 5.3 Render Targets
- `render_html(view)`: Serializes the component tree to standard HTML5 markup.
- `render_tui(view)`: Matches CSS styles (selectors, colors, borders, typography) and renders to styled ANSI terminal graphics.

---

## 6. Systems Programming & C++ Capabilities

### 6.1 Direct Memory Operations
- `alloc(bytes)`: Allocates raw heap memory.
- `free(ptr)`: Releases raw memory.
- `peek64(ptr)`: Reads a 64-bit integer from a memory address.
- `poke64(ptr, value)`: Writes a 64-bit integer into a memory address.

### 6.2 Native C++ Blocks
```verb
native cxx {
    #include <chrono>
    inline uint64_t hardware_time() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
}
```

---

## 7. Toolchain (`verbc`)

The compiler executable `verbc` coordinates the pipeline:
- `verbc compile <file.verb> -o <binary>`: Generates C++20, compiles via `clang++ -O3`, and links `libverbrt.a`.
- `verbc run <file.verb>`: Compiles and executes in a single pass.
- `verbc emit-cxx <file.verb>`: Prints generated C++20 code.
- `verbc repl`: Interactive REPL.

© 2026 Verbund Labs.
