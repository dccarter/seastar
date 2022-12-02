/**
 * Copyright (c) 2022 suilteam, Carter
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 * @author Mpho Mbotho
 * @date 2022-11-26
 */

#include "compiler/codegen.hpp"

#include "compiler/ast.hpp"
#include "compiler/encoding.hpp"

namespace cstar {

Codegen::Codegen(std::ostream &os) : _os{os} {}

void Codegen::generate(Program &p)
{
    AppendNl("// Generated code");

    Nl();

    p.accept(*this);

    Nl();
}

void Codegen::visit(ContainerNode &node)
{
    for (auto &p : node.all()) {
        if (p != nullptr)
            p->accept(*this);
    }
}

void Codegen::visit(FunctionDecl &node)
{
    Tab();
    Append(node.returnType()->name(), " ", node.name);
    Append("()");
    Nl();
    node.body()->accept(*this);
    Nl();
}

void Codegen::visit(Block &node)
{
    Tab();
    Append('{');
    _level += 2;
    for (auto &stmt : node.all()) {
        Nl();
        stmt->accept(*this);
    }
    _level -= 2;
    Nl();
    Tab();
    Append('}');
}

void Codegen::visit(UnaryExpr &node)
{
    Append(Token::toString(node.op, true));
    node.operand()->accept(*this);
}

void Codegen::visit(BinaryExpr &node)
{
    node.left()->accept(*this);
    Append(' ', Token::toString(node.op, true), ' ');
    node.right()->accept(*this);
}

void Codegen::visit(GroupingExpr &node)
{
    Append('(');
    node.expr()->accept(*this);
    Append(')');
}

void Codegen::visit(VariableExpr &node) { Append(node.name); }

void Codegen::visit(BoolExpr &node) { Append(node.value ? '1' : '0'); }

void Codegen::visit(CharExpr &node) { writeUtf8(_os, node.value); }

void Codegen::visit(IntegerExpr &node) { Append(node.value); }

void Codegen::visit(FloatExpr &node) { Append(node.value); }

void Codegen::visit(StringExpr &node) { Append(node.value); }

void Codegen::visit(AssignmentExpr &node)
{
    node.assignee()->accept(*this);
    Append(" = ");
    node.value()->accept(*this);
}

void Codegen::visit(DeclarationStmt &node)
{
    Tab();

    if (node.isImmutable) {
        Append("const ");
    }

    Append(node.type()->name(), ' ');
    Append(node.name);
    if (auto value = node.value()) {
        Append(" = ");
        value->accept(*this);
    }
    Append(';');
}

void Codegen::visit(ExpressionStmt &node)
{
    Tab();
    node.expr()->accept(*this);
    Append(';');
}

void Codegen::visit(IfStmt &node)
{
    Tab();
    Append("if (");
    node.condition()->accept(*this);
    Append(")\n");
    if (auto stmt = std::dynamic_pointer_cast<ExpressionStmt>(node.then())) {
        _level += 2;
        stmt->accept(*this);
        _level -= 2;
    }
    else {
        node.then()->accept(*this);
    }
    if (auto stmt =
            std::dynamic_pointer_cast<ExpressionStmt>(node.otherwise())) {
        Append('\n');
        Tab();
        Append("else\n");
        _level += 2;
        stmt->accept(*this);
        _level -= 2;
    }
    else if (auto stmt = node.otherwise()) {
        Append('\n');
        Tab();
        Append("else\n");
        stmt->accept(*this);
    }
}

} // namespace cstar