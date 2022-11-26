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
#include "compiler/types.hpp"
#include "compiler/log.hpp"

#include <iostream>

namespace  {
    inline void puttab(std::uint32_t level) {
        std::printf("%*c- ", level, ' ');
    }
}

namespace cstar {

    void AstDump::dump(Program &program)
    {
        program.accept(*this);
    }

    void AstDump::visit(Node &node) {}

    void AstDump::visit(ContainerNode &node)
    {
        for (auto& e: node.all()) {
            e->accept(*this);
        }
    }

    void AstDump::visit(Block &node)
    {
        std::printf("%*c- Block\n", level, ' ');
        level += 2;

        for (auto& child: node.all()) {
            puttab(level);
            child->accept(*this);
            std::putchar('\n');
        }

        level -= 2;
    }

    void AstDump::visit(FunctionDecl &node)
    {
        csAssert(node.returnType());
        csAssert(node.name());
        csAssert(node.body());

        std::printf("%*c- FunctionDecl:\n", level, ' ');
        level += 2;

        std::printf("%*c- returns: ", level, ' ');
        node.returnType()->accept(*this);

        std::printf("\n%*c- name: ", level, ' ');
        node.name()->accept(*this);

        std::printf("\n%*c- body: \n", level, ' ');
        level += 2;
        node.body()->accept(*this);
        level -= 4;
    }

    void AstDump::visit(BoolType &node)
    {
        std::fputs("bool", stdout);
    }

    void AstDump::visit(CharType &node)
    {
        std::fputs("char", stdout);
    }

    void AstDump::visit(VoidType &node)
    {
        std::fputs("void", stdout);
    }

    void AstDump::visit(IntegerType &node)
    {
        std::printf("%*s", int(node.name().size()), node.name().data());
    }

    void AstDump::visit(FloatType &node)
    {
        std::printf("%*s", int(node.name().size()), node.name().data());
    }

    void AstDump::visit(StringType &node)
    {
        std::fputs("string", stdout);
    }

    void AstDump::visit(BoolExpr &node)
    {
        std::printf("%s", node.value? "true": "false");
    }

    void AstDump::visit(CharExpr &node)
    {
        std::putchar('\'');
        writeUtf8(std::cout, node.value);
        std::putchar('\'');
    }

    void AstDump::visit(IntegerExpr &node)
    {
        std::printf("%lld", node.value);
    }

    void AstDump::visit(FloatExpr &node)
    {
        std::printf("%g", node.value);
    }

    void AstDump::visit(StringExpr &node)
    {
        std::printf("\"%.*s\"",
                    int(node.value.size()),
                    node.value.data());
    }

    void AstDump::visit(VariableExpr &node)
    {
        std::printf("\"%.*s\"",
                    int(node.name.size()),
                    node.name.data());
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
}