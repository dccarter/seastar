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

#include "compiler/lexer.hpp"
#include "compiler/ast.hpp"

namespace cstar {

    class Parser {
        class Synchronize : public std::exception {
        public:
            using std::exception::exception;
        };

    public:
        Parser(Log& L, Token::Tange tokens);

        bool parse(Program& program);

    private:
        Log& L;

        FunctionDecl::Ptr functionDecl();
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

        Token::Ref advance();
        Token::Ref peek();
        Token::Ref previous();

        template <typename ...Args>
        bool check(Args&&... args) {
            return !Eof() && ((_current->kind == args) || ...);
        }

        template<typename ...Args>
        bool match(Args&&... args) {
            if (check(std::forward<Args>(args)...)) {
                advance();
                return true;
            }
            return false;
        }

        template<typename ...Args>
        [[noreturn]] void error(const Token::Ref& tok, Args&... args) {
            L.error(tok->range(), std::forward<Args>(args)...);
            throw Synchronize();
        }

        template<typename ...Args>
        [[noreturn]] void error(const char* fmt, Args... args) {
            error(_current, fmt, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        Token::Ref expect(Token::Kind kind, Args&&... args) {
            if (check(kind)) {
                return _current;
            }
            error(_current, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        Token::Ref consume(Token::Kind kind, Args&&... args) {
            if (check(kind)) {
                auto tok = _current;
                advance();
                return tok;
            }
            error(_current, std::forward<Args>(args)...);
        }


        void synchronize();

        bool Eof() { return _current == _tokens.second; }

        Token::Tange _tokens;
        Token::Ref   _current;
    };
}