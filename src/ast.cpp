#include "verb/ast.hpp"

namespace verb {

void IntLiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void FloatLiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void StringLiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void BoolLiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void NilLiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void IdentifierExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void TemplateLiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void BinaryExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void UnaryExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void CallExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void IndexExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void SliceExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ListLiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void DictLiteralExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ListCompExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ArrowFnExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void HtmlElementExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }
void AwaitExpr::accept(ASTVisitor* visitor) { visitor->visit(this); }

void ExprStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void BlockStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void VarDeclStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void AssignStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void FunctionDeclStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ReturnStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void IfStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void WhileStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void ForInStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void DeferStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void NativeCxxStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void StyleDeclStmt::accept(ASTVisitor* visitor) { visitor->visit(this); }
void Program::accept(ASTVisitor* visitor) { visitor->visit(this); }

} // namespace verb
