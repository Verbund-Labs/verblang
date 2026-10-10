#pragma once

#include "verb/token.hpp"
#include <string>
#include <vector>

namespace verb {

class Lexer {
public:
    explicit Lexer(std::string source);

    std::vector<Token> tokenize();

private:
    char peek(size_t offset = 0) const;
    char advance();
    bool match(char expected);
    bool is_at_end() const;
    void skip_whitespace_and_comments();

    Token lex_number();
    Token lex_identifier_or_keyword();
    Token lex_string(char quote);
    Token lex_template_string();
    Token lex_raw_cxx_block();

    std::string source_;
    size_t cursor_ = 0;
    size_t line_ = 1;
    size_t col_ = 1;
    std::vector<Token> tokens_;
    bool inside_html_tag_ = false;
};

} // namespace verb
