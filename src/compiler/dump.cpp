/**
 * Copyright (c) 2022 suilteam, Carter
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 * @author Mpho Mbotho
 * @date 2022-11-25
 */

#include "compiler/dump.hpp"

#include "compiler/ast.hpp"
#include "compiler/encoding.hpp"
#include "compiler/log.hpp"
#include "compiler/types.hpp"

#include <iostream>

namespace {
inline void puttab(std::uint32_t level) { std::printf("%*c- ", level, ' '); }
} // namespace

namespace cstar {

void AstDump::dump(Program &program)
{
    program.accept(*this);
    std::putchar('\n');
}

void AstDump::visit(Node &node) {}

void AstDump::visit(ContainerNode &node)
{
    for (auto &e : node.all()) {
        if (e != nullptr)
            e->accept(*this);
    }
}

void AstDump::visit(Block &node)
{
    std::printf("%*c- Block", level, ' ');
    level += 2;

    for (auto &child : node.all()) {
        std::putchar('\n');
        child->accept(*this);
    }

    level -= 2;
}

void AstDump::visit(FunctionDecl &node)
{
    csAssert(node.returnType());
    csAssert(node.body());

    std::printf("%*c- FunctionDecl:\n", level, ' ');
    level += 2;

    std::printf("%*c- returns: ", level, ' ');
    node.returnType()->accept(*this);

    std::printf("\n%*c- name: %.*s",
                level,
                ' ',
                int(node.name.size()),
                node.name.data());

    std::printf("\n%*c- body: \n", level, ' ');
    level += 2;
    node.body()->accept(*this);
    level -= 4;
}

void AstDump::visit(BoolType &node) { std::fputs("bool", stdout); }

void AstDump::visit(CharType &node) { std::fputs("char", stdout); }

void AstDump::visit(VoidType &node) { std::fputs("void", stdout); }

void AstDump::visit(IntegerType &node)
{
    std::printf("%.*s", int(node.name().size()), node.name().data());
}

void AstDump::visit(BuiltinType &node)
{
    std::printf("%.*s", int(node.name().size()), node.name().data());
}

void AstDump::visit(FloatType &node)
{
    std::printf("%.*s", int(node.name().size()), node.name().data());
}

void AstDump::visit(StringType &node) { std::fputs("string", stdout); }

void AstDump::visit(BoolExpr &node)
{
    std::printf("%s", node.value ? "true" : "false");
}

void AstDump::visit(CharExpr &node)
{
    std::putchar('\'');
    writeUtf8(std::cout, node.value);
    std::putchar('\'');
}

void AstDump::visit(IntegerExpr &node) { std::printf("%lld", node.value); }

void AstDump::visit(FloatExpr &node) { std::printf("%g", node.value); }

void AstDump::visit(StringExpr &node)
{
    std::printf("\"%.*s\"", int(node.value.size()), node.value.data());
}

void AstDump::visit(VariableExpr &node)
{
    std::printf("\"%.*s\"", int(node.name.size()), node.name.data());
}

void AstDump::visit(GroupingExpr &node)
{
    std::putchar('(');
    node.expr()->accept(*this);
    std::putchar(')');
}

void AstDump::visit(UnaryExpr &node)
{
    std::putchar('(');
    auto op = Token::toString(node.op, true);
    std::printf("%.*s", int(op.size()), op.data());
    node.operand()->accept(*this);
    std::putchar(')');
}

void AstDump::visit(BinaryExpr &node)
{
    std::putchar('(');
    node.left()->accept(*this);
    std::putchar(' ');

    auto op = Token::toString(node.op, true);
    std::printf("%.*s", int(op.size()), op.data());

    std::putchar(' ');
    node.right()->accept(*this);
    std::putchar(')');
}

void AstDump::visit(AssignmentExpr &node)
{
    std::fputs("AssignmentExpr:\n", stdout);
    level += 2;
    std::printf("%*c- lhs: ", level, ' ');
    node.assignee()->accept(*this);
    std::printf("\n%*c- rhs: ", level, ' ');
    node.value()->accept(*this);
    level -= 2;
}

void AstDump::visit(cstar::ExpressionStmt &node)
{
    puttab(level);
    node.expr()->accept(*this);
}

void AstDump::visit(cstar::DeclarationStmt &node)
{
    std::printf("%*c- DeclarationStmt:", level, ' ');
    level += 2;
    if (auto tp = node.type()) {
        std::printf("\n%*c- type: ", level, ' ');
        tp->accept(*this);
    }

    std::printf("\n%*c- type: %.*s",
                level,
                ' ',
                int(node.name.size()),
                node.name.data());

    if (auto val = node.value()) {
        std::printf("\n%*c- type: ", level, ' ');
        val->accept(*this);
    }
    level -= 2;
}

void AstDump::visit(IfStmt &node)
{
    puttab(level);
    std::fputs("IfStmt\n", stdout);
    level += 2;
    std::printf("%*c- cond: ", level, ' ');
    node.condition()->accept(*this);
    std::printf("\n%*c- then: \n", level, ' ');
    level += 2;
    node.then()->accept(*this);
    level -= 2;
    if (auto otherwise = node.otherwise()) {
        std::printf("\n%*c- else: \n", level, ' ');
        level += 2;
        otherwise->accept(*this);
        level -= 2;
    }
    level -= 2;
}

void AstDump::visit(WhileStmt &node)
{
    std::printf("%*c- WhileStmt:\n", level, ' ');
    level += 2;
    std::printf("*%c- cond: ", level, ' ');
}

} // namespace cstar