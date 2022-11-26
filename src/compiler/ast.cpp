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

    FunctionDecl::FunctionDecl(Expr::Ptr var, Range range)
        : Stmt(std::move(range))
    {
        returnType(builtin::voidType());
        name(std::move(var));
        body(std::make_shared<Block>(this->range()));
    }

    AssignmentExpr::AssignmentExpr(ExprList::Ptr lhs, ExprList::Ptr rhs, Range range)
        : Expr(std::move(range))
    {
        assignee(std::move(lhs));
        value(std::move(rhs));
    }

    BinaryExpr::BinaryExpr(Expr::Ptr lhs, Token::Kind op, Expr::Ptr rhs, Range range)
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
    { type(builtin::charType()); }

    IntegerExpr::IntegerExpr(int64_t value, Range range)
        : LiteralExpr(std::move(range)), value{value}
    {
        type(builtin::i64Type());
    }

    FloatExpr::FloatExpr(double value, Range range)
        : LiteralExpr(std::move(range)), value{value}
    { type(builtin::f64Type()); }

    StringExpr::StringExpr(std::string_view value, Range range)
            : LiteralExpr(std::move(range)), value{value}
    { type(builtin::stringType()); }

    GroupingExpr::GroupingExpr(Expr::Ptr e, Range range)
        : Expr(std::move(range))
    { expr(std::move(e)); }
}