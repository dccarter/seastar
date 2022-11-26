/**
 * Copyright (c) 2022 Suilteam, Carter Mbotho
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Carter
 * @date 2022-06-14
 */

#include "compiler/types.hpp"

namespace cstar {

    bool Type::isAssignable(const Type::Ptr from)
    {
        return this == from.get();
    }
}