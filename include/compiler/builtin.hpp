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

#include <compiler/types.hpp>

namespace cstar::builtin {

    BuiltinType::Ptr voidType();
    BuiltinType::Ptr nullType();
    BuiltinType::Ptr booleanType();
    BuiltinType::Ptr charType();
    BuiltinType::Ptr i8Type();
    BuiltinType::Ptr u8Type();
    BuiltinType::Ptr i16Type();
    BuiltinType::Ptr u16Type();
    BuiltinType::Ptr i32Type();
    BuiltinType::Ptr u32Type();
    BuiltinType::Ptr i64Type();
    BuiltinType::Ptr u64Type();
    BuiltinType::Ptr f32Type();
    BuiltinType::Ptr f64Type();
    BuiltinType::Ptr stringType();
    BuiltinType::Ptr autoType();
    BuiltinType::Ptr getBuiltinType(const std::string_view name);
}