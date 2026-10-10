#pragma once

#include <string>
#include <string_view>
#include <iostream>

namespace verb {

enum class TokenType {
    // End of file / error
    Eof,
    Error,

    // Identifiers & Literals
    Ident,
    IntLit,
    FloatLit,
    StringLit,
    TemplateLit, // `Hello ${name}`
    RawCxxLit,   // content inside native cxx { ... }

    // Keywords
    Let,
    Const,
    Fn,
    Def,
    Return,
    If,
    Elif,
    Else,
    While,
    For,
    In,
    Async,
    Await,
    Defer,
    Style,
    Native,
    Cxx,
    True,
    False,
    Nil,
    Null,
    And,
    Or,
    Not,

    // Operators
    Plus,       // +
    Minus,      // -
    Star,       // *
    Slash,      // /
    Percent,    // %
    Assign,     // =
    PlusAssign, // +=
    MinusAssign,// -=
    Equal,      // ==
    NotEqual,   // !=
    Less,       // <
    LessEqual,  // <=
    Greater,    // >
    GreaterEqual,// >=
    Arrow,      // =>
    AndAnd,     // &&
    OrOr,       // ||
    Bang,       // !
    QuestionDot,// ?.
    QuestionQuestion, // ??

    // Delimiters
    LParen,     // (
    RParen,     // )
    LBracket,   // [
    RBracket,   // ]
    LBrace,     // {
    RBrace,     // }
    Comma,      // ,
    Colon,      // :
    Semicolon,  // ;
    Dot,        // .

    // HTML / JSX
    HtmlOpenTag,      // <tag
    HtmlCloseTag,     // </tag>
    HtmlSelfClose,    // />
    HtmlEndTag,       // >
};

struct SourceLocation {
    size_t line = 1;
    size_t column = 1;
};

struct Token {
    TokenType type = TokenType::Eof;
    std::string text;
    SourceLocation loc;

    std::string to_string() const {
        return text;
    }
};

} // namespace verb
