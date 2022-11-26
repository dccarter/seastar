/**
 * Copyright (c) 2022 suilteam, Carter 
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Mpho Mbotho
 * @date 2022-11-26
 */

#pragma once

#include <unordered_map>
#include <string>

namespace cstar {

    class Strings final {
    public:

        static std::string_view intern(std::string str) {
            return _impl.intern(std::move(str));
        }

        static std::string_view intern(std::string_view str) {
            return _impl.intern(str);
        }

    private:
        Strings() = default;

        class StringsImpl {
        public:
            std::string_view intern(std::string str);
            std::string_view intern(std::string_view str);
            std::unordered_map<std::string_view, std::string> _strings{};
        };

        static StringsImpl _impl;
    };
}