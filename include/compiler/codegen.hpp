/**
 * Copyright (c) 2022 suilteam, Carter
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 * @author Mpho Mbotho
 * @date 2022-11-26
 */

#pragma once

#include "compiler/vistor.hpp"

#include <sstream>

namespace cstar {

class Program;

class Codegen : public Visitor {
public:
    Codegen(std::ostream &os);
    void generate(Program &p);

    void visit(ContainerNode &node) override;
    void visit(FunctionDecl &node) override;
    void visit(Block &node) override;
    void visit(UnaryExpr &node) override;
    void visit(BinaryExpr &node) override;
    void visit(GroupingExpr &node) override;
    void visit(BoolExpr &node) override;
    void visit(CharExpr &node) override;
    void visit(IntegerExpr &node) override;
    void visit(FloatExpr &node) override;
    void visit(StringExpr &node) override;
    void visit(VariableExpr &node) override;
    void visit(AssignmentExpr &node) override;

    void visit(DeclarationStmt &node) override;
    void visit(ExpressionStmt &node) override;
    void visit(IfStmt &node) override;

private:
    template <typename... Args>
    void AppendNl(Args &&...args)
    {
        (_os << ... << args) << std::endl;
    }

    template <typename... Args>
    void Append(Args &&...args)
    {
        (_os << ... << args);
    }

    void Tab() { _os << std::string(std::size_t(_level), ' '); }
    void Nl() { _os << std::endl; }

    int _level{0};
    std::ostream &_os;
};
} // namespace cstar