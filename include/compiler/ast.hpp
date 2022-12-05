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

    Expr(Range range = {});
    Expr(Type::Ptr type, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Type, 0, type);
};

struct Stmt : public ContainerNode {
public:
    CSTAR_PTR(Stmt);

    using ContainerNode::ContainerNode;
};

struct Block : public Stmt {
public:
    CSTAR_PTR(Block);

    using Stmt::Stmt;

    VisitableNode();
};

struct StatementList : public ContainerNode {
public:
    CSTAR_PTR(StatementList);

public:
    using ContainerNode::ContainerNode;

    CYN_CONTAINER_NODE_VIEW(0, stmts);

    void add(Stmt::Ptr stmts) { push(std::move(stmts)); }

    VisitableNode()
};

struct FunctionDecl : public Stmt {
public:
    CSTAR_PTR(FunctionDecl);

    CYN_CONTAINER_NODE_MEMBER(Type, 0, returnType);
    CYN_CONTAINER_NODE_MEMBER(StatementList, 1, params);
    CYN_CONTAINER_NODE_MEMBER(Stmt, 2, body);

    explicit FunctionDecl(std::string_view name, Range range = {});

    VisitableNode();

    std::string_view name{};
};

struct VariableExpr : public Expr {
public:
    CSTAR_PTR(VariableExpr);

public:
    explicit VariableExpr(std::string_view name, Range range = {})
        : Expr(std::move(range)), name{name}
    {
    }

    VisitableNode();

    std::string_view name{};
};

struct AssignmentExpr : public Expr {
public:
    CSTAR_PTR(AssignmentExpr);

    using Expr::Expr;

    AssignmentExpr(Expr::Ptr assignee, Expr::Ptr value, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 0, assignee);
    CYN_CONTAINER_NODE_MEMBER(Expr, 1, value);

    VisitableNode()
};

struct BinaryExpr : public Expr {
public:
    CSTAR_PTR(BinaryExpr);

    using Expr::Expr;

    BinaryExpr(Expr::Ptr left,
               Token::Kind op,
               Expr::Ptr right,
               Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 0, left);
    CYN_CONTAINER_NODE_MEMBER(Expr, 1, right);

    VisitableNode();

    Token::Kind op{};
};

struct UnaryExpr : public Expr {
public:
    CSTAR_PTR(UnaryExpr);

    using Expr::Expr;

    UnaryExpr(Token::Kind op, Expr::Ptr operand, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 0, operand);

    VisitableNode();

    Token::Kind op{};
};

struct LiteralExpr : public Expr {
public:
    CSTAR_PTR(LiteralExpr);

    using Expr::Expr;

    VisitableNode()
};

struct BoolExpr : public LiteralExpr {
public:
    CSTAR_PTR(BoolExpr);

    using LiteralExpr::LiteralExpr;

    explicit BoolExpr(bool value, Range range = {});

    VisitableNode();

    bool value{};
};

struct CharExpr : public LiteralExpr {
public:
    CSTAR_PTR(CharExpr);

    using LiteralExpr::LiteralExpr;

    explicit CharExpr(uint32_t value, Range range = {});

    VisitableNode();

    uint32_t value{};
};

struct IntegerExpr : public LiteralExpr {
public:
    CSTAR_PTR(IntegerExpr);

public:
    using LiteralExpr::LiteralExpr;

    explicit IntegerExpr(int64_t value, Range range = {});

    VisitableNode();

    int64_t value{};
};

struct FloatExpr : public LiteralExpr {
public:
    CSTAR_PTR(FloatExpr);

    using LiteralExpr::LiteralExpr;

    explicit FloatExpr(double value, Range range = {});

    VisitableNode();

    double value{};
};

struct StringExpr : public LiteralExpr {
public:
    CSTAR_PTR(StringExpr);

    using LiteralExpr::LiteralExpr;

    StringExpr(std::string_view value, Range range = {});

    VisitableNode();

    std::string_view value{};
};

struct GroupingExpr : public Expr {
public:
    CSTAR_PTR(GroupingExpr);

public:
    using Expr::Expr;
    GroupingExpr(Expr::Ptr expr, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 0, expr);

    VisitableNode()
};

struct ExpressionList : public ContainerNode {
public:
    CSTAR_PTR(ExpressionList);

public:
    using ContainerNode::ContainerNode;

    CYN_CONTAINER_NODE_VIEW(0, exprs);

    void add(Expr::Ptr expr) { push(std::move(expr)); }

    VisitableNode()
};

struct CallExpr : public Expr {
public:
    CSTAR_PTR(CallExpr);

public:
    using Expr::Expr;
    CallExpr(Expr::Ptr callee, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 0, callee);
    CYN_CONTAINER_NODE_MEMBER(ExpressionList, 1, arguments);

    VisitableNode()
};

class ExpressionStmt : public Stmt {
public:
    CSTAR_PTR(ExpressionStmt);
    using Stmt::Stmt;

    ExpressionStmt(Expr::Ptr expr, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 0, expr);

    VisitableNode();
};

class DeclarationStmt : public Stmt {
public:
    CSTAR_PTR(DeclarationStmt);
    using Stmt::Stmt;
    DeclarationStmt(std::string_view name, bool imm, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Type, 0, type);
    CYN_CONTAINER_NODE_MEMBER(Expr, 1, value);

    VisitableNode();

    bool isImmutable{false};
    std::string_view name{};
};

class ParameterStmt : public DeclarationStmt {
public:
    CSTAR_PTR(ParameterStmt);
    using DeclarationStmt::DeclarationStmt;
    ParameterStmt(std::string_view name, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 2, def);

    VisitableNode();
};

class IfStmt : public Stmt {
public:
    CSTAR_PTR(IfStmt);
    using Stmt::Stmt;
    IfStmt(Expr::Ptr cond, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 0, condition);
    CYN_CONTAINER_NODE_MEMBER(Stmt, 1, then);
    CYN_CONTAINER_NODE_MEMBER(Stmt, 2, otherwise);

    VisitableNode();
};

class WhileStmt : public Stmt {
public:
    CSTAR_PTR(WhileStmt);
    using Stmt::Stmt;
    WhileStmt(Expr::Ptr cond, Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Expr, 0, condition);
    CYN_CONTAINER_NODE_MEMBER(Stmt, 1, body);

    VisitableNode();
};

class ForStmt : public Stmt {
public:
    CSTAR_PTR(ForStmt);
    ForStmt(Range range = {});

    CYN_CONTAINER_NODE_MEMBER(Stmt, 0, init);
    CYN_CONTAINER_NODE_MEMBER(Expr, 1, condition);
    CYN_CONTAINER_NODE_MEMBER(Expr, 2, update);
    CYN_CONTAINER_NODE_MEMBER(Stmt, 3, body);

    VisitableNode();
};

} // namespace cstar