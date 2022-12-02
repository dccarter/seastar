/**
 * Copyright (c) 2022 Suilteam, Carter Mbotho
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 *
 * @author Carter
 * @date 2022-06-15
 */

#include "compiler/symbol.hpp"
#include "compiler/log.hpp"

namespace cstar {

bool SymbolTable::define(std::string_view name,
                         Node::Ptr sym,
                         Range range,
                         SymbolKind kind)
{
    auto it = _symbols.find(name);
    if (it == _symbols.end()) {
        _symbols.emplace(
            name, Symbol<>{kind, std::move(range), std::move(sym), this});
        return true;
    }
    return false;
}

Symbol<> SymbolTable::find(std::string_view name, int depth)
{
    if (depth < 0)
        return {};
    auto it = _symbols.find(name);
    if (it != _symbols.end())
        return it->second;
    if (_enclosing)
        return _enclosing->find(name, depth - 1);

    return {};
}

bool SymbolTable::assign(std::string_view name, Node::Ptr value)
{
    auto scope = this;
    for (auto i = 0; ((i <= MAX_LOOKUP_DEPTH) && (scope != nullptr)); i++) {
        auto it = scope->_symbols.find(name);
        if (it != scope->_symbols.end()) {
            it->second.value = std::move(value);
            return true;
        }
    }
    return false;
}

SymbolTableScope::SymbolTableScope(SymbolTable::Ptr root)
    : _table{std::move(root)}
{
}

void SymbolTableScope::push()
{
    _table = std::make_shared<SymbolTable>(_table);
}

void SymbolTableScope::pop()
{
    _table = _table->enclosing();
    csAssert(_table, "Popping to unknown scope");
}

SymbolTable &SymbolTableScope::table()
{
    csAssert(_table);
    return *_table;
}
} // namespace cstar