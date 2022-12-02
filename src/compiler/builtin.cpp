/**
 * Copyright (c) 2022 suilteam, Carter 
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Mpho Mbotho
 * @date 2022-11-25
 */

#include "compiler/builtin.hpp"

namespace cstar {

#define BUILTIN_CREATE(I, N)                                            \
    static auto s##I##Type = mk<BuiltinType>(N);                        \
    return s##I##Type \

    BuiltinType::Ptr builtin::voidType()
    {
        static auto sVoidType = mk<BuiltinType>("void");
        return sVoidType;
    }

    BuiltinType::Ptr builtin::autoType()
    {
        static auto sVoidType = mk<BuiltinType>("auto");
        return sVoidType;
    }

    BuiltinType::Ptr builtin::nullType()
    {
        BUILTIN_CREATE(Null, "null");
    }

#undef BUILTIN_CREATE

#define BUILTIN_CREATE(T, ...)                             \
    static auto s##T##Type = mk<T##Type>(__VA_ARGS__);     \
    return s##T##Type

    BuiltinType::Ptr builtin::booleanType()
    {
        BUILTIN_CREATE(Bool);
    }

    BuiltinType::Ptr builtin::charType()
    {
        BUILTIN_CREATE(Char);
    }

    BuiltinType::Ptr builtin::stringType()
    {
        BUILTIN_CREATE(String);
    }

#undef BUILTIN_CREATE

#define u false
#define i true

#define BUILTIN_CREATE(S, N)                                     \
    static auto s_##S##N##Type = mk<IntegerType>(#S#N, N, S);    \
    return s_##S##N##Type

    BuiltinType::Ptr builtin::i8Type()
    {
        BUILTIN_CREATE(i, 8);
    }

    BuiltinType::Ptr builtin::u8Type()
    {
        BUILTIN_CREATE(u, 8);
    }

    BuiltinType::Ptr builtin::i16Type()
    {
        BUILTIN_CREATE(i, 16);
    }

    BuiltinType::Ptr builtin::u16Type()
    {
        BUILTIN_CREATE(u, 16);
    }

    BuiltinType::Ptr builtin::i32Type()
    {
        BUILTIN_CREATE(i, 32);
    }

    BuiltinType::Ptr builtin::u32Type()
    {
        BUILTIN_CREATE(u, 32);
    }

    BuiltinType::Ptr builtin::i64Type()
    {
        BUILTIN_CREATE(i, 64);
    }

    BuiltinType::Ptr builtin::u64Type()
    {
        BUILTIN_CREATE(u, 64);
    }

#undef u
#undef i
#undef BUILTIN_CREATE

#define BUILTIN_CREATE(N)                                      \
    static auto s_Float##N##Type = mk<FloatType>("f"#N, N);    \
    return s_Float##N##Type

    BuiltinType::Ptr builtin::f32Type()
    {
        BUILTIN_CREATE(32);
    }

    BuiltinType::Ptr builtin::f64Type()
    {
        BUILTIN_CREATE(64);
    }
#undef BUILTIN_CREATE

    BuiltinType::Ptr builtin::getBuiltinType(const std::string_view name)
    {
        static const std::unordered_map<std::string_view, BuiltinType::Ptr> sBuiltins = {
            {voidType()->name(), voidType()},
            {autoType()->name(), autoType()},
            {nullType()->name(), nullType()},
            {booleanType()->name(), booleanType()},
            {charType()->name(), charType()},
            {i8Type()->name(), i8Type()},
            {u8Type()->name(), u8Type()},
            {i16Type()->name(), i16Type()},
            {u16Type()->name(), u16Type()},
            {i32Type()->name(), i32Type()},
            {u32Type()->name(), u32Type()},
            {i64Type()->name(), i64Type()},
            {u64Type()->name(), u64Type()},
            {f32Type()->name(), f32Type()},
            {f64Type()->name(), f64Type()},
            {stringType()->name(), stringType()}
        };
        auto it = sBuiltins.find(name);
        if (it != sBuiltins.end()) {
            return it->second;
        }
        return nullptr;
    }

}