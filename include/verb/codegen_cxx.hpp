#pragma once

#include "verb/ast.hpp"
#include <string>
#include <sstream>

namespace verb {

class CxxCodeGen : public ASTVisitor {
public:
    CxxCodeGen();

    std::string generate(Program* program);

    void visit(IntLiteralExpr* node) override;
    void visit(FloatLiteralExpr* node) override;
    void visit(StringLiteralExpr* node) override;
    void visit(BoolLiteralExpr* node) override;
    void visit(NilLiteralExpr* node) override;
    void visit(IdentifierExpr* node) override;
    void visit(TemplateLiteralExpr* node) override;
    void visit(BinaryExpr* node) override;
    void visit(UnaryExpr* node) override;
    void visit(CallExpr* node) override;
    void visit(IndexExpr* node) override;
    void visit(SliceExpr* node) override;
    void visit(ListLiteralExpr* node) override;
    void visit(DictLiteralExpr* node) override;
    void visit(ListCompExpr* node) override;
    void visit(ArrowFnExpr* node) override;
    void visit(HtmlElementExpr* node) override;
    void visit(AwaitExpr* node) override;

    void visit(ExprStmt* node) override;
    void visit(BlockStmt* node) override;
    void visit(VarDeclStmt* node) override;
    void visit(AssignStmt* node) override;
    void visit(FunctionDeclStmt* node) override;
    void visit(ReturnStmt* node) override;
    void visit(IfStmt* node) override;
    void visit(WhileStmt* node) override;
    void visit(ForInStmt* node) override;
    void visit(DeferStmt* node) override;
    void visit(MatchStmt* node) override;
    void visit(NativeCxxStmt* node) override;
    void visit(StyleDeclStmt* node) override;
    void visit(Program* node) override;

private:
    std::string indent() const;
    void emit_indent();

    std::stringstream out_;
    std::stringstream top_level_cxx_;
    int indent_level_ = 0;
    bool in_expression_ = false;
};

} // namespace verb
