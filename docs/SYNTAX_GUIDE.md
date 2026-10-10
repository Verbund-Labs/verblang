# VerbLang Syntax Guide 📘
**Created by Verbund Labs**

VerbLang merges the best idioms from Python, JavaScript/TypeScript, HTML, CSS, and C++ into one seamless language.

---

## 🐍 Python Features in VerbLang

### 1. List Comprehensions
Transform and filter lists concisely:
```verb
let numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
let evens_squared = [x * x for x in numbers if x % 2 == 0];
# Output: [4, 16, 36, 64, 100]
```

### 2. Slices
Slice arrays and strings with `[start:end]` and `[start:end:step]`:
```verb
let subset = numbers[2:7]; # [3, 4, 5, 6, 7]
```

### 3. Clean Iteration & Builtins
```verb
for i in range(5) {
    print("Item:", i);
}

let count = len(numbers);
```

---

## 🌐 JavaScript / TypeScript Features in VerbLang

### 1. Arrow Functions & Closures
```verb
let add = (a, b) => a + b;
let square = x => x * x;

fn make_adder(base) {
    return x => x + base;
}
```

### 2. Template Strings
Interpolate any expression directly inside backticks:
```verb
let name = "Verbund Labs";
let count = 42;
print(`Welcome to ${name}, total items: ${count * 2}!`);
```

### 3. Asynchronous Functions
```verb
async fn download_data(endpoint) {
    print(`Fetching ${endpoint}...`);
    return "Data payload received";
}
```

---

## 🎨 HTML & CSS First-Class Features

### 1. Direct Markup AST
Tags are native expressions, not strings:
```verb
let card = <div class="card" id="user-profile">
    <h1 class="heading">"Developer Dashboard"</h1>
    <p>"High performance native rendering."</p>
    <button>"Click to Benchmark"</button>
</div>;
```

### 2. First-Class Stylesheets
```verb
style .card {
    color: "cyan";
    border: "1px solid white";
}

style .heading {
    color: "magenta";
    font-weight: "bold";
}

style button {
    color: "green";
    font-weight: "bold";
}
```

### 3. Rendering Outputs
```verb
# 1. Output standard HTML5 string:
print(render_html(card));

# 2. Output rich ANSI colored terminal UI:
print(render_tui(card));
```

---

## ⚙️ C++ Capabilities & Performance

### 1. Direct Memory Manipulation
Allocate raw memory buffers, write 64-bit values, and inspect pointers:
```verb
let ptr = alloc(64);
poke64(ptr, 0xCAFEBABE);
let value = peek64(ptr);
free(ptr);
```

### 2. RAII Cleanup (`defer`)
Guarantee resource cleanup even on error or early return:
```verb
let buffer = alloc(1024);
defer {
    print("Buffer released!");
    free(buffer);
}
```

### 3. Inline C++ (`native cxx`)
Embed standard C++20 code directly in your `.verb` source file:
```verb
native cxx {
    #include <chrono>
    inline uint64_t get_nanoseconds() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }
}
```

### 4. Microsecond Benchmarking
```verb
let t0 = now_ns();
# Run 1,000,000 operations
let t1 = now_ns();
let elapsed_ms = (t1 - t0) / 1000000.0;
print(`Completed in ${elapsed_ms} ms!`);
```

---

## 🎯 Pattern Matching (`match`)

```verb
match response_code {
    200 => print("OK")
    404 => print("Not Found")
    500 => print("Internal Error")
    _   => print("Unknown Status")
}
```

---

## 📦 JSON & Standard Library

```verb
# JSON serialization & parsing
let payload = { "user": "VerbDev", "active": true };
let json_str = json_stringify(payload);
let restored = json_parse(json_str);

# String utilities
let clean = str_trim("  hello verb  ");
let upper = str_upper(clean);
let parts = str_split(clean, " ");
let joined = str_join(parts, "-");

# Reactive state signals
let count = create_signal(0);
count["subscribe"]((new_val) => print("Counter:", new_val));
count["set"](10); # Triggers subscriber
```

---

## 🛠️ CLI Project Workflow

```bash
# Initialize a new project
verbc new my_project

# Run project entry point
verbc run my_project/src/main.verb

# Format code automatically
verbc fmt my_project/src/main.verb

# Compile to optimized native binary
verbc compile my_project/src/main.verb -o my_app
```

---
© 2026 Verbund Labs.
