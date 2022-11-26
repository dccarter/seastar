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

#include <compiler/node.hpp>
#include <compiler/utils.hpp>
#include <unordered_map>

namespace cstar {

    class Type : public ContainerNode {
    public:
        CSTAR_PTR(Type);
        using ContainerNode::ContainerNode;

        virtual ~Type() = default;

        virtual bool isAssignable(const Type::Ptr from);

        virtual size_t size() const { return 0; }

        virtual std::string_view name() const { return ""; }

        static Type::Ptr leastUpperBound(Type::Ptr t1, Type::Ptr t2);

        VisitableNode();
    };

    class BuiltinType : public virtual Type {
    public:
        CSTAR_PTR(BuiltinType);

        using Type::Type;
        BuiltinType(std::string_view name)
            : _name{name}
        {}


        std::string_view name() const override { return _name; }

        VisitableNode();

    private:
        std::string_view _name{"unknown"};
    };

    class BoolType : public BuiltinType {
    public:
        CSTAR_PTR(BoolType);

        BoolType() : BuiltinType("bool") {}

        virtual size_t size() const override { return sizeof(bool); }
    };

    class CharType : public BuiltinType {
    public:
        CSTAR_PTR(CharType);
        CharType() : BuiltinType("char") {}

        size_t size() const override { return sizeof(uint32_t); }
    };

    class StringType : public BuiltinType {
    public:
        CSTAR_PTR(StringType);
        StringType() : BuiltinType("string") {}
        size_t size() const override { return sizeof(const char*); }
    };

    class IntegerType final : public BuiltinType {
    public:
        CSTAR_PTR(IntegerType);
        IntegerType(std::string_view name, uint8_t bits, bool isSigned)
            : BuiltinType(name), bits{bits}, isSigned{isSigned}
        {}

        static IntegerType::Ptr bigger(IntegerType::Ptr i1, IntegerType::Ptr i2);

        size_t size() const override { return bits/8; }

        uint8_t bits{0};
        bool    isSigned{true};
    };

    class FloatType : public BuiltinType {
    public:
        CSTAR_PTR(FloatType);
        FloatType(std::string_view name, uint8_t bits)
            : BuiltinType(std::move(name)), bits{bits}
        {}

        size_t size() const override { return bits/8; }

        uint8_t bits{0};
    };
}