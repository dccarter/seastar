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

#include <compiler/utils.hpp>
#include <compiler/node.hpp>
#include <utility>
#include <unordered_map>

namespace cstar {

    class SymbolTable {
    public:
        CSTAR_PTR(SymbolTable);
        typedef enum {
            kndUnknown,
            kndVariable,
            kndFunc
        } Kind;
        template <typename T = Node>
        using Entry = std::pair<typename T::Ptr, Kind>;

    public:
        static constexpr int MAX_LOOKUP_DEPTH = 500;

    public:
        SymbolTable(SymbolTable::Ptr enclosing = nullptr)
            : _enclosing{std::move(enclosing)}
        {}

        bool define(std::string_view name, Node::Ptr sym, Kind kind);
        Entry<> lookup(std::string_view name, int depth = MAX_LOOKUP_DEPTH);
        template <typename T>
            requires std::is_base_of_v<Node, T>
        Entry<T> lookup(std::string_view name, int depth = MAX_LOOKUP_DEPTH) {
            auto [f, s] = lookup(name, depth);
            return {std::dynamic_pointer_cast<T>(f), s};
        }
        bool assign(std::string_view name, Node::Ptr  sym);
        SymbolTable::Ptr& enclosing() { return _enclosing; }
    private:

        using SymbolsMap = std::unordered_map<std::string_view, Entry<>>;

        SymbolsMap   _symbols{};
        SymbolTable::Ptr _enclosing{};
    };

}