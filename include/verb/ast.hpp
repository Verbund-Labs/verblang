#pragma once

#include <string>
#include <vector>
#include <memory>
#include "verb/token.hpp"

namespace verb {

class ASTVisitor;

struct ASTNode {
    SourceLocation loc;
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
};

// Expressions
struct Expr : public ASTNode {};

struct IntLiteralExpr : public Expr {
    int64_t value = 0;
    explicit IntLiteralExpr(int64_t val) : value(val) {}
    void accept(ASTVisitor* visitor) override;
};

struct FloatLiteralExpr : public Expr {
    double value = 0.0;
    explicit FloatLiteralExpr(double val) : value(val) {}
    void accept(ASTVisitor* visitor) override;
};

struct StringLiteralExpr : public Expr {
    std::string value;
    explicit StringLiteralExpr(std::string val) : value(std::move(val)) {}
    void accept(ASTVisitor* visitor) override;
};

struct BoolLiteralExpr : public Expr {
    bool value = false;
    explicit BoolLiteralExpr(bool val) : value(val) {}
    void accept(ASTVisitor* visitor) override;
};

struct NilLiteralExpr : public Expr {
    void accept(ASTVisitor* visitor) override;
};

struct IdentifierExpr : public Expr {
    std::string name;
    explicit IdentifierExpr(std::string n) : name(std::move(n)) {}
    void accept(ASTVisitor* visitor) override;
};

struct TemplateLiteralExpr : public Expr {
    std::vector<std::string> string_parts;
    std::vector<std::shared_ptr<Expr>> expr_parts;
    void accept(ASTVisitor* visitor) override;
};

struct BinaryExpr : public Expr {
    std::string op;
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    BinaryExpr(std::string o, std::shared_ptr<Expr> l, std::shared_ptr<Expr> r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
    void accept(ASTVisitor* visitor) override;
};

struct UnaryExpr : public Expr {
    std::string op;
    std::shared_ptr<Expr> operand;
    UnaryExpr(std::string o, std::shared_ptr<Expr> opnd)
        : op(std::move(o)), operand(std::move(opnd)) {}
    void accept(ASTVisitor* visitor) override;
};

struct CallExpr : public Expr {
    std::shared_ptr<Expr> callee;
    std::vector<std::shared_ptr<Expr>> args;
    CallExpr(std::shared_ptr<Expr> c, std::vector<std::shared_ptr<Expr>> a)
        : callee(std::move(c)), args(std::move(a)) {}
    void accept(ASTVisitor* visitor) override;
};

struct IndexExpr : public Expr {
    std::shared_ptr<Expr> collection;
    std::shared_ptr<Expr> index;
    IndexExpr(std::shared_ptr<Expr> c, std::shared_ptr<Expr> i)
        : collection(std::move(c)), index(std::move(i)) {}
    void accept(ASTVisitor* visitor) override;
};

struct SliceExpr : public Expr {
    std::shared_ptr<Expr> collection;
    std::shared_ptr<Expr> start;
    std::shared_ptr<Expr> end;
    std::shared_ptr<Expr> step;
    SliceExpr(std::shared_ptr<Expr> c, std::shared_ptr<Expr> s, std::shared_ptr<Expr> e, std::shared_ptr<Expr> st = nullptr)
        : collection(std::move(c)), start(std::move(s)), end(std::move(e)), step(std::move(st)) {}
    void accept(ASTVisitor* visitor) override;
};

struct ListLiteralExpr : public Expr {
    std::vector<std::shared_ptr<Expr>> elements;
    void accept(ASTVisitor* visitor) override;
};

struct DictLiteralExpr : public Expr {
    std::vector<std::pair<std::string, std::shared_ptr<Expr>>> entries;
    void accept(ASTVisitor* visitor) override;
};

struct ListCompExpr : public Expr {
    std::shared_ptr<Expr> item_expr;
    std::string var_name;
    std::shared_ptr<Expr> iterable_expr;
    std::shared_ptr<Expr> condition; // optional filter
    void accept(ASTVisitor* visitor) override;
};

struct ArrowFnExpr : public Expr {
    std::vector<std::string> params;
    std::shared_ptr<ASTNode> body; // either an Expr or BlockStmt
    void accept(ASTVisitor* visitor) override;
};

struct HtmlAttribute {
    std::string name;
    std::shared_ptr<Expr> value;
};

struct HtmlElementExpr : public Expr {
    std::string tag;
    std::vector<HtmlAttribute> attributes;
    std::vector<std::shared_ptr<Expr>> children;
    void accept(ASTVisitor* visitor) override;
};

struct AwaitExpr : public Expr {
    std::shared_ptr<Expr> expr;
    explicit AwaitExpr(std::shared_ptr<Expr> e) : expr(std::move(e)) {}
    void accept(ASTVisitor* visitor) override;
};

// Statements
struct Stmt : public ASTNode {};

struct ExprStmt : public Stmt {
    std::shared_ptr<Expr> expr;
    explicit ExprStmt(std::shared_ptr<Expr> e) : expr(std::move(e)) {}
    void accept(ASTVisitor* visitor) override;
};

struct BlockStmt : public Stmt {
    std::vector<std::shared_ptr<Stmt>> statements;
    void accept(ASTVisitor* visitor) override;
};

struct VarDeclStmt : public Stmt {
    bool is_const = false;
    std::string name;
    std::string type_annotation;
    std::shared_ptr<Expr> initializer;
    void accept(ASTVisitor* visitor) override;
};

struct AssignStmt : public Stmt {
    std::shared_ptr<Expr> target; // IdentifierExpr or IndexExpr
    std::string op;               // "=", "+=", "-="
    std::shared_ptr<Expr> value;
    void accept(ASTVisitor* visitor) override;
};

struct FunctionDeclStmt : public Stmt {
    std::string name;
    std::vector<std::string> params;
    std::shared_ptr<BlockStmt> body;
    bool is_async = false;
    void accept(ASTVisitor* visitor) override;
};

struct ReturnStmt : public Stmt {
    std::shared_ptr<Expr> value;
    explicit ReturnStmt(std::shared_ptr<Expr> v = nullptr) : value(std::move(v)) {}
    void accept(ASTVisitor* visitor) override;
};

struct IfStmt : public Stmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<BlockStmt> then_branch;
    std::vector<std::pair<std::shared_ptr<Expr>, std::shared_ptr<BlockStmt>>> elif_branches;
    std::shared_ptr<BlockStmt> else_branch;
    void accept(ASTVisitor* visitor) override;
};

struct WhileStmt : public Stmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<BlockStmt> body;
    void accept(ASTVisitor* visitor) override;
};

struct ForInStmt : public Stmt {
    std::string var_name;
    std::shared_ptr<Expr> iterable;
    std::shared_ptr<BlockStmt> body;
    void accept(ASTVisitor* visitor) override;
};

struct DeferStmt : public Stmt {
    std::shared_ptr<BlockStmt> body;
    void accept(ASTVisitor* visitor) override;
};

struct NativeCxxStmt : public Stmt {
    std::string code;
    explicit NativeCxxStmt(std::string c) : code(std::move(c)) {}
    void accept(ASTVisitor* visitor) override;
};

struct StyleDeclStmt : public Stmt {
    std::string selector;
    std::vector<std::pair<std::string, std::shared_ptr<Expr>>> properties;
    void accept(ASTVisitor* visitor) override;
};

struct Program : public ASTNode {
    std::vector<std::shared_ptr<Stmt>> statements;
    void accept(ASTVisitor* visitor) override;
};

// Visitor Interface
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(IntLiteralExpr* node) = 0;
    virtual void visit(FloatLiteralExpr* node) = 0;
    virtual void visit(StringLiteralExpr* node) = 0;
    virtual void visit(BoolLiteralExpr* node) = 0;
    virtual void visit(NilLiteralExpr* node) = 0;
    virtual void visit(IdentifierExpr* node) = 0;
    virtual void visit(TemplateLiteralExpr* node) = 0;
    virtual void visit(BinaryExpr* node) = 0;
    virtual void visit(UnaryExpr* node) = 0;
    virtual void visit(CallExpr* node) = 0;
    virtual void visit(IndexExpr* node) = 0;
    virtual void visit(SliceExpr* node) = 0;
    virtual void visit(ListLiteralExpr* node) = 0;
    virtual void visit(DictLiteralExpr* node) = 0;
    virtual void visit(ListCompExpr* node) = 0;
    virtual void visit(ArrowFnExpr* node) = 0;
    virtual void visit(HtmlElementExpr* node) = 0;
    virtual void visit(AwaitExpr* node) = 0;

    virtual void visit(ExprStmt* node) = 0;
    virtual void visit(BlockStmt* node) = 0;
    virtual void visit(VarDeclStmt* node) = 0;
    virtual void visit(AssignStmt* node) = 0;
    virtual void visit(FunctionDeclStmt* node) = 0;
    virtual void visit(ReturnStmt* node) = 0;
    virtual void visit(IfStmt* node) = 0;
    virtual void visit(WhileStmt* node) = 0;
    virtual void visit(ForInStmt* node) = 0;
    virtual void visit(DeferStmt* node) = 0;
    virtual void visit(NativeCxxStmt* node) = 0;
    virtual void visit(StyleDeclStmt* node) = 0;
    virtual void visit(Program* node) = 0;
};

} // namespace verb
