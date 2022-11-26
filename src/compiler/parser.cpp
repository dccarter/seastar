/**
 * Copyright (c) 2022 suilteam, Carter 
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Mpho Mbotho
 * @date 2022-11-26
 */

#include <utility>

#include "compiler/parser.hpp"

namespace cstar {

    Parser::Parser(Log &L, Token::Tange tokens)
        : L{L}, _tokens{std::move(tokens)}
    {
        _current = _tokens.first;
    }

    Token::Ref Parser::peek()
    {
        if (Eof())
            return _current;
        return std::next(_current);
    }

    Token::Ref Parser::advance()
    {
        auto curr = _current;
        if (!Eof()) {
            std::advance(_current, 1);
        }
        return curr;
    }

    Token::Ref Parser::previous()
    {
        if (_current == _tokens.first)
            return _current;
        return std::prev(_current);
    }

    void Parser::synchronize()
    {
        advance();

        while (!Eof()) {
            if (previous()->kind == Token::SEMICOLON) return;

            switch (peek()->kind) {
                case Token::STRUCT:
                case Token::FUNC:
                case Token::IMM:
                case Token::MUT:
                case Token::FOR:
                case Token::IF:
                case Token::WHILE:
                case Token::UNION:
                case Token::RETURN:
                    return;
                default:
                    break;
            }

            advance();
        }
    }

    bool Parser::parse(Program &program)
    {
        while (!Eof()) {
            try {
                program.insert(functionDecl());
            }
            catch (Synchronize&) {
                synchronize();
            }
        }

        return !L.hasErrors();
    }

    FunctionDecl::Ptr Parser::functionDecl()
    {
        auto fn = consume(Token::FUNC, "expecting a 'func' keyword to start a function");

        auto name = variable();
        auto func = std::make_shared<FunctionDecl>(std::move(name), fn->range());

        // TODO parse parameters
        consume(Token::LPAREN, "expecting an opening paren '('");
        consume(Token::RPAREN, "expecting an closing paren ')'");

        if (match(Token::RARROW)) {
            auto expr = expression();
            auto block = std::make_shared<Block>(expr->range());
            block->insert(std::move(expr));
            func->body(std::move(block));
        }
        else {
            func->body(block());
        }

        func->range().extend(func->body()->range());

        return func;
    }

    Block::Ptr Parser::block()
    {
        auto lb = consume(Token::LBRACE, "expecting an opening brace '{'");

        auto block = std::make_shared<Block>(lb->range());
        while (!check(Token::RBRACE)) {
            block->insert(expression());
            consume(Token::SEMICOLON, "expecting a semicolon ';' after an expression");
        }

        auto rb = consume(Token::RBRACE, "expecting a closing brace '}'");
        block->range().extend(rb->range());

        return std::move(block);
    }

    Expr::Ptr Parser::expression()
    {
        return equality();
    }

    Expr::Ptr Parser::equality()
    {
        auto expr = comparison();

        while (match(Token::NEQ, Token::EQUAL)) {
            auto op = previous();
            auto right = comparison();
            expr = std::make_shared<BinaryExpr>(
                    expr, op->kind, right, expr->range());
            expr->range().extend(right->range());
        }

        return expr;
    }

    Expr::Ptr Parser::comparison()
    {
        auto expr = terminal();

        while (match(Token::GT, Token::GTE, Token::LT, Token::LTE)) {
            auto op = previous()->kind;
            auto right = terminal();
            expr = std::make_shared<BinaryExpr>(
                    expr, op, right, expr->range());
            expr->range().extend(right->range());
        }

        return expr;
    }

    Expr::Ptr Parser::terminal()
    {
        auto expr = factor();

        while (match(Token::MINUS, Token::PLUS)) {
            auto op = previous()->kind;
            auto right = factor();

            expr = std::make_shared<BinaryExpr>(
                    expr, op, right, expr->range());
            expr->range().extend(right->range());
        }

        return expr;
    }

    Expr::Ptr Parser::factor()
    {
        auto expr = unary();

        while (match(Token::DIV, Token::MULT)) {
            auto op = previous()->kind;
            auto right = unary();

            expr = std::make_shared<BinaryExpr>(
                    expr, op, right, expr->range());
            expr->range().extend(right->range());
        }

        return expr;
    }

    Expr::Ptr Parser::unary()
    {
        if (match(Token::NOT, Token::MINUS)) {
            auto op = previous();
            auto right = unary();

            auto expr = std::make_shared<UnaryExpr>(op->kind, right, op->range());
            expr->range().extend(right->range());

            return expr;
        }

        return primary();
    }

    Expr::Ptr Parser::primary()
    {
        if (auto expr = literal()) {
            advance();
            return expr;
        }

        auto range = _current->range();
        if (match(Token::LPAREN)) {
            auto expr = expression();
            range.extend(_current->range());

            consume(Token::RPAREN, "expecting a closing ')' after expression.");
            return std::make_shared<GroupingExpr>(expr, range);
        }

        error("unexpected token, expecting an expression");
    }

    LiteralExpr::Ptr Parser::literal()
    {
        auto tok = _current;
        switch (tok->kind) {
            case Token::TRUE:
            case Token::FALSE:
                return std::make_shared<BoolExpr>(tok->value<bool>(), tok->range());
            case Token::CHAR:
                return std::make_shared<CharExpr>(tok->value<uint32_t>(), tok->range());
            case Token::INTEGER:
                return std::make_shared<IntegerExpr>(tok->value<uint64_t>(), tok->range());
            case Token::FLOAT:
                return std::make_shared<FloatExpr>(tok->value<double>(), tok->range());
            case Token::STRING:
                return std::make_shared<StringExpr>(tok->value<std::string_view>(), tok->range());
            default:
                return nullptr;
        }
    }

    VariableExpr::Ptr Parser::variable()
    {
        auto var = consume(Token::IDENTIFIER, "expecting an identifier");

        return std::make_shared<VariableExpr>(var->range().toString(), var->range());
    }
}