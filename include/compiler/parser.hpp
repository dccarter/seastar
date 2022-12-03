/**
 * Copyright (c) 2022 suilteam, Carter
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 * @author Mpho Mbotho
 * @date 2022-11-25
 */

#pragma once

#include "compiler/ast.hpp"
#include "compiler/lexer.hpp"
#include "compiler/symbol.hpp"

namespace cstar {

class Parser : protected SymbolTableScope {
    class Synchronize : public std::exception {
    public:
        using std::exception::exception;
    };

public:
    Parser(Log &L, Token::Tange tokens, SymbolTable::Ptr symbols);

    bool parse(Program &program);

private:
    Log &L;

    FunctionDecl::Ptr function();
    Block::Ptr block();
    LiteralExpr::Ptr literal();
    VariableExpr::Ptr variable();
    Expr::Ptr unary();
    Expr::Ptr factor();
    Expr::Ptr equality();
    Expr::Ptr terminal();
    Expr::Ptr comparison();
    Expr::Ptr primary();
    Expr::Ptr expression();
    Expr::Ptr assignment();
    Expr::Ptr lor();
    Expr::Ptr land();
    Expr::Ptr call();
    Stmt::Ptr statement();
    Stmt::Ptr variableDecl();
    Stmt::Ptr declaration();
    Stmt::Ptr expressionStmt();
    Stmt::Ptr ifStmt();
    Stmt::Ptr whileStmt();
    Stmt::Ptr forStmt();
    ParameterStmt::Ptr parameter(ParameterStmt::Ptr prev = nullptr);
    Type::Ptr expressionType();

    Token::Ref advance();
    Token::Ref peek();
    Token::Ref previous();
    Token::Kind kind() { return Eof() ? Token::EoF : _current->kind; }
    template <typename... Args>
    bool check(Args &&...args)
    {
        return !Eof() && ((_current->kind == args) || ...);
    }

    template <typename... Args>
    bool match(Args &&...args)
    {
        if (check(std::forward<Args>(args)...)) {
            advance();
            return true;
        }
        return false;
    }

    template <typename... Args>
    [[noreturn]] void error(const Range &range, Args &&...args)
    {
        L.error(range, std::forward<Args>(args)...);
        throw Synchronize();
    }

    template <typename... Args>
    [[noreturn]] void error(const char *fmt, Args &&...args)
    {
        error(_current->range(), fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    Token::Ref expect(Token::Kind kind, Args &&...args)
    {
        if (check(kind)) {
            return _current;
        }
        error(_current->range(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    Token::Ref consume(Token::Kind kind, Args &&...args)
    {
        if (check(kind)) {
            auto tok = _current;
            advance();
            return tok;
        }
        error(_current->range(), std::forward<Args>(args)...);
    }

    void synchronize();

    bool Eof() { return _current == _tokens.second; }

    Token::Tange _tokens;
    Token::Ref _current;
};
} // namespace cstar