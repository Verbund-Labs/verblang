#include "verb/parser.hpp"
#include "verb/lexer.hpp"
#include <iostream>
#include <sstream>

namespace verb {

Parser::Parser(std::vector<Token> tokens, std::string source_file, std::string source_code)
    : tokens_(std::move(tokens)), source_file_(std::move(source_file)), source_code_(std::move(source_code)) {}

std::string Parser::format_diagnostic(const std::string& err_msg, const SourceLocation& loc) const {
    std::stringstream ss;
    ss << "\033[1;31merror:\033[0m " << err_msg << "\n";
    ss << "  \033[1;34m-->\033[0m " << source_file_ << ":" << loc.line << ":" << loc.column << "\n";

    if (!source_code_.empty()) {
        std::istringstream stream(source_code_);
        std::string line_str;
        size_t current_line = 1;
        while (std::getline(stream, line_str)) {
            if (current_line == loc.line) {
                ss << "   \033[1;34m|\033[0m\n";
                ss << " " << loc.line << " \033[1;34m|\033[0m " << line_str << "\n";
                ss << "   \033[1;34m|\033[0m ";
                for (size_t i = 1; i < loc.column; ++i) {
                    ss << (line_str[i - 1] == '\t' ? '\t' : ' ');
                }
                ss << "\033[1;31m^\033[0m\n";
                break;
            }
            current_line++;
        }
    }
    return ss.str();
}

const Token& Parser::peek(size_t offset) const {
    if (cursor_ + offset >= tokens_.size()) return tokens_.back();
    return tokens_[cursor_ + offset];
}

const Token& Parser::previous() const {
    if (cursor_ == 0) return tokens_[0];
    return tokens_[cursor_ - 1];
}

bool Parser::is_at_end() const {
    return peek().type == TokenType::Eof;
}

Token Parser::advance() {
    if (!is_at_end()) cursor_++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (is_at_end()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& err_msg) {
    if (check(type)) return advance();
    std::string msg = format_diagnostic(err_msg + " (found '" + peek().text + "')", peek().loc);
    throw std::runtime_error(msg);
}

void Parser::skip_semicolons() {
    while (match(TokenType::Semicolon)) {}
}

std::shared_ptr<Program> Parser::parse_program() {
    auto prog = std::make_shared<Program>();
    while (!is_at_end()) {
        skip_semicolons();
        if (is_at_end()) break;
        prog->statements.push_back(parse_statement());
        skip_semicolons();
    }
    return prog;
}

std::shared_ptr<Stmt> Parser::parse_statement() {
    if (match(TokenType::Import)) return parse_import();
    if (match(TokenType::Let)) return parse_var_decl(false);
    if (match(TokenType::Const)) return parse_var_decl(true);
    if (match(TokenType::Fn) || match(TokenType::Def)) return parse_function_decl(false);
    if (check(TokenType::Async) && (peek(1).type == TokenType::Fn || peek(1).type == TokenType::Def)) {
        advance(); // consume async
        advance(); // consume fn/def
        return parse_function_decl(true);
    }
    if (match(TokenType::Return)) return parse_return();
    if (match(TokenType::If)) return parse_if();
    if (match(TokenType::While)) return parse_while();
    if (match(TokenType::For)) return parse_for_in();
    if (match(TokenType::Defer)) return parse_defer();
    if (match(TokenType::Match)) return parse_match();
    if (check(TokenType::RawCxxLit)) {
        Token tok = advance();
        return std::make_shared<NativeCxxStmt>(tok.text);
    }
    if (match(TokenType::Style)) return parse_style_decl();
    if (check(TokenType::LBrace)) return parse_block();

    // Expression statement or Assignment
    std::shared_ptr<Expr> expr = parse_expression();

    if (match(TokenType::Assign) || match(TokenType::PlusAssign) || match(TokenType::MinusAssign)) {
        std::string op = previous().text;
        std::shared_ptr<Expr> val = parse_expression();
        auto assign = std::make_shared<AssignStmt>();
        assign->target = expr;
        assign->op = op;
        assign->value = val;
        skip_semicolons();
        return assign;
    }

    skip_semicolons();
    return std::make_shared<ExprStmt>(expr);
}

std::shared_ptr<ImportStmt> Parser::parse_import() {
    Token path_tok = consume(TokenType::StringLit, "expected module or file path string after 'import'");
    skip_semicolons();
    auto stmt = std::make_shared<ImportStmt>(path_tok.text);
    stmt->loc = path_tok.loc;
    return stmt;
}

std::shared_ptr<VarDeclStmt> Parser::parse_var_decl(bool is_const) {
    Token name_tok = consume(TokenType::Ident, "Expected variable name after let/const");
    auto decl = std::make_shared<VarDeclStmt>();
    decl->is_const = is_const;
    decl->name = name_tok.text;

    // Optional type annotation: `let x: int = 10;`
    if (match(TokenType::Colon)) {
        Token type_tok = consume(TokenType::Ident, "Expected type name after ':'");
        decl->type_annotation = type_tok.text;
    }

    if (match(TokenType::Assign)) {
        decl->initializer = parse_expression();
    } else {
        decl->initializer = std::make_shared<NilLiteralExpr>();
    }

    skip_semicolons();
    return decl;
}

std::shared_ptr<FunctionDeclStmt> Parser::parse_function_decl(bool is_async) {
    Token name_tok = consume(TokenType::Ident, "Expected function name");
    consume(TokenType::LParen, "Expected '(' after function name");

    auto fn_decl = std::make_shared<FunctionDeclStmt>();
    fn_decl->name = name_tok.text;
    fn_decl->is_async = is_async;

    if (!check(TokenType::RParen)) {
        do {
            Token param = consume(TokenType::Ident, "Expected parameter name");
            // Optional parameter type annotation
            if (match(TokenType::Colon)) {
                consume(TokenType::Ident, "Expected parameter type");
            }
            fn_decl->params.push_back(param.text);
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RParen, "Expected ')' after parameter list");

    // Optional return type annotation
    if (match(TokenType::Colon)) {
        if (check(TokenType::Ident)) {
            advance();
        }
    }

    fn_decl->body = parse_block();
    return fn_decl;
}

std::shared_ptr<ReturnStmt> Parser::parse_return() {
    auto ret = std::make_shared<ReturnStmt>();
    if (!check(TokenType::Semicolon) && !check(TokenType::RBrace) && !is_at_end()) {
        ret->value = parse_expression();
    } else {
        ret->value = std::make_shared<NilLiteralExpr>();
    }
    skip_semicolons();
    return ret;
}

std::shared_ptr<BlockStmt> Parser::parse_block() {
    consume(TokenType::LBrace, "Expected '{' to start block");
    auto block = std::make_shared<BlockStmt>();
    while (!check(TokenType::RBrace) && !is_at_end()) {
        skip_semicolons();
        if (check(TokenType::RBrace) || is_at_end()) break;
        block->statements.push_back(parse_statement());
        skip_semicolons();
    }
    consume(TokenType::RBrace, "Expected '}' to end block");
    return block;
}

std::shared_ptr<IfStmt> Parser::parse_if() {
    auto if_stmt = std::make_shared<IfStmt>();
    bool has_paren = match(TokenType::LParen);
    if_stmt->condition = parse_expression();
    if (has_paren) consume(TokenType::RParen, "Expected ')' after if condition");
    match(TokenType::Colon); // optional python colon
    if_stmt->then_branch = parse_block();

    while (match(TokenType::Elif)) {
        bool elif_paren = match(TokenType::LParen);
        auto cond = parse_expression();
        if (elif_paren) consume(TokenType::RParen, "Expected ')' after elif condition");
        match(TokenType::Colon);
        auto body = parse_block();
        if_stmt->elif_branches.push_back({cond, body});
    }

    if (match(TokenType::Else)) {
        match(TokenType::Colon);
        if_stmt->else_branch = parse_block();
    }

    return if_stmt;
}

std::shared_ptr<WhileStmt> Parser::parse_while() {
    auto while_stmt = std::make_shared<WhileStmt>();
    bool has_paren = match(TokenType::LParen);
    while_stmt->condition = parse_expression();
    if (has_paren) consume(TokenType::RParen, "Expected ')' after while condition");
    match(TokenType::Colon);
    while_stmt->body = parse_block();
    return while_stmt;
}

std::shared_ptr<ForInStmt> Parser::parse_for_in() {
    auto for_stmt = std::make_shared<ForInStmt>();
    Token var_tok = consume(TokenType::Ident, "Expected loop variable name after for");
    for_stmt->var_name = var_tok.text;
    consume(TokenType::In, "Expected 'in' in for loop");
    for_stmt->iterable = parse_expression();
    match(TokenType::Colon);
    for_stmt->body = parse_block();
    return for_stmt;
}

std::shared_ptr<DeferStmt> Parser::parse_defer() {
    auto defer_stmt = std::make_shared<DeferStmt>();
    defer_stmt->body = parse_block();
    return defer_stmt;
}

std::shared_ptr<MatchStmt> Parser::parse_match() {
    auto match_stmt = std::make_shared<MatchStmt>();
    bool has_paren = match(TokenType::LParen);
    match_stmt->value = parse_expression();
    if (has_paren) consume(TokenType::RParen, "Expected ')' after match expression");
    match(TokenType::Colon);

    consume(TokenType::LBrace, "Expected '{' to start match block");
    while (!check(TokenType::RBrace) && !is_at_end()) {
        skip_semicolons();
        if (check(TokenType::RBrace) || is_at_end()) break;

        MatchCase c;
        if (check(TokenType::Ident) && peek().text == "_") {
            advance(); // consume '_'
            c.pattern = nullptr;
        } else {
            c.pattern = parse_expression();
        }

        consume(TokenType::Arrow, "Expected '=>' after match pattern");

        if (check(TokenType::LBrace)) {
            c.body = parse_block();
        } else {
            c.body = parse_statement();
        }
        match_stmt->cases.push_back(c);
        skip_semicolons();
    }
    consume(TokenType::RBrace, "Expected '}' to end match block");
    return match_stmt;
}

std::shared_ptr<StyleDeclStmt> Parser::parse_style_decl() {
    auto style_stmt = std::make_shared<StyleDeclStmt>();
    std::string selector;

    if (match(TokenType::Dot)) {
        Token cls = consume(TokenType::Ident, "Expected class name after '.'");
        selector = "." + cls.text;
        while (match(TokenType::Minus)) {
            if (check(TokenType::Ident)) {
                selector += "-" + advance().text;
            }
        }
    } else if (match(TokenType::Ident)) {
        selector = previous().text;
        while (match(TokenType::Minus)) {
            if (check(TokenType::Ident)) {
                selector += "-" + advance().text;
            }
        }
    } else if (match(TokenType::StringLit)) {
        selector = previous().text;
    } else {
        selector = "*";
    }
    style_stmt->selector = selector;

    consume(TokenType::LBrace, "Expected '{' to start style block");
    while (!check(TokenType::RBrace) && !is_at_end()) {
        std::string prop_name;
        if (check(TokenType::Ident) || check(TokenType::StringLit)) {
            prop_name = advance().text;
            while (match(TokenType::Minus)) {
                if (check(TokenType::Ident)) {
                    prop_name += "-" + advance().text;
                }
            }
        } else {
            throw std::runtime_error(format_diagnostic("expected style property name", peek().loc));
        }
        consume(TokenType::Colon, "Expected ':' after style property");
        auto val = parse_expression();
        style_stmt->properties.push_back({prop_name, val});
        skip_semicolons();
    }
    consume(TokenType::RBrace, "Expected '}' to end style block");
    return style_stmt;
}

// Expressions
std::shared_ptr<Expr> Parser::parse_expression() {
    return parse_arrow_or_assignment();
}

std::shared_ptr<Expr> Parser::parse_arrow_or_assignment() {
    // Check for arrow function `(a, b) => expr` or `x => expr`
    if (check(TokenType::Ident) && peek(1).type == TokenType::Arrow) {
        Token param = advance();
        advance(); // consume =>
        auto arrow = std::make_shared<ArrowFnExpr>();
        arrow->params.push_back(param.text);
        if (check(TokenType::LBrace)) {
            arrow->body = parse_block();
        } else {
            arrow->body = parse_expression();
        }
        return arrow;
    }

    if (check(TokenType::LParen)) {
        // Lookahead to check if this is `(...) =>`
        size_t lookahead = 1;
        bool is_arrow = false;
        while (cursor_ + lookahead < tokens_.size()) {
            if (tokens_[cursor_ + lookahead].type == TokenType::RParen) {
                if (cursor_ + lookahead + 1 < tokens_.size() &&
                    tokens_[cursor_ + lookahead + 1].type == TokenType::Arrow) {
                    is_arrow = true;
                }
                break;
            }
            lookahead++;
        }

        if (is_arrow) {
            advance(); // consume '('
            auto arrow = std::make_shared<ArrowFnExpr>();
            if (!check(TokenType::RParen)) {
                do {
                    Token p = consume(TokenType::Ident, "Expected parameter name in arrow function");
                    arrow->params.push_back(p.text);
                } while (match(TokenType::Comma));
            }
            consume(TokenType::RParen, "Expected ')' in arrow function");
            consume(TokenType::Arrow, "Expected '=>'");
            if (check(TokenType::LBrace)) {
                arrow->body = parse_block();
            } else {
                arrow->body = parse_expression();
            }
            return arrow;
        }
    }

    return parse_logical_or();
}

std::shared_ptr<Expr> Parser::parse_logical_or() {
    auto left = parse_logical_and();
    while (match(TokenType::OrOr) || match(TokenType::Or)) {
        std::string op = previous().text;
        auto right = parse_logical_and();
        left = std::make_shared<BinaryExpr>(op, left, right);
    }
    return left;
}

std::shared_ptr<Expr> Parser::parse_logical_and() {
    auto left = parse_equality();
    while (match(TokenType::AndAnd) || match(TokenType::And)) {
        std::string op = previous().text;
        auto right = parse_equality();
        left = std::make_shared<BinaryExpr>(op, left, right);
    }
    return left;
}

std::shared_ptr<Expr> Parser::parse_equality() {
    auto left = parse_comparison();
    while (match(TokenType::Equal) || match(TokenType::NotEqual)) {
        std::string op = previous().text;
        auto right = parse_comparison();
        left = std::make_shared<BinaryExpr>(op, left, right);
    }
    return left;
}

std::shared_ptr<Expr> Parser::parse_comparison() {
    auto left = parse_additive();
    while (match(TokenType::Less) || match(TokenType::LessEqual) ||
           match(TokenType::Greater) || match(TokenType::GreaterEqual)) {
        std::string op = previous().text;
        auto right = parse_additive();
        left = std::make_shared<BinaryExpr>(op, left, right);
    }
    return left;
}

std::shared_ptr<Expr> Parser::parse_additive() {
    auto left = parse_multiplicative();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        std::string op = previous().text;
        auto right = parse_multiplicative();
        left = std::make_shared<BinaryExpr>(op, left, right);
    }
    return left;
}

std::shared_ptr<Expr> Parser::parse_multiplicative() {
    auto left = parse_unary();
    while (match(TokenType::Star) || match(TokenType::Slash) || match(TokenType::Percent)) {
        std::string op = previous().text;
        auto right = parse_unary();
        left = std::make_shared<BinaryExpr>(op, left, right);
    }
    return left;
}

std::shared_ptr<Expr> Parser::parse_unary() {
    if (match(TokenType::Bang) || match(TokenType::Not) || match(TokenType::Minus)) {
        std::string op = previous().text;
        auto operand = parse_unary();
        return std::make_shared<UnaryExpr>(op, operand);
    }
    if (match(TokenType::Await)) {
        auto expr = parse_unary();
        return std::make_shared<AwaitExpr>(expr);
    }
    return parse_postfix();
}

std::shared_ptr<Expr> Parser::parse_postfix() {
    auto expr = parse_primary();

    while (true) {
        if (match(TokenType::LParen)) {
            // Function call
            std::vector<std::shared_ptr<Expr>> args;
            if (!check(TokenType::RParen)) {
                do {
                    args.push_back(parse_expression());
                } while (match(TokenType::Comma));
            }
            consume(TokenType::RParen, "Expected ')' after arguments");
            expr = std::make_shared<CallExpr>(expr, std::move(args));
        } else if (match(TokenType::LBracket)) {
            // Indexing or slicing: arr[i], arr[0:5], arr[0:5:2]
            std::shared_ptr<Expr> start = nullptr;
            std::shared_ptr<Expr> end = nullptr;
            std::shared_ptr<Expr> step = nullptr;

            if (match(TokenType::Colon)) {
                // Slicing without start: arr[:5]
                if (!check(TokenType::RBracket) && !check(TokenType::Colon)) {
                    end = parse_expression();
                }
                if (match(TokenType::Colon)) {
                    step = parse_expression();
                }
                consume(TokenType::RBracket, "Expected ']' after slice");
                expr = std::make_shared<SliceExpr>(expr, start, end, step);
            } else {
                start = parse_expression();
                if (match(TokenType::Colon)) {
                    // Slicing: arr[start:end]
                    if (!check(TokenType::RBracket) && !check(TokenType::Colon)) {
                        end = parse_expression();
                    }
                    if (match(TokenType::Colon)) {
                        step = parse_expression();
                    }
                    consume(TokenType::RBracket, "Expected ']' after slice");
                    expr = std::make_shared<SliceExpr>(expr, start, end, step);
                } else {
                    consume(TokenType::RBracket, "Expected ']' after index");
                    expr = std::make_shared<IndexExpr>(expr, start);
                }
            }
        } else if (match(TokenType::Dot)) {
            // Member access: obj.field -> desugared to index
            Token member = consume(TokenType::Ident, "Expected member name after '.'");
            auto member_name = std::make_shared<StringLiteralExpr>(member.text);
            expr = std::make_shared<IndexExpr>(expr, member_name);
        } else {
            break;
        }
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parse_primary() {
    if (match(TokenType::IntLit)) {
        int64_t val = std::stoll(previous().text, nullptr, 0);
        return std::make_shared<IntLiteralExpr>(val);
    }
    if (match(TokenType::FloatLit)) {
        double val = std::stod(previous().text);
        return std::make_shared<FloatLiteralExpr>(val);
    }
    if (match(TokenType::StringLit)) {
        return std::make_shared<StringLiteralExpr>(previous().text);
    }
    if (match(TokenType::True)) {
        return std::make_shared<BoolLiteralExpr>(true);
    }
    if (match(TokenType::False)) {
        return std::make_shared<BoolLiteralExpr>(false);
    }
    if (match(TokenType::Nil) || match(TokenType::Null)) {
        return std::make_shared<NilLiteralExpr>();
    }
    if (match(TokenType::Ident)) {
        return std::make_shared<IdentifierExpr>(previous().text);
    }
    if (match(TokenType::TemplateLit)) {
        return parse_template_literal(previous().text);
    }
    if (check(TokenType::HtmlOpenTag)) {
        return parse_html_element();
    }
    if (check(TokenType::LBracket)) {
        return parse_bracket_expression();
    }
    if (check(TokenType::LBrace)) {
        return parse_dict_literal();
    }
    if (match(TokenType::LParen)) {
        auto expr = parse_expression();
        consume(TokenType::RParen, "Expected ')' after expression");
        return expr;
    }

    throw std::runtime_error(format_diagnostic("unexpected token in expression: '" + peek().text + "'", peek().loc));
}

std::shared_ptr<HtmlElementExpr> Parser::parse_html_element() {
    Token open_tag = consume(TokenType::HtmlOpenTag, "Expected HTML open tag");
    auto elem = std::make_shared<HtmlElementExpr>();
    elem->tag = open_tag.text;

    // Parse attributes until `>` or `/>`
    while (!check(TokenType::HtmlEndTag) && !check(TokenType::HtmlSelfClose) && !is_at_end()) {
        Token attr_name = consume(TokenType::Ident, "Expected attribute name");
        std::shared_ptr<Expr> attr_val = std::make_shared<BoolLiteralExpr>(true); // boolean attr default
        if (match(TokenType::Assign)) {
            if (check(TokenType::StringLit)) {
                attr_val = std::make_shared<StringLiteralExpr>(advance().text);
            } else if (match(TokenType::LBrace)) {
                attr_val = parse_expression();
                consume(TokenType::RBrace, "Expected '}' after attribute expression");
            } else {
                attr_val = parse_primary();
            }
        }
        elem->attributes.push_back({attr_name.text, attr_val});
    }

    if (match(TokenType::HtmlSelfClose)) {
        return elem;
    }

    consume(TokenType::HtmlEndTag, "Expected '>' to close open tag");

    // Parse children until `</tag>`
    while (!check(TokenType::HtmlCloseTag) && !is_at_end()) {
        if (check(TokenType::HtmlOpenTag)) {
            elem->children.push_back(parse_html_element());
        } else if (match(TokenType::StringLit)) {
            elem->children.push_back(std::make_shared<StringLiteralExpr>(previous().text));
        } else if (match(TokenType::LBrace)) {
            elem->children.push_back(parse_expression());
            consume(TokenType::RBrace, "Expected '}' after expression in HTML child");
        } else if (check(TokenType::TemplateLit)) {
            elem->children.push_back(parse_template_literal(advance().text));
        } else {
            elem->children.push_back(parse_expression());
        }
    }

    consume(TokenType::HtmlCloseTag, "Expected closing tag for <" + elem->tag + ">");
    return elem;
}

std::shared_ptr<Expr> Parser::parse_bracket_expression() {
    consume(TokenType::LBracket, "Expected '['");
    if (match(TokenType::RBracket)) {
        return std::make_shared<ListLiteralExpr>(); // empty list
    }

    auto first_expr = parse_expression();

    // Check for list comprehension: `[x * 2 for x in items if x > 0]`
    if (match(TokenType::For)) {
        auto comp = std::make_shared<ListCompExpr>();
        comp->item_expr = first_expr;
        Token var_tok = consume(TokenType::Ident, "Expected variable name in list comprehension");
        comp->var_name = var_tok.text;
        consume(TokenType::In, "Expected 'in' in list comprehension");
        comp->iterable_expr = parse_expression();

        if (match(TokenType::If)) {
            comp->condition = parse_expression();
        }

        consume(TokenType::RBracket, "Expected ']' at end of list comprehension");
        return comp;
    }

    // Standard list literal
    auto list = std::make_shared<ListLiteralExpr>();
    list->elements.push_back(first_expr);
    while (match(TokenType::Comma)) {
        if (check(TokenType::RBracket)) break;
        list->elements.push_back(parse_expression());
    }
    consume(TokenType::RBracket, "Expected ']' at end of list");
    return list;
}

std::shared_ptr<DictLiteralExpr> Parser::parse_dict_literal() {
    consume(TokenType::LBrace, "Expected '{'");
    auto dict = std::make_shared<DictLiteralExpr>();

    if (match(TokenType::RBrace)) {
        return dict; // empty dict
    }

    do {
        std::string key;
        if (match(TokenType::StringLit) || match(TokenType::Ident)) {
            key = previous().text;
        } else {
            throw std::runtime_error(format_diagnostic("expected string or identifier key in dictionary literal", peek().loc));
        }
        consume(TokenType::Colon, "Expected ':' after dict key");
        auto val = parse_expression();
        dict->entries.push_back({key, val});
    } while (match(TokenType::Comma) && !check(TokenType::RBrace));

    consume(TokenType::RBrace, "Expected '}' at end of dict");
    return dict;
}

std::shared_ptr<TemplateLiteralExpr> Parser::parse_template_literal(const std::string& raw) {
    auto tmpl = std::make_shared<TemplateLiteralExpr>();
    std::string current_str;
    size_t i = 0;

    while (i < raw.size()) {
        if (raw[i] == '$' && i + 1 < raw.size() && raw[i + 1] == '{') {
            tmpl->string_parts.push_back(current_str);
            current_str.clear();
            i += 2; // skip ${

            std::string expr_str;
            int depth = 1;
            while (i < raw.size() && depth > 0) {
                if (raw[i] == '{') depth++;
                else if (raw[i] == '}') {
                    depth--;
                    if (depth == 0) {
                        i++;
                        break;
                    }
                }
                expr_str += raw[i++];
            }

            // Lex and parse inner expression
            Lexer sub_lexer(expr_str);
            auto tokens = sub_lexer.tokenize();
            Parser sub_parser(tokens);
            tmpl->expr_parts.push_back(sub_parser.parse_expression());
        } else {
            current_str += raw[i++];
        }
    }

    tmpl->string_parts.push_back(current_str);
    return tmpl;
}

} // namespace verb
