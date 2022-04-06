// Copyright 2022, Zephyr Authors
// All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __CHEAT_STRUCT_H__
#define __CHEAT_STRUCT_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <stdint.h>

#include <cheat_config.h>

#if defined(HAVE_CPLUSPLUS)
namespace cheat
{
#endif
    enum MCOperandType // class MCOperand
    {
        kInvalid,      ///< Uninitialized.
        kRegister,     ///< Register operand.
        kImmediate,    ///< Immediate operand.
        kSFPImmediate, ///< Single-floating-point immediate operand.
        kDFPImmediate, ///< Double-Floating-point immediate operand.
        kExpr,         ///< Relocatable immediate operand.
        kInst          ///< Sub-instruction operand.
    };

    enum MCObjectType
    {
        MC_LABEL,
        MC_DESC,
        MC_ATTRIBUTE,
        MC_COMMON,
        MC_ZEROFILL,
        MC_CHANGESECTION,
        MC_BYTES,
        MC_VALUEIMPL,
        MC_INSTRUCTION
    };

    enum MCGenCodeSyntax
    {
        MC_SYNTAX_ATT,
        MC_SYNTAX_INTEL
    };

    enum MCGenCodeOption
    {
        MC_OPTION_SYNTAX,
        MC_OPTION_PRETTY_PRINT
    };

    enum MCGenCodePretty
    {
        MC_OPTION_PRINT,
        MC_OPTION_NOPRINT
    };

    enum MCGenCodeError
    {
        MC_SUCCESS,
        MC_FAILED_WITH_DIAGNOSTICS,
        MC_UNSUPPORTED_TARGET,
        MC_FAILED,
        MC_INVALID_OPTION,
        MC_INVALID_OPTION_VALUE
    };

    struct MCKind
    {
        char name[256];
        uint32_t value;
    };

    struct MCFixup
    {
        struct { uint32_t value; } kind;

        struct
        {
            uint32_t target_offset;
            uint32_t target_size;
        } info;

        uint32_t offset;

        struct
        {
            struct { uint32_t value; } kind;

            struct { uint32_t value; } opcode;

            struct
            {
                struct { uint32_t value; } kind;

                uint64_t value;
                uint32_t fotmat;
            } lhs;

            struct
            {
                struct { uint32_t value; } kind;

                uint64_t value;
                uint32_t fotmat;
            } rhs;

        } value;
    };

    struct MCInst
    {
        uint64_t data;         // assembly instruction max length 8 bytes
        int8_t binary[16];

        struct
        {
            struct { uint32_t value; } opcode;
            uint32_t flags;
        } inst;

        struct
        {
            uint32_t opcode;
            uint32_t numoperands;
            uint32_t numdefs;
            uint32_t size;
            uint32_t schedclass;
            uint32_t flags;
            uint64_t tsflags;

            struct
            {
                uint64_t canfoldasload : 1;
                uint64_t has_delayslot : 1;
                uint64_t has_optionaldef : 1;
                uint64_t has_unmodeledsideeffects : 1;
                uint64_t reserved_1 : 2;
                uint64_t is_add : 1;
                uint64_t is_authenticated : 1;

                uint64_t is_barrier : 1;
                uint64_t is_bitcast : 1;
                uint64_t is_branch : 1;
                uint64_t is_call : 1;
                uint64_t is_compare : 1;
                uint64_t is_conditionalbranch : 1;
                uint64_t is_convergent : 1;
                uint64_t is_extractsubreglike : 1;

                uint64_t is_indirectbranch : 1;
                uint64_t is_insertsubreglike : 1;
                uint64_t is_moveimmediate : 1;
                uint64_t is_movereg : 1;
                uint64_t is_notduplicable : 1;
                uint64_t is_predicable : 1;
                uint64_t is_pseudo : 1;
                uint64_t is_regsequencelike : 1;

                uint64_t is_return : 1;
                uint64_t is_select : 1;
                uint64_t is_terminator : 1;
                uint64_t is_trap : 1;
                uint64_t is_unconditionalbranch : 1;
                uint64_t is_variadic : 1;
                uint64_t may_load : 1;
                uint64_t may_raisefpexception : 1;

                uint64_t may_store : 1;
                uint64_t numdefs : 8;
                uint64_t variadicopsaredefs : 1;

                uint64_t reserved_2 : 21;
            } value;
        } desc;
    };



    struct section_kind
    {
        uint16_t is_bss : 1;
        uint16_t is_common : 1;
        uint16_t is_data : 1;
        uint16_t is_executeonly : 1;
        uint16_t is_globalwriteabledata : 1;
        uint16_t is_mergeablecstring : 1;
        uint16_t is_mergeableconst : 1;
        uint16_t is_metadata : 1;
        uint16_t is_readonly : 1;
        uint16_t is_readonlywithrel : 1;
        uint16_t is_text : 1;
        uint16_t is_threadbss : 1;
        uint16_t is_threadlocal : 1;
        uint16_t is_writeable : 1;
        uint16_t reserved : 2;
    };

    struct inst_desc
    {
        uint64_t canfoldasload : 1;
        uint64_t has_delayslot : 1;
        uint64_t has_optionaldef : 1;
        uint64_t has_unmodeledsideeffects : 1;
        uint64_t imp_licitdefs : 1;
        uint64_t imp_licituses : 1;
        uint64_t is_add : 1;
        uint64_t is_authenticated : 1;

        uint64_t is_barrier : 1;
        uint64_t is_bitcast : 1;
        uint64_t is_branch : 1;
        uint64_t is_call : 1;
        uint64_t is_compare : 1;
        uint64_t is_conditionalbranch : 1;
        uint64_t is_convergent : 1;
        uint64_t is_extractsubreglike : 1;

        uint64_t is_indirectbranch : 1;
        uint64_t is_insertsubreglike : 1;
        uint64_t is_moveimmediate : 1;
        uint64_t is_movereg : 1;
        uint64_t is_notduplicable : 1;
        uint64_t is_predicable : 1;
        uint64_t is_pseudo : 1;
        uint64_t is_regsequencelike : 1;

        uint64_t is_return : 1;
        uint64_t is_select : 1;
        uint64_t is_terminator : 1;
        uint64_t is_trap : 1;
        uint64_t is_unconditionalbranch : 1;
        uint64_t is_variadic : 1;
        uint64_t may_load : 1;
        uint64_t may_raisefpexception : 1;

        uint64_t may_store : 1;
        uint64_t numdefs : 8;
        uint64_t variadicopsaredefs : 1;

        uint64_t reserved : 21;
    };

    struct fixup
    {
        uint32_t flags;         // MCFixupKindInfo
        uint32_t name;
        uint32_t offset;
        uint32_t target_offset;
        uint32_t target_size;
    };

    struct fixup_expr
    {
        struct
        {
            uint32_t kind;
            uint32_t opcode;
        };

        struct
        {
            uint32_t kind;
            uint32_t format;
            uint64_t value;
        } lhs;

        struct
        {
            uint32_t kind;
            uint32_t format;
            uint64_t value;
        } rhs;
    };


    struct fixup_data
    {
        uint8_t data[1024];
    };

    struct section
    {
        uint32_t alignment;
        uint32_t characteristics;
        uint32_t section;
        uint32_t is_virtual;
        uint32_t name;

        struct
        {
            uint16_t is_bss : 1;
            uint16_t is_common : 1;
            uint16_t is_data : 1;
            uint16_t is_executeonly : 1;
            uint16_t is_globalwriteabledata : 1;
            uint16_t is_mergeablecstring : 1;
            uint16_t is_mergeableconst : 1;
            uint16_t is_metadata : 1;
            uint16_t is_readonly : 1;
            uint16_t is_readonlywithrel : 1;
            uint16_t is_text : 1;
            uint16_t is_threadbss : 1;
            uint16_t is_threadlocal : 1;
            uint16_t is_writeable : 1;
            uint16_t reserved : 2;
        } kind;
    };



    struct inst
    {
        uint32_t flags;
        uint32_t opcode;

        struct
        {
            uint64_t canfoldasload : 1;
            uint64_t has_delayslot : 1;
            uint64_t has_optionaldef : 1;
            uint64_t has_unmodeledsideeffects : 1;
            uint64_t imp_licitdefs : 1;
            uint64_t imp_licituses : 1;
            uint64_t is_add : 1;
            uint64_t is_authenticated : 1;

            uint64_t is_barrier : 1;
            uint64_t is_bitcast : 1;
            uint64_t is_branch : 1;
            uint64_t is_call : 1;
            uint64_t is_compare : 1;
            uint64_t is_conditionalbranch : 1;
            uint64_t is_convergent : 1;
            uint64_t is_extractsubreglike : 1;

            uint64_t is_indirectbranch : 1;
            uint64_t is_insertsubreglike : 1;
            uint64_t is_moveimmediate : 1;
            uint64_t is_movereg : 1;
            uint64_t is_notduplicable : 1;
            uint64_t is_predicable : 1;
            uint64_t is_pseudo : 1;
            uint64_t is_regsequencelike : 1;

            uint64_t is_return : 1;
            uint64_t is_select : 1;
            uint64_t is_terminator : 1;
            uint64_t is_trap : 1;
            uint64_t is_unconditionalbranch : 1;
            uint64_t is_variadic : 1;
            uint64_t may_load : 1;
            uint64_t may_raisefpexception : 1;

            uint64_t may_store : 1;
            uint64_t numdefs : 1;
            uint64_t variadicopsaredefs : 1;

            uint64_t reserved : 29;
        } desc;
    };

    struct inst_operands
    {
        uint32_t kind;
        uint64_t value;
    };


    struct inst_object
    {
        union
        {
            struct
            {
                struct
                {
                    uint32_t flags;         // MCFixupKindInfo
                    uint32_t name;
                    uint32_t offset;
                    uint32_t target_offset;
                    uint32_t target_size;

                    struct
                    {
                        uint32_t kind;

                        struct
                        {
                            uint32_t kind;
                            uint32_t format;
                            uint32_t value;
                        } lhs;

                        struct
                        {
                            uint32_t kind;
                            uint32_t format;
                            uint32_t value;
                        } rhs;
                    } value;
                } fixup;

                struct
                {
                    uint32_t flags;
                    uint32_t opcode;

                    struct
                    {
                        uint64_t canfoldasload : 1;
                        uint64_t has_delayslot : 1;
                        uint64_t has_optionaldef : 1;
                        uint64_t has_unmodeledsideeffects : 1;
                        uint64_t imp_licitdefs : 1;
                        uint64_t imp_licituses : 1;
                        uint64_t is_add : 1;
                        uint64_t is_authenticated : 1;

                        uint64_t is_barrier : 1;
                        uint64_t is_bitcast : 1;
                        uint64_t is_branch : 1;
                        uint64_t is_call : 1;
                        uint64_t is_compare : 1;
                        uint64_t is_conditionalbranch : 1;
                        uint64_t is_convergent : 1;
                        uint64_t is_extractsubreglike : 1;

                        uint64_t is_indirectbranch : 1;
                        uint64_t is_insertsubreglike : 1;
                        uint64_t is_moveimmediate : 1;
                        uint64_t is_movereg : 1;
                        uint64_t is_notduplicable : 1;
                        uint64_t is_predicable : 1;
                        uint64_t is_pseudo : 1;
                        uint64_t is_regsequencelike : 1;

                        uint64_t is_return : 1;
                        uint64_t is_select : 1;
                        uint64_t is_terminator : 1;
                        uint64_t is_trap : 1;
                        uint64_t is_unconditionalbranch : 1;
                        uint64_t is_variadic : 1;
                        uint64_t may_load : 1;
                        uint64_t may_raisefpexception : 1;

                        uint64_t may_store : 1;
                        uint64_t numdefs : 1;
                        uint64_t variadicopsaredefs : 1;

                        uint64_t reserved : 29;
                    } desc;
                } inst;
            } exper;

            struct
            {
                uint32_t kind;
                struct
                {
                    uint32_t kind;
                    uint32_t column;
                    uint32_t line;
                    uint8_t msg[1024];
                };
            };
        } diagnostic;
    };
#if defined(HAVE_CPLUSPLUS)
};
#endif
#endif // __CHEAT_STRUCT_H__