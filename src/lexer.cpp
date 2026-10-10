#include "verb/lexer.hpp"
#include <cctype>
#include <unordered_map>

namespace verb {

static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"let", TokenType::Let},
    {"const", TokenType::Const},
    {"fn", TokenType::Fn},
    {"def", TokenType::Def},
    {"return", TokenType::Return},
    {"if", TokenType::If},
    {"elif", TokenType::Elif},
    {"else", TokenType::Else},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"in", TokenType::In},
    {"async", TokenType::Async},
    {"await", TokenType::Await},
    {"defer", TokenType::Defer},
    {"style", TokenType::Style},
    {"native", TokenType::Native},
    {"cxx", TokenType::Cxx},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"nil", TokenType::Nil},
    {"null", TokenType::Null},
    {"and", TokenType::And},
    {"or", TokenType::Or},
    {"not", TokenType::Not},
    {"match", TokenType::Match},
    {"import", TokenType::Import},
};

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

char Lexer::peek(size_t offset) const {
    if (cursor_ + offset >= source_.size()) return '\0';
    return source_[cursor_ + offset];
}

char Lexer::advance() {
    if (is_at_end()) return '\0';
    char c = source_[cursor_++];
    if (c == '\n') {
        line_++;
        col_ = 1;
    } else {
        col_++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (is_at_end() || source_[cursor_] != expected) return false;
    advance();
    return true;
}

bool Lexer::is_at_end() const {
    return cursor_ >= source_.size();
}

void Lexer::skip_whitespace_and_comments() {
    while (!is_at_end()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '#' || (c == '/' && peek(1) == '/')) {
            // Line comment (# or //)
            while (!is_at_end() && peek() != '\n') {
                advance();
            }
        } else if (c == '/' && peek(1) == '*') {
            // Block comment /* ... */
            advance(); advance();
            while (!is_at_end() && !(peek() == '*' && peek(1) == '/')) {
                advance();
            }
            if (!is_at_end()) {
                advance(); advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::lex_number() {
    size_t start_col = col_;
    size_t start_line = line_;
    std::string num_str;

    if (peek() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
        num_str += advance();
        num_str += advance();
        while (std::isxdigit(static_cast<unsigned char>(peek()))) {
            num_str += advance();
        }
        return Token{TokenType::IntLit, num_str, {start_line, start_col}};
    }

    bool has_dot = false;
    while (std::isdigit(static_cast<unsigned char>(peek())) || (peek() == '.' && std::isdigit(static_cast<unsigned char>(peek(1))))) {
        if (peek() == '.') {
            if (has_dot) break;
            has_dot = true;
        }
        num_str += advance();
    }

    return Token{has_dot ? TokenType::FloatLit : TokenType::IntLit, num_str, {start_line, start_col}};
}

Token Lexer::lex_identifier_or_keyword() {
    size_t start_col = col_;
    size_t start_line = line_;
    std::string text;

    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
        text += advance();
    }

    auto it = KEYWORDS.find(text);
    if (it != KEYWORDS.end()) {
        return Token{it->second, text, {start_line, start_col}};
    }
    return Token{TokenType::Ident, text, {start_line, start_col}};
}

Token Lexer::lex_string(char quote) {
    size_t start_col = col_;
    size_t start_line = line_;
    std::string text;
    advance(); // Consume open quote

    while (!is_at_end() && peek() != quote) {
        if (peek() == '\\') {
            advance();
            char esc = advance();
            if (esc == 'n') text += '\n';
            else if (esc == 't') text += '\t';
            else if (esc == 'r') text += '\r';
            else if (esc == '\\') text += '\\';
            else if (esc == '\"') text += '\"';
            else if (esc == '\'') text += '\'';
            else text += esc;
        } else {
            text += advance();
        }
    }

    if (!is_at_end()) {
        advance(); // Consume close quote
    }

    return Token{TokenType::StringLit, text, {start_line, start_col}};
}

Token Lexer::lex_template_string() {
    size_t start_col = col_;
    size_t start_line = line_;
    std::string text;
    advance(); // Consume `

    while (!is_at_end() && peek() != '`') {
        if (peek() == '\\') {
            advance();
            text += advance();
        } else {
            text += advance();
        }
    }

    if (!is_at_end()) {
        advance(); // Consume `
    }

    return Token{TokenType::TemplateLit, text, {start_line, start_col}};
}

Token Lexer::lex_raw_cxx_block() {
    size_t start_col = col_;
    size_t start_line = line_;
    skip_whitespace_and_comments();

    if (peek() != '{') {
        return Token{TokenType::Error, "Expected '{' after native cxx", {start_line, start_col}};
    }
    advance(); // Consume '{'

    int brace_depth = 1;
    std::string code;
    while (!is_at_end() && brace_depth > 0) {
        char c = peek();
        if (c == '{') {
            brace_depth++;
            code += advance();
        } else if (c == '}') {
            brace_depth--;
            if (brace_depth == 0) {
                advance(); // Consume outer '}'
                break;
            }
            code += advance();
        } else {
            code += advance();
        }
    }

    return Token{TokenType::RawCxxLit, code, {start_line, start_col}};
}

std::vector<Token> Lexer::tokenize() {
    tokens_.clear();

    while (!is_at_end()) {
        skip_whitespace_and_comments();
        if (is_at_end()) break;

        size_t start_col = col_;
        size_t start_line = line_;
        char c = peek();

        // Check if inside HTML tag and encountering closing delimiters
        if (inside_html_tag_) {
            if (c == '/' && peek(1) == '>') {
                advance(); advance();
                tokens_.push_back(Token{TokenType::HtmlSelfClose, "/>", {start_line, start_col}});
                inside_html_tag_ = false;
                continue;
            }
            if (c == '>') {
                advance();
                tokens_.push_back(Token{TokenType::HtmlEndTag, ">", {start_line, start_col}});
                inside_html_tag_ = false;
                continue;
            }
        }

        // HTML Closing tag </tag>
        if (c == '<' && peek(1) == '/') {
            advance(); advance(); // consume </
            std::string tag_name;
            while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '-' || peek() == '_') {
                tag_name += advance();
            }
            skip_whitespace_and_comments();
            if (peek() == '>') {
                advance();
            }
            tokens_.push_back(Token{TokenType::HtmlCloseTag, tag_name, {start_line, start_col}});
            continue;
        }

        // HTML Opening tag <tag ...>
        // An open tag starts with `<` followed by [a-zA-Z]
        // Determine whether this is an HTML tag or a less-than `<` operator
        bool can_be_html_tag = false;
        if (c == '<' && std::isalpha(static_cast<unsigned char>(peek(1)))) {
            if (tokens_.empty()) {
                can_be_html_tag = true;
            } else {
                TokenType prev = tokens_.back().type;
                if (prev == TokenType::Assign || prev == TokenType::Return ||
                    prev == TokenType::LParen || prev == TokenType::LBracket ||
                    prev == TokenType::LBrace || prev == TokenType::RBrace ||
                    prev == TokenType::Comma || prev == TokenType::Colon ||
                    prev == TokenType::Semicolon || prev == TokenType::Arrow ||
                    prev == TokenType::HtmlEndTag || prev == TokenType::HtmlCloseTag ||
                    prev == TokenType::HtmlSelfClose) {
                    can_be_html_tag = true;
                }
            }
        }

        if (can_be_html_tag) {
            advance(); // consume '<'
            std::string tag_name;
            while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '-' || peek() == '_') {
                tag_name += advance();
            }
            tokens_.push_back(Token{TokenType::HtmlOpenTag, tag_name, {start_line, start_col}});
            inside_html_tag_ = true;
            continue;
        }

        // Numbers
        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens_.push_back(lex_number());
            continue;
        }

        // Identifiers & Keywords
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            Token tok = lex_identifier_or_keyword();

            // Check for `native cxx { ... }`
            if (tok.type == TokenType::Cxx && !tokens_.empty() && tokens_.back().type == TokenType::Native) {
                tokens_.pop_back(); // Remove 'native'
                tokens_.push_back(lex_raw_cxx_block());
                continue;
            }

            tokens_.push_back(tok);
            continue;
        }

        // Strings
        if (c == '\"' || c == '\'') {
            tokens_.push_back(lex_string(c));
            continue;
        }

        // Template strings
        if (c == '`') {
            tokens_.push_back(lex_template_string());
            continue;
        }

        // Multi-character & single-character operators
        advance();
        switch (c) {
            case '+':
                if (match('=')) tokens_.push_back(Token{TokenType::PlusAssign, "+=", {start_line, start_col}});
                else tokens_.push_back(Token{TokenType::Plus, "+", {start_line, start_col}});
                break;
            case '-':
                if (match('=')) tokens_.push_back(Token{TokenType::MinusAssign, "-=", {start_line, start_col}});
                else tokens_.push_back(Token{TokenType::Minus, "-", {start_line, start_col}});
                break;
            case '*':
                tokens_.push_back(Token{TokenType::Star, "*", {start_line, start_col}});
                break;
            case '/':
                tokens_.push_back(Token{TokenType::Slash, "/", {start_line, start_col}});
                break;
            case '%':
                tokens_.push_back(Token{TokenType::Percent, "%", {start_line, start_col}});
                break;
            case '=':
                if (match('=')) tokens_.push_back(Token{TokenType::Equal, "==", {start_line, start_col}});
                else if (match('>')) tokens_.push_back(Token{TokenType::Arrow, "=>", {start_line, start_col}});
                else tokens_.push_back(Token{TokenType::Assign, "=", {start_line, start_col}});
                break;
            case '!':
                if (match('=')) tokens_.push_back(Token{TokenType::NotEqual, "!=", {start_line, start_col}});
                else tokens_.push_back(Token{TokenType::Bang, "!", {start_line, start_col}});
                break;
            case '<':
                if (match('=')) tokens_.push_back(Token{TokenType::LessEqual, "<=", {start_line, start_col}});
                else tokens_.push_back(Token{TokenType::Less, "<", {start_line, start_col}});
                break;
            case '>':
                if (match('=')) tokens_.push_back(Token{TokenType::GreaterEqual, ">=", {start_line, start_col}});
                else tokens_.push_back(Token{TokenType::Greater, ">", {start_line, start_col}});
                break;
            case '&':
                if (match('&')) tokens_.push_back(Token{TokenType::AndAnd, "&&", {start_line, start_col}});
                break;
            case '|':
                if (match('|')) tokens_.push_back(Token{TokenType::OrOr, "||", {start_line, start_col}});
                break;
            case '?':
                if (match('.')) tokens_.push_back(Token{TokenType::QuestionDot, "?.", {start_line, start_col}});
                else if (match('?')) tokens_.push_back(Token{TokenType::QuestionQuestion, "??", {start_line, start_col}});
                break;
            case '(': tokens_.push_back(Token{TokenType::LParen, "(", {start_line, start_col}}); break;
            case ')': tokens_.push_back(Token{TokenType::RParen, ")", {start_line, start_col}}); break;
            case '[': tokens_.push_back(Token{TokenType::LBracket, "[", {start_line, start_col}}); break;
            case ']': tokens_.push_back(Token{TokenType::RBracket, "]", {start_line, start_col}}); break;
            case '{': tokens_.push_back(Token{TokenType::LBrace, "{", {start_line, start_col}}); break;
            case '}': tokens_.push_back(Token{TokenType::RBrace, "}", {start_line, start_col}}); break;
            case ',': tokens_.push_back(Token{TokenType::Comma, ",", {start_line, start_col}}); break;
            case ':': tokens_.push_back(Token{TokenType::Colon, ":", {start_line, start_col}}); break;
            case ';': tokens_.push_back(Token{TokenType::Semicolon, ";", {start_line, start_col}}); break;
            case '.': tokens_.push_back(Token{TokenType::Dot, ".", {start_line, start_col}}); break;
            default:
                break;
        }
    }

    tokens_.push_back(Token{TokenType::Eof, "", {line_, col_}});
    return tokens_;
}

} // namespace verb
