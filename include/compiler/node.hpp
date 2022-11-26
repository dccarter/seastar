/**
 * Copyright (c) 2022 suilteam, Carter 
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Mpho Mbotho
 * @date 2022-11-24
 */

#pragma once

#include <compiler/token.hpp>
#include <compiler/vistor.hpp>

#include <ranges>

namespace cstar {

    struct Node {
        CSTAR_PTR(Node);

        Node(Range range = {}) : _range{std::move(range)} {}

        void range(Range range) { _range = std::move(range); }

        const Range &range() const { return _range; }
        Range &range() { return _range; }

        virtual void accept(Visitor& visitor) { visitor.visit(*this); }

    private:
        Range _range{};
    };

    class ContainerNode : public Node {
    public:
        CSTAR_PTR(ContainerNode);
        using Node::Node;
        using iterator = std::vector<Node::Ptr>::iterator;
        using const_iterator = std::vector<Node::Ptr>::const_iterator;

#define CYN_CONTAINER_NODE_MEMBER(T, idx, name)                 \
    typename T::Ptr name() const {                              \
        return std::dynamic_pointer_cast<T>(get(idx));          \
    }                                                           \
    typename T::Ptr name() {                                    \
        return std::dynamic_pointer_cast<T>(get(idx));          \
    }                                                           \
    void name (Node::Ptr node) { set(idx, std::move(node)); }

#define CYN_CONTAINER_NODE_VIEW(idx, name)                                  \
        auto name () { return std::ranges::drop_view{all(), idx}; }          \
        auto name () const { return std::ranges::drop_view{all(), idx}; }    \


        void insert(Node::Ptr node) {
            set(int(_children.size()), std::move(node));
        }

        template<typename T, typename ...Args>
            requires std::is_base_of_v<Node, T>
        void create(Args&&... args) {
            insert(std::make_shared<T>(std::forward<Args>(args)...));
        }

        vec<Node::Ptr>& all() { return _children; }
        const vec<Node::Ptr>& all() const { return _children; }

        VisitableNode()

    protected:
        Node::Ptr get(int i);
        Node::Ptr get(int i) const;
        void set(int i, Node::Ptr);
    private:
        std::vector<Node::Ptr> _children{};
    };
}
