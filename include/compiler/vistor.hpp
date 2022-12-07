/**
 * Copyright (c) 2022 Suilteam, Carter Mbotho
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 * @author Carter
 * @date 2022-06-14
 */

#pragma once

namespace cstar {

class Program;

#define NODE_TYPE_LIST(XX)                                                     \
    XX(Builtin)                                                                \
    XX(Void)                                                                   \
    XX(Bool)                                                                   \
    XX(Char)                                                                   \
    XX(String)                                                                 \
    XX(Integer)                                                                \
    XX(Float)

#define NODE_STMT_LIST(XX)                                                     \
    XX(Declaration)                                                            \
    XX(Expression)                                                             \
    XX(If)                                                                     \
    XX(While)                                                                  \
    XX(For)                                                                    \
    XX(Parameter)

#define NODE_DECL_LIST(XX) XX(Function)

#define NODE_EXPR_LIST(XX)                                                     \
    XX(Bool)                                                                   \
    XX(Char)                                                                   \
    XX(Integer)                                                                \
    XX(Float)                                                                  \
    XX(String)                                                                 \
    XX(Variable)                                                               \
    XX(Binary)                                                                 \
    XX(Unary)                                                                  \
    XX(Grouping)                                                               \
    XX(Assignment)                                                             \
    XX(Call)

#define NODE_LIST(XX)                                                          \
    XX(Node)                                                                   \
    XX(ContainerNode)                                                          \
    XX(Block)                                                                  \
    XX(ExpressionList)                                                         \
    XX(StatementList)

#define XX(N) struct N;
NODE_LIST(XX)
#undef XX

#define XX(N) struct N##Type;
NODE_TYPE_LIST(XX)
#undef XX

#define XX(N) struct N##Stmt;
NODE_STMT_LIST(XX)
#undef XX

#define XX(N) struct N##Expr;
NODE_EXPR_LIST(XX)
#undef XX

#define XX(N) struct N##Decl;
NODE_DECL_LIST(XX)
#undef XX

class Visitor {
public:
#define XX(N)                                                                  \
    virtual void visit(N &node) {}
    NODE_LIST(XX)
#undef XX

#define XX(N) virtual void visit(N##Type &node){};
    NODE_TYPE_LIST(XX)
#undef XX

#define XX(N)                                                                  \
    virtual void visit(N##Stmt &node) {}
    NODE_STMT_LIST(XX)
#undef XX

#define XX(N)                                                                  \
    virtual void visit(N##Expr &node) {}
    NODE_EXPR_LIST(XX)
#undef XX

#define XX(N)                                                                  \
    virtual void visit(N##Decl &node) {}
    NODE_DECL_LIST(XX)
#undef XX
};

#define VisitableNode()                                                        \
    virtual void accept(Visitor &visitor) override { visitor.visit(*this); }
} // namespace cstar