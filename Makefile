CXX ?= clang++
CXXFLAGS ?= -std=c++20 -O3 -Wall -Wextra -Iinclude -I../verb-runtime/include
LDFLAGS ?= ../verb-runtime/libverbrt.a -ldl -lpthread

SRCS = src/lexer.cpp src/parser.cpp src/ast.cpp src/codegen_cxx.cpp src/main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = bin/verbc

all: dirs $(TARGET)

dirs:
	mkdir -p bin

$(TARGET): $(OBJS) ../verb-runtime/libverbrt.a
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $@

../verb-runtime/libverbrt.a:
	$(MAKE) -C ../verb-runtime

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: all
	@echo "Running VerbLang Test Suite..."
	./bin/verbc run examples/01_hello_world.verb
	./bin/verbc run examples/02_pythonic_data.verb
	./bin/verbc run examples/03_js_async_lambdas.verb
	./bin/verbc run examples/04_html_css_components.verb
	./bin/verbc run examples/05_cpp_speed_pointers.verb
	./bin/verbc run examples/06_full_stack_app.verb

clean:
	rm -rf bin $(OBJS)

.PHONY: all dirs test clean
