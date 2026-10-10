cmake_minimum_required(VERSION 3.16)
project(verblang VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include ../verb-runtime/include)
link_directories(../verb-runtime)

set(VERB_COMPILER_SRCS
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/codegen_cxx.cpp
    src/main.cpp
)

add_executable(verbc ${VERB_COMPILER_SRCS})
target_link_libraries(verbc PRIVATE verbrt dl pthread)

install(TARGETS verbc DESTINATION bin)
