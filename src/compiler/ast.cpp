/**
 * Copyright (c) 2022 Suilteam, Carter Mbotho
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 * @author Carter
 * @date 2022-06-15
 */

#include "compiler/ast.hpp"
#include "compiler/builtin.hpp"

#include <utility>

namespace cstar {

FunctionDecl::FunctionDecl(std::string_view funcName, Range range)
    : Stmt(std::move(range)), name{funcName}
{
    returnType(builtin::voidType());
    params(nullptr);
    body(nullptr);
}

Expr::Expr(Type::Ptr tp, Range range) : ContainerNode(std::move(range))
{
    type(std::move(tp));
}

Expr::Expr(Range range) : Expr(builtin::autoType(), std::move(range)) {}

AssignmentExpr::AssignmentExpr(Expr::Ptr lhs, Expr::Ptr rhs, Range range)
    : Expr(std::move(range))
{
    assignee(std::move(lhs));
    value(std::move(rhs));
}

BinaryExpr::BinaryExpr(Expr::Ptr lhs,
                       Token::Kind op,
                       Expr::Ptr rhs,
                       Range range)
    : Expr(std::move(range)), op{op}
{
    left(std::move(lhs));
    right(std::move(rhs));
}

UnaryExpr::UnaryExpr(Token::Kind op, Expr::Ptr rhs, Range range)
    : Expr(std::move(range)), op{op}
{
    operand(std::move(rhs));
}

BoolExpr::BoolExpr(bool value, Range range)
    : LiteralExpr(std::move(range)), value{value}
{
    type(builtin::booleanType());
}

CharExpr::CharExpr(uint32_t value, Range range)
    : LiteralExpr(std::move(range)), value{value}
{
    type(builtin::charType());
}

IntegerExpr::IntegerExpr(int64_t value, Range range)
    : LiteralExpr(std::move(range)), value{value}
{
    type(builtin::i64Type());
}

FloatExpr::FloatExpr(double value, Range range)
    : LiteralExpr(std::move(range)), value{value}
{
    type(builtin::f64Type());
}

StringExpr::StringExpr(std::string_view value, Range range)
    : LiteralExpr(std::move(range)), value{value}
{
    type(builtin::stringType());
}

GroupingExpr::GroupingExpr(Expr::Ptr e, Range range) : Expr(std::move(range))
{
    expr(std::move(e));
}

CallExpr::CallExpr(Expr::Ptr func, Range range) : Expr(std::move(range))
{
    callee(std::move(func));
    arguments(nullptr);
}

DeclarationStmt::DeclarationStmt(std::string_view var, bool imm, Range range)
    : Stmt(std::move(range)), isImmutable{imm}, name{var}
{
    type(builtin::autoType());
    value(nullptr);
}

ParameterStmt::ParameterStmt(std::string_view var, Range range)
    : DeclarationStmt(var, true, std::move(range))
{
    def(nullptr);
}

ExpressionStmt::ExpressionStmt(Expr::Ptr exp, Range range)
    : Stmt(std::move(range))
{
    expr(std::move(exp));
}

IfStmt::IfStmt(Expr::Ptr cond, Range range) : Stmt(std::move(range))
{
    condition(std::move(cond));
    then(nullptr);
    otherwise(nullptr);
}

WhileStmt::WhileStmt(Expr::Ptr cond, Range range) : Stmt(std::move(range))
{
    condition(std::move(cond));
    body(nullptr);
}

ForStmt::ForStmt(Range range) : Stmt(std::move(range))
{
    init(nullptr);
    condition(nullptr);
    update(nullptr);
    body(nullptr);
}

} // namespace cstar