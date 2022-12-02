/**
 * Copyright (c) 2022 suilteam, Carter 
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Mpho Mbotho
 * @date 2022-11-25
 */

#include "compiler/node.hpp"
#include "compiler/log.hpp"

namespace cstar {

    Node::Ptr ContainerNode::get(int i)
    {
        csAssert(i < _children.size(), "index out of bounds");
        return _children[i];
    }

    Node::Ptr ContainerNode::get(int i) const
    {
        csAssert(i < _children.size(), "index out of bounds");
        return _children[i];
    }

    void ContainerNode::set(int i, Node::Ptr node)
    {
        if (i >= _children.size()) {
            _children.resize(i + 1, {nullptr});
        }
        _children[i] = node;
    }
}