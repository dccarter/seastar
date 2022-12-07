/**
 * Copyright (c) 2022 suilteam, Carter
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 * @author Mpho Mbotho
 * @date 2022-11-26
 */

#include "compiler/parser.hpp"
#include "compiler/builtin.hpp"

#include <utility>

namespace cstar {

Parser::Parser(Log &L, Token::Tange tokens, SymbolTable::Ptr symbols)
    : SymbolTableScope(std::move(symbols)), L{L}, _tokens{std::move(tokens)}
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
        if (_current->kind == Token::SEMICOLON) {
            advance();
            break;
        }

        switch (_current->kind) {
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
            program.insert(declaration());
        }
        catch (Synchronize &) {
            synchronize();
        }
    }

    return !L.hasErrors();
}

FunctionDecl::Ptr Parser::function()
{
    auto fn =
        consume(Token::FUNC, "expecting a 'func' keyword to start a function");

    auto name =
        consume(Token::IDENTIFIER, "expecting the name of the function");
    auto nstr = name->range().toString();

    auto func = std::make_shared<FunctionDecl>(nstr, fn->range());

    try {
        push();
        consume(Token::LPAREN, "expecting an opening paren '('");
        if (!check(Token::RPAREN)) {
            auto params = std::make_shared<StatementList>(previous()->range());
            ParameterStmt::Ptr param = nullptr;
            do {
                param = parameter(param);
                params->range().extend(param->range());
                params->add(param);
            } while (match(Token::COMMA));

            func->params(std::move(params));
        }

        consume(Token::RPAREN, "expecting an closing paren ')'");

        if (match(Token::RARROW)) {
            auto expr = expressionStmt();
            auto block = std::make_shared<Block>(expr->range());
            block->insert(std::move(expr));
            func->body(std::move(block));
        }
        else {
            func->body(block());
        }

        func->range().extend(func->body()->range());
        pop();
    }
    catch (...) {
        pop();
        throw;
    }

    return func;
}

Block::Ptr Parser::block()
{
    auto lb = consume(Token::LBRACE, "expecting an opening brace '{'");
    push();
    auto block = std::make_shared<Block>(lb->range());
    while (!Eof() && !check(Token::RBRACE)) {
        if (auto stmt = declaration())
            block->insert(stmt);
    }

    auto rb = consume(Token::RBRACE, "expecting a closing brace '}'");
    block->range().extend(rb->range());
    pop();

    return block;
}

Stmt::Ptr Parser::statement()
{
    switch (kind()) {
    case Token::IF:
        return ifStmt();
    case Token::WHILE:
        return whileStmt();
    case Token::FOR:
        return forStmt();
    case Token::LBRACE:
        return block();
    default:
        return expressionStmt();
    }
}

Stmt::Ptr Parser::declaration()
{
    auto isComptime = match(Token::AT);
    Stmt::Ptr stmt{nullptr};

    try {
        switch (kind()) {
        case Token::MUT:
        case Token::IMM:
            stmt = variableDecl();
            break;
        case Token::FUNC:
            stmt = function();
            break;
        default:
            stmt = statement();
        }
        if (isComptime) {
            stmt->flags |= gflIsComptime;
        }

        return stmt;
    }
    catch (Synchronize &) {
        synchronize();
        return nullptr;
    }
}

Stmt::Ptr Parser::expressionStmt()
{
    auto expr = expression();
    auto stmt = std::make_shared<ExpressionStmt>(expr, expr->range());

    consume(Token::SEMICOLON, "expecting a semicolon ';' after a statement");

    return stmt;
}

Stmt::Ptr Parser::ifStmt()
{
    auto start = consume(Token::IF, "expecting an 'if' statement");
    consume(Token::LPAREN,
            "expecting an opening paren '(' after an 'if' keyword");
    auto condition = expression();
    consume(Token::RPAREN, "expect a closing paren ')' after an if condition");

    auto stmt = std::make_shared<IfStmt>(std::move(condition), start->range());
    stmt->then(statement());
    if (match(Token::ELSE)) {
        stmt->otherwise(statement());
        stmt->range().extend(stmt->otherwise()->range());
    }
    else {
        stmt->range().extend(stmt->then()->range());
    }

    return stmt;
}

Stmt::Ptr Parser::whileStmt()
{
    auto start = consume(
        Token::WHILE, "expecting a 'while' keyword to start a while statement");
    consume(Token::LPAREN,
            "expecting an opening paren '(' after 'while' keyword");

    auto stmt = std::make_shared<WhileStmt>(expression(), start->range());
    consume(
        Token::RPAREN,
        "expecting an closing paren ')' after a 'while' statement condition");

    if (!match(Token::SEMICOLON)) {
        stmt->body(statement());
        stmt->range().extend(stmt->body()->range());
    }
    else {
        // while(true);
        stmt->range().extend(previous()->range());
    }

    return stmt;
}

Stmt::Ptr Parser::forStmt()
{
    auto start = consume(
        Token::FOR, "expecting a 'for' keyword to start a 'for' statement");
    consume(Token::LPAREN,
            "expecting an open paren ';' to start for loop clauses");

    auto stmt = std::make_shared<ForStmt>(start->range());
    push();
    try {
        if (!match(Token::SEMICOLON)) {
            if (check(Token::MUT, Token::IMM)) {
                stmt->init(variableDecl());
            }
            else {
                stmt->init(expressionStmt());
            }
        }

        if (!check(Token::SEMICOLON)) {
            stmt->condition(expression());
        }
        consume(Token::SEMICOLON,
                "expecting a semicolon ';' after loop condition.");

        if (!check(Token::RPAREN)) {
            stmt->update(expression());
        }
        consume(Token::RPAREN,
                "expecting a closing paren ')' to close for loop clauses.");

        if (!match(Token::SEMICOLON)) {
            stmt->body(statement());
            stmt->range().extend(stmt->body()->range());
        }
        else {
            stmt->range().extend(previous()->range());
        }
        pop();
    }
    catch (...) {
        pop();
        throw;
    }

    return stmt;
}

Stmt::Ptr Parser::variableDecl()
{
    auto modifier = advance();
    auto name =
        consume(Token::IDENTIFIER, "expecting the name of the variable");
    auto nstr = name->range().toString();

    auto decl = std::make_shared<DeclarationStmt>(
        nstr,
        modifier->kind == Token::IMM,
        modifier->range().merge(name->range()));

    if (auto sym = table().find(nstr, 0)) {
        error(name->range(),
              "variable '",
              nstr,
              "' already defined in current scope");
    }

    if (match(Token::COLON)) {
        decl->type(expressionType());
        decl->range().extend(previous()->range());
    }

    if (match(Token::ASSIGN)) {
        decl->value(expression());
        decl->range().extend(decl->value()->range());
    }

    if (decl->value() == nullptr && decl->type() == builtin::autoType()) {
        error(decl->range(),
              "an explicit type must be assigned to an uninitialized "
              "variable");
    }

    if (!table().define(nstr, decl->value(), name->range(), symVariable)) {
        error(name->range(),
              "variable '",
              nstr,
              "' already defined in current scope");
    }
    consume(Token::SEMICOLON,
            "expecting a semicolon ';' after a variable declaration "
            "expression");
    return decl;
}

ParameterStmt::Ptr Parser::parameter(ParameterStmt::Ptr prev)
{

    if ((prev && (prev->flags & gflIsVariadic) == gflIsVariadic)) {
        // ...param: Type ) only allowed as last parameter
        error(prev->range(),
              "parameter '",
              prev->name,
              "' cannot be a variadic parameter, it is followed by another "
              "parameter");
    }

    auto range = _current->range();
    auto isElipsis = match(Token::ELIPSIS);

    auto name =
        consume(Token::IDENTIFIER, "expecting the name of the parameter");
    if (isElipsis)
        range.extend(name->range());

    auto nstr = name->range().toString();

    if (isElipsis && (prev && prev->value())) {
        // ...param: Type ) not allowed when previous parameter has a default
        // value
        error(range,
              "variadiac parameter '",
              nstr,
              "' not allowed after parameters with default arguments");
    }

    if (table().find(nstr, 0)) {
        error(range,
              "parameter '",
              nstr,
              "' already defined in the parameter list");
    }

    consume(
        Token::COLON,
        "expecting a colon ':' after a parameter name and before the parameter "
        "type");

    auto param = std::make_shared<ParameterStmt>(nstr, range);
    param->type(expressionType());

    // TODO use type range
    param->range().extend(previous()->range());

    if (match(Token::ASSIGN)) {
        // parameter default value
        range.extend(previous()->range());
        if (isElipsis) {
            error(range,
                  "default parameter arguments cannot be assigned to variadiac "
                  "parameters");
        }
        auto def = expression();
        param->range().extend(def->range());
        param->value(def);
    }
    else if (prev && (prev->value() != nullptr)) {
        error(param->range(),
              "default argument missing for parameter '",
              nstr,
              "'");
    }

    if (isElipsis) {
        param->flags |= gflIsVariadic;
    }

    table().define(nstr, nullptr, param->range(), symVariable);

    return param;
}

Type::Ptr Parser::expressionType()
{
    auto tok = consume(Token::IDENTIFIER, "expecting a type name");
    if (auto type = builtin::getBuiltinType(tok->range().toString())) {
        return type;
    }
    error("unknown type name (TODO support custom types)");
}

Expr::Ptr Parser::expression() { return assignment(); }

Expr::Ptr Parser::assignment()
{
    auto expr = ternary();
    Expr::Ptr value;
    switch (kind()) {
    case Token::ASSIGN:
        advance();
        value = assignment();
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::MINUSASSIGN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::MINUS, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::PLUSASSGIN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::PLUS, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::MULTASSIGN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::MULT, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::DIVASSIGN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::DIV, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::SHLASSIGN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::SHL, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::SHRASSIGN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::SHR, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::MODASSIGN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::MOD, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::BITANDASSIGN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::BITAND, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    case Token::BITORASSIGN:
        advance();
        value = assignment();
        value = std::make_shared<BinaryExpr>(
            expr, Token::BITOR, value, value->range());
        expr = std::make_shared<AssignmentExpr>(expr, value, expr->range());
        expr->range().extend(value->range());
        return expr;
    default:
        break;
    }

    return expr;
}

Expr::Ptr Parser::ternary()
{
    auto expr = coalescing();
    if (match(Token::QUESTION)) {
        auto ifTrue = ternary();
        consume(Token::COLON,
                "expecting a colon ':' to seperate a ternary expression.");
        auto ifFalse = ternary();
        expr = std::make_shared<TernaryExpr>(
            expr, std::move(ifTrue), ifFalse, expr->range());
        expr->range().extend(ifFalse->range());
    }

    return expr;
}

Expr::Ptr Parser::coalescing()
{
    auto expr = lor();
    if (match(Token::QUESTIONQUESTION)) {
        auto rhs = lor();
        expr =
            std::make_shared<NullishCoalescingExpr>(expr, rhs, expr->range());
        expr->range().extend(rhs->range());
    }

    return expr;
}

Expr::Ptr Parser::lor()
{
    auto expr = land();

    while (match(Token::LOR)) {
        auto rhs = land();
        expr =
            std::make_shared<BinaryExpr>(expr, Token::LOR, rhs, expr->range());
        expr->range().extend(rhs->range());
    }

    return expr;
}

Expr::Ptr Parser::land()
{
    auto expr = bor();

    while (match(Token::LAND)) {
        auto rhs = bor();
        expr =
            std::make_shared<BinaryExpr>(expr, Token::LAND, rhs, expr->range());
        expr->range().extend(rhs->range());
    }

    return expr;
}

Expr::Ptr Parser::bor()
{
    auto expr = bxor();

    while (match(Token::BITOR)) {
        auto rhs = bxor();
        expr = std::make_shared<BinaryExpr>(
            expr, Token::BITOR, rhs, expr->range());
        expr->range().extend(rhs->range());
    }

    return expr;
}

Expr::Ptr Parser::bxor()
{
    auto expr = band();

    while (match(Token::BITXOR)) {
        auto rhs = band();
        expr = std::make_shared<BinaryExpr>(
            expr, Token::BITXOR, rhs, expr->range());
        expr->range().extend(rhs->range());
    }

    return expr;
}

Expr::Ptr Parser::band()
{
    auto expr = equality();

    while (match(Token::BITAND)) {
        auto rhs = equality();
        expr = std::make_shared<BinaryExpr>(
            expr, Token::BITAND, rhs, expr->range());
        expr->range().extend(rhs->range());
    }

    return expr;
}

Expr::Ptr Parser::call()
{
    auto expr = primary();

    while (true) {
        if (match(Token::LPAREN)) {
            auto arguments =
                std::make_shared<ExpressionList>(previous()->range());
            if (!check(Token::RPAREN)) {
                do {
                    auto arg = expression();
                    arguments->range().extend(arg->range());
                    arguments->add(std::move(arg));
                } while (match(Token::COMMA));
            }

            auto tok = consume(
                Token::RPAREN,
                "expecting a closing paren '(' to end function arguments");
            arguments->range().extend(tok->range());

            auto call = std::make_shared<CallExpr>(expr, expr->range());
            call->range().extend(tok->range());
            call->arguments(std::move(arguments));

            expr = std::move(call);
        }
        else {
            break;
        }
    }

    return expr;
}

Expr::Ptr Parser::equality()
{
    auto expr = comparison();

    while (match(Token::NEQ, Token::EQUAL)) {
        auto op = previous();
        auto right = comparison();
        expr =
            std::make_shared<BinaryExpr>(expr, op->kind, right, expr->range());
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
        expr = std::make_shared<BinaryExpr>(expr, op, right, expr->range());
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

        expr = std::make_shared<BinaryExpr>(expr, op, right, expr->range());
        expr->range().extend(right->range());
    }

    return expr;
}

Expr::Ptr Parser::factor()
{
    auto expr = nots();

    while (match(Token::DIV, Token::MULT)) {
        auto op = previous()->kind;
        auto right = nots();

        expr = std::make_shared<BinaryExpr>(expr, op, right, expr->range());
        expr->range().extend(right->range());
    }

    return expr;
}

Expr::Ptr Parser::unary()
{
    if (match(Token::PLUS, Token::MINUS)) {
        auto op = previous();
        auto right = unary();

        auto expr = std::make_shared<UnaryExpr>(op->kind, right, op->range());
        expr->range().extend(right->range());

        return expr;
    }

    return prefix();
}

Expr::Ptr Parser::nots()
{
    if (match(Token::COMPLEMENT, Token::NOT)) {
        auto op = previous();
        auto right = nots();

        auto expr = std::make_shared<UnaryExpr>(op->kind, right, op->range());
        expr->range().extend(right->range());

        return expr;
    }

    return unary();
}

Expr::Ptr Parser::prefix()
{
    if (match(Token::MINUSMINUS, Token::PLUSPLUS)) {
        auto op = previous();
        auto right = prefix();

        auto expr = std::make_shared<PrefixExpr>(op->kind, right, op->range());
        expr->range().extend(right->range());

        return expr;
    }

    auto expr = call();
    while (match(Token::PLUSPLUS, Token::MINUSMINUS)) {
        auto op = previous();
        expr = std::make_shared<PostfixExpr>(op->kind, expr, expr->range());
        expr->range().extend(op->range());
    }

    return expr;
}

Expr::Ptr Parser::primary()
{
    if (auto expr = literal()) {
        advance();
        return expr;
    }

    if (match(Token::LSTREXPR)) {
        auto expr = std::make_shared<StringExpressionExpr>(previous()->range());
        while (!match(Token::RSTREXPR)) {
            auto node = expression();
            expr->addPart(node);
        }
        expr->range().extend(previous()->range());
        return expr;
    }

    if (check(Token::IDENTIFIER)) {
        auto &tok = *advance();
        auto sym = table().find(tok.range().toString());
        if (!sym) {
            error(tok.range(),
                  "accessing an undefined variable '",
                  tok.range().toString(),
                  "'");
        }
        return std::make_shared<VariableExpr>(tok.range().toString(),
                                              tok.range());
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
        return std::make_shared<IntegerExpr>(tok->value<uint64_t>(),
                                             tok->range());
    case Token::FLOAT:
        return std::make_shared<FloatExpr>(tok->value<double>(), tok->range());
    case Token::STRING:
        return std::make_shared<StringExpr>(tok->value<std::string_view>(),
                                            tok->range());
    default:
        return nullptr;
    }
}

VariableExpr::Ptr Parser::variable()
{
    auto var = consume(Token::IDENTIFIER, "expecting an identifier");

    return std::make_shared<VariableExpr>(var->range().toString(),
                                          var->range());
}
} // namespace cstar