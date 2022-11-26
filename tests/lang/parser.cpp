/**
 * Copyright (c) 2022 Suilteam, Carter Mbotho
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 * 
 * @author Carter
 * @date 2022-03-04
 */

#include "compiler/dump.hpp"
#include "compiler/lexer.hpp"
#include "compiler/source.hpp"
#include "compiler/parser.hpp"

using cstar::Lexer;
using cstar::Log;
using cstar::Source;
using cstar::Parser;
using cstar::AstDump;

int main(int argc, char *argv[])
{
    auto testScript = CSTAR_LANG_DIR "/parser.cstr";
    Log L;
    Source src{L, testScript};
    Lexer lexer{L, src};
    if (!lexer.tokenize())
        abortCompiler(L);

    Parser parser(L, lexer.tange());
    cstar::Program program;
    if (!parser.parse(program)) {
        abortCompiler(L);
    }
    AstDump dump;
    dump.dump(program);

    abortCompiler(L);
}