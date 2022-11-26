/**
 * Copyright (c) 2022 suilteam, Carter 
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Mpho Mbotho
 * @date 2022-11-26
 */

#include "compiler/strings.hpp"

namespace cstar {

    Strings::StringsImpl Strings::_impl{};

    std::string_view Strings::StringsImpl::intern(std::string str)
    {
        auto key = std::string_view{str};
        auto it = _strings.emplace(key, std::move(str));
        return it.first->second;
    }

    std::string_view Strings::StringsImpl::intern(std::string_view str)
    {
        return intern(std::string{str});
    }
}