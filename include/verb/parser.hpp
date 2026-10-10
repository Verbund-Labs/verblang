#pragma once

#include "verb/token.hpp"
#include "verb/ast.hpp"
#include <vector>
#include <memory>
#include <stdexcept>

namespace verb {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::shared_ptr<Program> parse_program();

private:
    const Token& peek(size_t offset = 0) const;
    const Token& previous() const;
    bool is_at_end() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& err_msg);
    void skip_semicolons();

    // Statements
    std::shared_ptr<Stmt> parse_statement();
    std::shared_ptr<VarDeclStmt> parse_var_decl(bool is_const);
    std::shared_ptr<FunctionDeclStmt> parse_function_decl(bool is_async);
    std::shared_ptr<ReturnStmt> parse_return();
    std::shared_ptr<IfStmt> parse_if();
    std::shared_ptr<WhileStmt> parse_while();
    std::shared_ptr<ForInStmt> parse_for_in();
    std::shared_ptr<DeferStmt> parse_defer();
    std::shared_ptr<MatchStmt> parse_match();
    std::shared_ptr<NativeCxxStmt> parse_native_cxx();
    std::shared_ptr<StyleDeclStmt> parse_style_decl();
    std::shared_ptr<BlockStmt> parse_block();

    // Expressions
    std::shared_ptr<Expr> parse_expression();
    std::shared_ptr<Expr> parse_arrow_or_assignment();
    std::shared_ptr<Expr> parse_logical_or();
    std::shared_ptr<Expr> parse_logical_and();
    std::shared_ptr<Expr> parse_equality();
    std::shared_ptr<Expr> parse_comparison();
    std::shared_ptr<Expr> parse_additive();
    std::shared_ptr<Expr> parse_multiplicative();
    std::shared_ptr<Expr> parse_unary();
    std::shared_ptr<Expr> parse_postfix();
    std::shared_ptr<Expr> parse_primary();

    // Specific expressions
    std::shared_ptr<HtmlElementExpr> parse_html_element();
    std::shared_ptr<Expr> parse_bracket_expression(); // list or list comprehension
    std::shared_ptr<DictLiteralExpr> parse_dict_literal();
    std::shared_ptr<TemplateLiteralExpr> parse_template_literal(const std::string& raw);

    std::vector<Token> tokens_;
    size_t cursor_ = 0;
};

} // namespace verb
