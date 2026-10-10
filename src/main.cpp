#include "verb/lexer.hpp"
#include "verb/parser.hpp"
#include "verb/codegen_cxx.hpp"
#include "verb/agents_guide.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <unordered_set>

namespace fs = std::filesystem;

void print_banner() {
    std::cout << "\033[1;36m";
    std::cout << "██╗   ██╗███████╗██████╗ ██████╗ ██╗      █████╗ ███╗   ██╗ ██████╗ \n";
    std::cout << "██║   ██║██╔════╝██╔══██╗██╔══██╗██║     ██╔══██╗████╗  ██║██╔════╝ \n";
    std::cout << "██║   ██║█████╗  ██████╔╝██████╔╝██║     ███████║██╔██╗ ██║██║  ███╗\n";
    std::cout << "╚██╗ ██╔╝██╔══╝  ██╔══██╗██╔══██╗██║     ██╔══██║██║╚██╗██║██║   ██║\n";
    std::cout << " ╚████╔╝ ███████╗██║  ██║██████╔╝███████╗██║  ██║██║ ╚████║╚██████╔╝\n";
    std::cout << "  ╚═══╝  ╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ \n";
    std::cout << "\033[0m";
    std::cout << "\033[1;35mVerbLang 1.0.0 (C++20 Native Engine)\033[0m\n";
    std::cout << "Maintained by \033[1;33mVerbund Labs\033[0m\n\n";
}

void print_version() {
    std::cout << "verbc 1.0.0 (x86_64-linux-gnu)\n";
    std::cout << "runtime: libverbrt 1.0.0 (C++20 native engine)\n";
    std::cout << "backend: Clang++ / LLVM\n";
    std::cout << "organization: Verbund Labs\n";
}

void print_help() {
    std::cout << "VerbLang Compiler & Toolchain (verbc)\n";
    std::cout << "Maintained by Verbund Labs\n\n";
    std::cout << "Usage: verbc <command> [options] [arguments]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  run <file.verb> [args...]     Compile and execute a .verb source file\n";
    std::cout << "  compile <file.verb>           Compile a .verb file to a standalone native binary\n";
    std::cout << "  check <file.verb>             Typecheck and parse .verb file without compiling\n";
    std::cout << "  test [path]                   Discover and run .verb test suites\n";
    std::cout << "  emit-cxx <file.verb>          Transpile .verb file to optimized C++20 code\n";
    std::cout << "  new <project-name>            Initialize a new VerbLang project with manifest\n";
    std::cout << "  fmt <file.verb>               Format a .verb source file\n";
    std::cout << "  repl                          Start an interactive VerbLang REPL session\n";
    std::cout << "  version                       Display compiler and runtime version info\n";
    std::cout << "  help                          Display this help message\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <path>                     Specify output binary or C++ file path\n";
    std::cout << "  -O3, -O2, -O0                 Optimization level (default: -O3)\n";
    std::cout << "  --runtime-path <dir>          Path to verb-runtime include directory\n";
    std::cout << "  -v, --verbose                 Enable verbose output\n\n";
    std::cout << "Examples:\n";
    std::cout << "  verbc run app.verb\n";
    std::cout << "  verbc compile app.verb -o my_app\n";
    std::cout << "  verbc check app.verb\n";
    std::cout << "  verbc test tests/\n";
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "error: could not open file: " << path << "\n";
        std::exit(1);
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string find_runtime_include_dir(const std::string& override_path) {
    if (!override_path.empty() && fs::exists(override_path)) {
        return override_path;
    }
    std::vector<std::string> search_paths = {
        "../verb-runtime/include",
        "verb-runtime/include",
        "/usr/local/include/verbrt",
        "/usr/include/verbrt",
        "include"
    };
    for (const auto& p : search_paths) {
        if (fs::exists(p)) {
            return fs::canonical(p).string();
        }
    }
    return "../verb-runtime/include";
}

std::string find_runtime_lib_dir() {
    std::vector<std::string> search_paths = {
        "../verb-runtime",
        "verb-runtime",
        "/usr/local/lib",
        "/usr/lib"
    };
    for (const auto& p : search_paths) {
        if (fs::exists(p + "/libverbrt.a")) {
            return fs::canonical(p).string();
        }
    }
    return "../verb-runtime";
}

// Recursive module / import resolver
std::shared_ptr<verb::Program> parse_and_resolve(const std::string& file_path, std::unordered_set<std::string>& visited) {
    if (!fs::exists(file_path)) {
        throw std::runtime_error("error: file not found: '" + file_path + "'");
    }
    fs::path p = fs::canonical(file_path);
    std::string canonical_str = p.string();
    if (visited.count(canonical_str)) {
        return std::make_shared<verb::Program>();
    }
    visited.insert(canonical_str);

    std::string source = read_file(canonical_str);
    verb::Lexer lexer(source);
    auto tokens = lexer.tokenize();

    verb::Parser parser(tokens, canonical_str, source);
    auto program = parser.parse_program();

    auto combined = std::make_shared<verb::Program>();
    for (const auto& stmt : program->statements) {
        if (auto imp = dynamic_cast<verb::ImportStmt*>(stmt.get())) {
            fs::path imp_path = imp->path;
            fs::path target;
            if (imp_path.is_absolute()) {
                target = imp_path;
            } else {
                target = p.parent_path() / imp_path;
            }
            if (!fs::exists(target) && fs::exists(target.string() + ".verb")) {
                target = target.string() + ".verb";
            }
            if (!fs::exists(target)) {
                throw std::runtime_error("error: imported module not found: '" + imp->path + "'\n  --> " + canonical_str + ":" + std::to_string(imp->loc.line));
            }
            auto imported_prog = parse_and_resolve(target.string(), visited);
            for (auto& s : imported_prog->statements) {
                combined->statements.push_back(std::move(s));
            }
        } else {
            combined->statements.push_back(stmt);
        }
    }
    return combined;
}

int cmd_check(const std::string& file_path) {
    std::unordered_set<std::string> visited;
    auto program = parse_and_resolve(file_path, visited);
    std::cout << "✓ checked " << file_path << " (" << program->statements.size() << " statements, 0 errors)\n";
    return 0;
}

int cmd_emit_cxx(const std::string& file_path, const std::string& out_path) {
    std::unordered_set<std::string> visited;
    auto program = parse_and_resolve(file_path, visited);

    verb::CxxCodeGen codegen;
    std::string cpp_code = codegen.generate(program.get());

    if (!out_path.empty()) {
        std::ofstream out(out_path);
        out << cpp_code;
        std::cout << "✓ emitted C++20 to: " << out_path << "\n";
    } else {
        std::cout << cpp_code;
    }
    return 0;
}

int cmd_compile(const std::string& file_path, std::string out_binary, const std::string& opt_level, const std::string& rt_override, bool silent = false) {
    if (out_binary.empty()) {
        fs::path p(file_path);
        out_binary = p.stem().string();
    }

    std::unordered_set<std::string> visited;
    auto program = parse_and_resolve(file_path, visited);

    verb::CxxCodeGen codegen;
    std::string cpp_code = codegen.generate(program.get());

    // Write temp C++ file
    std::string temp_cpp = "/tmp/verb_" + std::to_string(std::rand()) + ".cpp";
    std::ofstream out(temp_cpp);
    out << cpp_code;
    out.close();

    std::string inc_dir = find_runtime_include_dir(rt_override);
    std::string lib_dir = find_runtime_lib_dir();

    std::string static_lib = lib_dir + "/libverbrt.a";
    std::stringstream cmd;
    cmd << "clang++ -std=c++20 " << opt_level
        << " -I\"" << inc_dir << "\""
        << " \"" << temp_cpp << "\"";
    if (fs::exists(static_lib)) {
        cmd << " \"" << static_lib << "\"";
    } else {
        cmd << " -L\"" << lib_dir << "\" -lverbrt";
    }
    cmd << " -ldl -lpthread -o \"" << out_binary << "\"";

    int ret = std::system(cmd.str().c_str());
    fs::remove(temp_cpp);

    if (ret != 0) {
        if (!silent) {
            std::cerr << "error: native compilation failed with status: " << ret << "\n";
        }
        return ret;
    }

    if (!silent) {
        std::cout << "✓ compiled: " << file_path << " -> " << out_binary << "\n";
    }
    return 0;
}

int cmd_run(const std::string& file_path, const std::vector<std::string>& program_args) {
    std::string temp_bin = "/tmp/verb_run_" + std::to_string(std::rand());
    int compile_res = cmd_compile(file_path, temp_bin, "-O3", "", true /* silent */);
    if (compile_res != 0) {
        return compile_res;
    }

    std::stringstream run_cmd;
    run_cmd << temp_bin;
    for (const auto& arg : program_args) {
        run_cmd << " \"" << arg << "\"";
    }

    int run_res = std::system(run_cmd.str().c_str());
    fs::remove(temp_bin);
    return run_res;
}

int cmd_test(const std::string& target_path) {
    std::vector<std::string> test_files;
    fs::path target = target_path.empty() ? "." : target_path;
    if (fs::is_regular_file(target)) {
        test_files.push_back(target.string());
    } else if (fs::is_directory(target)) {
        for (const auto& entry : fs::recursive_directory_iterator(target)) {
            if (entry.is_regular_file()) {
                std::string fname = entry.path().filename().string();
                if ((fname.rfind("test_", 0) == 0 || fname.find("_test.verb") != std::string::npos) &&
                    entry.path().extension() == ".verb") {
                    test_files.push_back(entry.path().string());
                }
            }
        }
    }

    if (test_files.empty()) {
        std::cout << "No test files matching 'test_*.verb' or '*_test.verb' found.\n";
        return 0;
    }

    std::cout << "running " << test_files.size() << " test" << (test_files.size() > 1 ? "s" : "") << "\n";
    int passed = 0;
    int failed = 0;

    for (const auto& tf : test_files) {
        std::cout << "test " << tf << " ... " << std::flush;
        std::string temp_bin = "/tmp/verb_test_" + std::to_string(std::rand());
        int compile_res = cmd_compile(tf, temp_bin, "-O3", "", true /* silent */);
        if (compile_res != 0) {
            std::cout << "\033[1;31mFAILED (compilation error)\033[0m\n";
            failed++;
            continue;
        }

        int run_res = std::system((temp_bin + " > /dev/null 2>&1").c_str());
        fs::remove(temp_bin);

        if (run_res == 0) {
            std::cout << "\033[1;32mok\033[0m\n";
            passed++;
        } else {
            std::cout << "\033[1;31mFAILED (exit code " << run_res << ")\033[0m\n";
            failed++;
        }
    }

    std::cout << "\ntest result: " << (failed == 0 ? "\033[1;32mok\033[0m" : "\033[1;31mFAILED\033[0m")
              << ". " << passed << " passed; " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}

void cmd_repl() {
    print_banner();
    std::cout << "VerbLang Interactive REPL (type 'exit()' to quit)\n";
    std::cout << "--------------------------------------------------\n";

    std::string line;
    while (true) {
        std::cout << "\033[1;34mverb>\033[0m ";
        if (!std::getline(std::cin, line)) break;
        if (line == "exit()" || line == "quit()") break;
        if (line.empty()) continue;

        try {
            std::string code = line;
            if (line.find("let ") != 0 && line.find("fn ") != 0 && line.find("def ") != 0 &&
                line.find("print") != 0 && line.find("style ") != 0 && line.find("import ") != 0) {
                code = "print(" + line + ")";
            }

            std::string temp_verb = "/tmp/verb_repl_eval.verb";
            std::ofstream f(temp_verb);
            f << code << "\n";
            f.close();

            cmd_run(temp_verb, {});
            fs::remove(temp_verb);
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

int cmd_new(const std::string& project_name) {
    if (project_name.empty()) {
        std::cerr << "Usage: verbc new <project-name>\n";
        return 1;
    }
    fs::path root(project_name);
    if (fs::exists(root)) {
        std::cerr << "error: directory '" << project_name << "' already exists\n";
        return 1;
    }
    fs::create_directories(root / "src");
    fs::create_directories(root / "tests");

    std::ofstream manifest(root / "verb.json");
    manifest << "{\n"
             << "  \"name\": \"" << project_name << "\",\n"
             << "  \"version\": \"0.1.0\",\n"
             << "  \"organization\": \"Verbund Labs\",\n"
             << "  \"entry\": \"src/main.verb\",\n"
             << "  \"license\": \"Apache-2.0\"\n"
             << "}\n";
    manifest.close();

    std::ofstream main_file(root / "src" / "main.verb");
    main_file << "# " << project_name << "\n"
              << "# Maintained by Verbund Labs\n\n"
              << "style .app {\n"
              << "    color: \"cyan\";\n"
              << "    border: \"1px solid #45475a\";\n"
              << "}\n\n"
              << "fn greet(name) {\n"
              << "    return `System initialized for ${name}`;\n"
              << "}\n\n"
              << "let view = <div class=\"app\">\n"
              << "    <h1>`Welcome to " << project_name << "`</h1>\n"
              << "    <p>\"High-performance multi-paradigm systems runtime.\"</p>\n"
              << "</div>;\n\n"
              << "print(render_tui(view));\n"
              << "print(greet(\"Verbund Labs\"));\n";
    main_file.close();

    std::ofstream readme(root / "README.md");
    readme << "# " << project_name << "\n\n"
           << "Systems project built with VerbLang.\n\n"
           << "## Build & Run\n"
           << "```bash\n"
           << "# Run directly\n"
           << "verbc run src/main.verb\n\n"
           << "# Check syntax\n"
           << "verbc check src/main.verb\n\n"
           << "# Compile to native binary\n"
           << "verbc compile src/main.verb -o " << project_name << "\n"
           << "./" << project_name << "\n"
           << "```\n";
    readme.close();

    std::ofstream agents_file(root / "AGENTS.md");
    agents_file << verb::AGENTS_GUIDE_TEMPLATE;
    agents_file.close();

    std::cout << "✓ created new project: " << project_name << "\n";
    std::cout << "  entry: " << project_name << "/src/main.verb\n";
    std::cout << "  guide: " << project_name << "/AGENTS.md\n";
    std::cout << "  run:   verbc run " << project_name << "/src/main.verb\n";
    return 0;
}

int cmd_fmt(const std::string& file_path) {
    std::string source = read_file(file_path);
    std::istringstream stream(source);
    std::string line;
    std::stringstream formatted;
    int indent = 0;

    while (std::getline(stream, line)) {
        size_t first = line.find_first_not_of(" \t");
        if (first == std::string::npos) {
            formatted << "\n";
            continue;
        }
        std::string trimmed = line.substr(first);

        if (!trimmed.empty() && (trimmed[0] == '}' || trimmed.substr(0, 2) == "</")) {
            indent = std::max(0, indent - 1);
        }

        formatted << std::string(indent * 4, ' ') << trimmed << "\n";

        if (!trimmed.empty() && trimmed.back() == '{') {
            indent++;
        }
    }

    std::ofstream out(file_path);
    out << formatted.str();
    std::cout << "✓ formatted: " << file_path << "\n";
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    try {
        std::string command = argv[1];

        if (command == "help" || command == "--help" || command == "-h") {
            print_help();
            return 0;
        }

        if (command == "version" || command == "--version" || command == "-v") {
            print_version();
            return 0;
        }

        if (command == "banner") {
            print_banner();
            return 0;
        }

        if (command == "repl") {
            cmd_repl();
            return 0;
        }

        if (command == "check") {
            if (argc < 3) {
                std::cerr << "Usage: verbc check <file.verb>\n";
                return 1;
            }
            return cmd_check(argv[2]);
        }

        if (command == "test") {
            std::string path = argc >= 3 ? argv[2] : "";
            return cmd_test(path);
        }

        if (command == "new") {
            if (argc < 3) {
                std::cerr << "Usage: verbc new <project-name>\n";
                return 1;
            }
            return cmd_new(argv[2]);
        }

        if (command == "fmt") {
            if (argc < 3) {
                std::cerr << "Usage: verbc fmt <file.verb>\n";
                return 1;
            }
            return cmd_fmt(argv[2]);
        }

        if (command == "run") {
            if (argc < 3) {
                std::cerr << "Usage: verbc run <file.verb> [args...]\n";
                return 1;
            }
            std::string file_path = argv[2];
            std::vector<std::string> program_args;
            for (int i = 3; i < argc; ++i) {
                program_args.push_back(argv[i]);
            }
            return cmd_run(file_path, program_args);
        }

        if (command == "compile") {
            if (argc < 3) {
                std::cerr << "Usage: verbc compile <file.verb> [-o <out_binary>] [-O3]\n";
                return 1;
            }
            std::string file_path = argv[2];
            std::string out_binary = "";
            std::string opt_level = "-O3";
            std::string rt_path = "";

            for (int i = 3; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "-o" && i + 1 < argc) {
                    out_binary = argv[++i];
                } else if (arg == "-O0" || arg == "-O1" || arg == "-O2" || arg == "-O3") {
                    opt_level = arg;
                } else if (arg == "--runtime-path" && i + 1 < argc) {
                    rt_path = argv[++i];
                }
            }
            return cmd_compile(file_path, out_binary, opt_level, rt_path, false);
        }

        if (command == "emit-cxx") {
            if (argc < 3) {
                std::cerr << "Usage: verbc emit-cxx <file.verb> [-o <out.cpp>]\n";
                return 1;
            }
            std::string file_path = argv[2];
            std::string out_cpp = "";
            for (int i = 3; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "-o" && i + 1 < argc) {
                    out_cpp = argv[++i];
                }
            }
            return cmd_emit_cxx(file_path, out_cpp);
        }

        // Default: treat argument as file to run
        if (fs::exists(command)) {
            std::vector<std::string> program_args;
            for (int i = 2; i < argc; ++i) {
                program_args.push_back(argv[i]);
            }
            return cmd_run(command, program_args);
        }

        std::cerr << "error: unrecognized command '" << command << "'\n\n";
        print_help();
        return 1;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
