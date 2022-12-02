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

#include <compiler/node.hpp>
#include <compiler/utils.hpp>
#include <unordered_map>
#include <utility>

namespace cstar {

class SymbolTable;

typedef enum { symUnknown, symVariable, symFunc } SymbolKind;

template <typename T = Node>
struct Symbol {
    SymbolKind kind{symUnknown};
    Range range{};
    typename T::Ptr value{nullptr};
    SymbolTable *scope{nullptr};
    operator bool() const { return kind != symUnknown; }
};

class SymbolTable {
public:
    CSTAR_PTR(SymbolTable);

public:
    static constexpr int MAX_LOOKUP_DEPTH = 500;

public:
    SymbolTable(SymbolTable::Ptr enclosing = nullptr)
        : _enclosing{std::move(enclosing)}
    {
    }

    bool define(std::string_view name,
                Node::Ptr sym,
                Range range,
                SymbolKind kind);

    Symbol<> find(std::string_view name, int depth = MAX_LOOKUP_DEPTH);

    // clang-format off
    template <typename T>
        requires std::is_base_of_v<Node, T>
    Symbol<> lookup( // clang-format on
        std::string_view name,
        int depth = MAX_LOOKUP_DEPTH)
    {
        auto sym = find(name, depth);
        return Symbol<T>{sym.kind,
                         sym.range,
                         std::dynamic_pointer_cast<T>(sym.value),
                         sym.scope};
    }

    bool assign(std::string_view name, Node::Ptr value);

    SymbolTable::Ptr enclosing() { return _enclosing; }

private:
    using SymbolsMap = std::unordered_map<std::string_view, Symbol<>>;

    SymbolsMap _symbols{};
    SymbolTable::Ptr _enclosing{};
};

class SymbolTableScope {
public:
    SymbolTableScope(SymbolTable::Ptr root);
    void push();
    void pop();
    SymbolTable &table();

private:
    SymbolTable::Ptr _table;
};

} // namespace cstar