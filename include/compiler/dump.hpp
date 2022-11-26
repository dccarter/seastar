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

#include "compiler/vistor.hpp"
#include <cstdint>

namespace cstar {

    class AstDump final : public Visitor {
    public:
        void dump(Program& program);

#define OVERRIDE_VISIT(XX)  void visit(XX &node) override;
        NODE_LIST(OVERRIDE_VISIT)
#undef OVERRIDE_VISIT

#define OVERRIDE_VISIT(XX)  void visit(XX##Type &node) override;
        NODE_TYPE_LIST(OVERRIDE_VISIT)
#undef OVERRIDE_VISIT

#define OVERRIDE_VISIT(XX)  void visit(XX##Expr &node) override;
        NODE_EXPR_LIST(OVERRIDE_VISIT)
#undef OVERRIDE_VISIT

#define OVERRIDE_VISIT(XX)  void visit(XX##Stmt &node) override;
        NODE_STMT_LIST(OVERRIDE_VISIT)
#undef OVERRIDE_VISIT

#define OVERRIDE_VISIT(XX)  void visit(XX##Decl &node) override;
        NODE_DECL_LIST(OVERRIDE_VISIT)
#undef OVERRIDE_VISIT

    private:
        std::uint32_t level{0};
    };
}