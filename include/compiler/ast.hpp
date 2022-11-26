/**
 * Copyright (c) 2022 Suilteam, Carter Mbotho
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Carter
 * @date 2022-06-15
 */

#pragma once

#include <compiler/token.hpp>
#include <compiler/types.hpp>
#include <compiler/vistor.hpp>

namespace cstar {

    class Program final : public ContainerNode {
    public:
        using ContainerNode::ContainerNode;
        CYN_CONTAINER_NODE_VIEW(0, statements);
    };

    struct Expr : public ContainerNode {
    public:
        CSTAR_PTR(Expr);

        using ContainerNode::ContainerNode;
    };

    struct Stmt : public ContainerNode {
    public:
        CSTAR_PTR(Stmt);

    public:
        using ContainerNode::ContainerNode;
    };

    struct Block : public Stmt {
    public:
        CSTAR_PTR(Block);

    public:
        using Stmt::Stmt;

        VisitableNode();
    };

    struct FunctionDecl: public Stmt {
    public:
        CSTAR_PTR(FunctionDecl);

        CYN_CONTAINER_NODE_MEMBER(Type, 0, returnType);
        CYN_CONTAINER_NODE_MEMBER(Expr, 1, name);
        CYN_CONTAINER_NODE_MEMBER(Stmt, 2, body);

        explicit FunctionDecl(Expr::Ptr name, Range range = {});

        VisitableNode();
    };

    struct ExprList : public ContainerNode {
    public:
        CSTAR_PTR(ExprList);

    public:
        using ContainerNode::ContainerNode;

        CYN_CONTAINER_NODE_VIEW(0, expressions);
    };

    struct VariableExpr : public Expr {
    public:
        CSTAR_PTR(VariableExpr);

    public:
        std::string_view name;
        CYN_CONTAINER_NODE_MEMBER(Type, 0, type);

    public:
        explicit VariableExpr(std::string_view name, Range range = {})
            : Expr(std::move(range)), name{name} {}

        VisitableNode()
    };

    struct AssignmentExpr : public Expr {
    public:
        CSTAR_PTR(AssignmentExpr);

    public:
        CYN_CONTAINER_NODE_MEMBER(ExprList, 0, assignee);
        CYN_CONTAINER_NODE_MEMBER(ExprList, 1, value);

    public:
        using Expr::Expr;

        AssignmentExpr(ExprList::Ptr assignee, ExprList::Ptr value, Range range = {});

    public:
        VisitableNode()
    };

    struct BinaryExpr : public Expr {
    public:
        CSTAR_PTR(BinaryExpr);

    public:
        Token::Kind op{};

        CYN_CONTAINER_NODE_MEMBER(Expr, 0, left);
        CYN_CONTAINER_NODE_MEMBER(Expr, 1, right);

    public:
        using Expr::Expr;

        BinaryExpr(Expr::Ptr left, Token::Kind op, Expr::Ptr right, Range range = {});

    public:
        VisitableNode()
    };

    struct UnaryExpr : public Expr {
    public:
        CSTAR_PTR(UnaryExpr);

    public:
        Token::Kind op{};

        CYN_CONTAINER_NODE_MEMBER(Expr, 0, operand);

    public:
        using Expr::Expr;

        UnaryExpr(Token::Kind op, Expr::Ptr operand, Range range = {});

    public:
        VisitableNode()
    };

    struct LiteralExpr : public Expr {
    public:
        CSTAR_PTR(LiteralExpr);

    public:
        using Expr::Expr;

        CYN_CONTAINER_NODE_MEMBER(Type, 0, type);

    public:
        VisitableNode()
    };

    struct BoolExpr : public LiteralExpr {
    public:
        CSTAR_PTR(BoolExpr);

    public:
        bool value{};

    public:
        using LiteralExpr::LiteralExpr;

        explicit BoolExpr(bool value, Range range = {});

    public:
        VisitableNode()
    };

    struct CharExpr : public LiteralExpr {
    public:
        CSTAR_PTR(CharExpr);

    public:
        uint32_t value{};

    public:
        using LiteralExpr::LiteralExpr;

        explicit CharExpr(uint32_t value, Range range = {});

    public:
        VisitableNode()
    };

    struct IntegerExpr : public LiteralExpr {
    public:
        CSTAR_PTR(IntegerExpr);

    public:
        int64_t value{};

    public:
        using LiteralExpr::LiteralExpr;

        explicit IntegerExpr(int64_t value, Range range = {});

    public:
        VisitableNode()
    };

    struct FloatExpr : public LiteralExpr {
    public:
        CSTAR_PTR(FloatExpr);

    public:
        double value{};

    public:
        using LiteralExpr::LiteralExpr;

        explicit FloatExpr(double value, Range range = {});

    public:
        VisitableNode()
    };

    struct StringExpr : public LiteralExpr {
    public:
        CSTAR_PTR(StringExpr);

    public:
        std::string_view value{};

    public:
        using LiteralExpr::LiteralExpr;

        StringExpr(std::string_view value, Range range = {});

    public:
        VisitableNode()
    };

    struct GroupingExpr : public Expr {
    public:
        CSTAR_PTR(StringExpr);

    public:
        std::string_view value{};

    public:
        using Expr::Expr;
        GroupingExpr(Expr::Ptr expr, Range range = {});

        CYN_CONTAINER_NODE_MEMBER(Expr, 0, expr);

    public:
        VisitableNode()
    };
}