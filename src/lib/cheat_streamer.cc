#include <cheat_streamer.h>
#include <cheat_utils.h>
namespace cheat
{
    MCStreamer::MCStreamer(const llvm::Triple& target_triple,
                           const std::shared_ptr<llvm::MCContext> context,
                           const std::shared_ptr<llvm::MCCodeEmitter>& ce,
                           const std::shared_ptr<llvm::MCAsmBackend>& mab,
                           const std::shared_ptr<llvm::MCInstrInfo>& mcii,
                           const std::shared_ptr<llvm::MCRegisterInfo>& mri,
                           MCSerializeObject* serialize
    )
        : llvm::MCStreamer(*context)
        , llvm_target_triple(target_triple)
        , llvm_context(context)
        , llvm_code_emitter(ce)
        , llvm_asm_backend(mab)
        , llvm_instr_info(mcii)
        , llvm_register_info(mri)
        , llvm_serialize(serialize)
    {

    }

    MCStreamer::~MCStreamer()
    {

    }
    void MCStreamer::emitLabel(llvm::MCSymbol* symbol,
                               llvm::SMLoc loc)
    {
        if (nullptr == llvm_serialize)
        {
            return;
        }

        llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "label" },
                                                                                                    MCSerializeObject{ "value", MCObjectType::MC_LABEL } } },
                                                     MCSerializeObject { "symbol", applySerialize(symbol) } }
        );
    }

    void MCStreamer::emitSymbolDesc(llvm::MCSymbol* symbol,
                                    std::uint32_t desc)
    {
        if (nullptr == llvm_serialize)
        {
            return;
        }

        llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "desc" },
                                                                                                    MCSerializeObject{ "value", MCObjectType::MC_DESC } } },
                                                     MCSerializeObject { "value", desc },
                                                     MCSerializeObject { "symbol", applySerialize(symbol) } }
        );
    }

    bool MCStreamer::emitSymbolAttribute(llvm::MCSymbol* symbol, llvm::MCSymbolAttr attr)
    {
        if (nullptr == llvm_serialize)
        {
            return true;
        }

        llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "attribute" },
                                                                                                    MCSerializeObject{ "value", MCObjectType::MC_ATTRIBUTE } } },
                                                     MCSerializeObject { "attribute", applySerialize(attr) },
                                                     MCSerializeObject { "symbol", applySerialize(symbol) } }
        );

        return true;
    }

    void MCStreamer::emitCommonSymbol(llvm::MCSymbol* symbol,
                                      std::uint64_t size,
                                      std::uint32_t alignment)
    {
        if (nullptr == llvm_serialize)
        {
            return;
        }

        llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "common" },
                                                                                                    MCSerializeObject{ "value", MCObjectType::MC_COMMON } } },
                                                     MCSerializeObject { "size", size },
                                                     MCSerializeObject { "alignment", alignment },
                                                     MCSerializeObject { "symbol", applySerialize(symbol) } }
        );
    }

    void MCStreamer::emitZerofill(llvm::MCSection* section,
                                  llvm::MCSymbol* symbol,
                                  std::uint64_t size,
                                  std::uint32_t alignment,
                                  llvm::SMLoc loc)
    {
        if (nullptr == llvm_serialize)
        {
            return;
        }

        llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "zerofill" },
                                                                                                    MCSerializeObject{ "value", MCObjectType::MC_ZEROFILL } } },
                                                     MCSerializeObject { "size", size },
                                                     MCSerializeObject { "alignment", alignment },
                                                     MCSerializeObject { "section", applySerialize(section) },
                                                     MCSerializeObject { "symbol", applySerialize(symbol) } }
        );
    }

    void MCStreamer::changeSection(llvm::MCSection* section,
                                   const llvm::MCExpr* sub_section)
    {
        if (nullptr == section || nullptr == llvm_serialize)
        {
            return;
        }

        llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "changesection" },
                                                                                                    MCSerializeObject{ "value", MCObjectType::MC_CHANGESECTION } } },
                                                     MCSerializeObject { "section", applySerialize(section) },
                                                     MCSerializeObject { "subsection", applySerialize(sub_section) } }
        );
    }

    void MCStreamer::emitBytes(llvm::StringRef data)
    {
        if (nullptr == llvm_serialize)
        {
            return;
        }

        llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "bytes" },
                                                                                                    MCSerializeObject{ "value", MCObjectType::MC_BYTES } } },
                                                     MCSerializeObject { "data", std::move(std::vector<std::uint8_t>(reinterpret_cast<const std::uint8_t*>(data.data()), reinterpret_cast<const std::uint8_t*>(data.data()) + data.size())) } }
        );
    }

    void MCStreamer::emitValueImpl(const llvm::MCExpr* value,
                                   std::uint32_t size,
                                   llvm::SMLoc loc)
    {
        llvm::MCStreamer::emitValueImpl(value, size, loc);

        if (nullptr == llvm_serialize)
        {
            return;
        }

        llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "valueimpl" },
                                                                                                    MCSerializeObject{ "value", MCObjectType::MC_VALUEIMPL } } },
                                                     MCSerializeObject { "size", size },
                                                     MCSerializeObject { "value", applySerialize(value) } }
        );
    }

    void MCStreamer::emitInstruction(const llvm::MCInst& inst,
                                     const llvm::MCSubtargetInfo& sit)
    {
        if (nullptr == llvm_serialize)
        {
            return;
        }

        llvm::SmallVector<llvm::MCFixup, 4> fixups;
        llvm::SmallString<8> code; // assembly instruction max length 8 bytes
        llvm::raw_svector_ostream ostream(code);
        std::string binary;
        MCSerializeObject buffer;
        {
            if (nullptr == llvm_code_emitter || nullptr == llvm_asm_backend)
            {
                return;
            }
            else
            {
                llvm_code_emitter->encodeInstruction(inst, ostream, fixups, sit);
            }

            for (const llvm::MCFixup& fixup : fixups)
            {
                buffer.push_back(applySerialize(fixup, llvm_asm_backend->getFixupKindInfo(fixup.getKind())));
            }
        }

        if (BinaryToString(std::string(code.data(), code.size()), binary))
        {
            llvm_serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject { MCSerializeObject{ "name", "instruction" },
                                                                                                         MCSerializeObject{ "value", MCObjectType::MC_INSTRUCTION } } },
                                                         MCSerializeObject { "data", MCSerializeData(reinterpret_cast<std::uint8_t*>(code.data()), reinterpret_cast<std::uint8_t*>(code.data()) + code.size()) },
                                                         MCSerializeObject { "binary", binary },
                                                         MCSerializeObject { "fixups", std::move(buffer) },
                                                         MCSerializeObject { "inst", applySerialize(inst) } }
            );
        }
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCFixup& fixup, const llvm::MCFixupKindInfo& kind_info) const
    {
        return MCSerializeObject{ MCSerializeObject { "kind", applySerialize(kind_info) },
                                  MCSerializeObject { "info", MCSerializeObject { MCSerializeObject { "name", kind_info.Name },
                                                                                  MCSerializeObject { "target_offset", kind_info.TargetOffset },
                                                                                  MCSerializeObject { "target_size", kind_info.TargetSize } } },
                                  MCSerializeObject { "offset", fixup.getOffset() },
                                  MCSerializeObject { "value", applySerialize(fixup.getValue()) }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCFixupKindInfo& kind_info) const
    {
        if (kind_info.Flags & llvm::MCFixupKindInfo::FKF_IsPCRel)
        {
            return MCSerializeObject{ MCSerializeObject { "name", "IsPCRel" } ,
                                      MCSerializeObject { "value", kind_info.Flags }
            };
        }

        if (kind_info.Flags & llvm::MCFixupKindInfo::FKF_IsAlignedDownTo32Bits)
        {
            return MCSerializeObject{ MCSerializeObject { "name", "IsAlignedDownTo32Bits" } ,
                                      MCSerializeObject { "value", kind_info.Flags }
            };
        }

        if (kind_info.Flags & llvm::MCFixupKindInfo::FKF_IsTarget)
        {
            return MCSerializeObject{ MCSerializeObject { "name", "IsTarget" } ,
                                      MCSerializeObject { "value", kind_info.Flags }
            };
        }

        if (kind_info.Flags & llvm::MCFixupKindInfo::FKF_Constant)
        {
            return MCSerializeObject{ MCSerializeObject { "name", "Constant" } ,
                                      MCSerializeObject { "value", kind_info.Flags }
            };
        }

        return MCSerializeObject{ MCSerializeObject { "name", "Unknown" } ,
                                  MCSerializeObject { "value", static_cast<std::uint32_t>(-1)}
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCExpr* expr) const
    {
        if (nullptr == expr)
        {
            return MCSerializeObject{ };
        }

        switch (expr->getKind())
        {
            case llvm::MCExpr::Binary:
            {
                return MCSerializeObject{ MCSerializeObject { "kind",   MCSerializeObject { MCSerializeObject { "name", "Binary" },
                                                                                            MCSerializeObject { "value", llvm::MCExpr::Binary } } },
                                          MCSerializeObject { "opcode", applySerialize(llvm::dyn_cast<llvm::MCBinaryExpr>(expr)->getOpcode()) },
                                          MCSerializeObject { "lhs", applySerialize(llvm::dyn_cast<llvm::MCBinaryExpr>(expr)->getLHS()) },
                                          MCSerializeObject { "rhs", applySerialize(llvm::dyn_cast<llvm::MCBinaryExpr>(expr)->getRHS()) }
                };
            }
            case llvm::MCExpr::Constant:
            {
                return MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject { MCSerializeObject { "name", "Constant" },
                                                                                          MCSerializeObject { "value", llvm::MCExpr::Constant } } },
                                          MCSerializeObject { "value", static_cast<std::int64_t>(llvm::dyn_cast<llvm::MCConstantExpr>(expr)->getValue()) },
                                          MCSerializeObject { "format", static_cast<std::int32_t>(llvm::dyn_cast<llvm::MCConstantExpr>(expr)->useHexFormat()) }
                };
            }
            case llvm::MCExpr::SymbolRef:
            {
                return MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject { MCSerializeObject { "name", "SymbolRef" },
                                                                                          MCSerializeObject { "value", llvm::MCExpr::SymbolRef } } },
                                          MCSerializeObject { "variantKind", llvm::MCSymbolRefExpr::getVariantKindName(llvm::dyn_cast<llvm::MCSymbolRefExpr>(expr)->getKind()) },
                                          MCSerializeObject { "hasSubsectionsViaSymbols", llvm::dyn_cast<llvm::MCSymbolRefExpr>(expr)->hasSubsectionsViaSymbols() },
                                          MCSerializeObject { "symbol", applySerialize(std::addressof(llvm::dyn_cast<llvm::MCSymbolRefExpr>(expr)->getSymbol())) }
                };
            }
            case llvm::MCExpr::Unary:
            {
                return MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject { MCSerializeObject { "name", "Unary" },
                                                                                          MCSerializeObject { "value", llvm::MCExpr::Unary } } },
                                          MCSerializeObject { "opcode", applySerialize(llvm::dyn_cast<llvm::MCUnaryExpr>(expr)->getOpcode()) },
                                          MCSerializeObject { "hasSubsectionsViaSymbols", llvm::dyn_cast<llvm::MCSymbolRefExpr>(expr)->hasSubsectionsViaSymbols() },
                                          MCSerializeObject { "symbol", applySerialize(std::addressof(llvm::dyn_cast<llvm::MCSymbolRefExpr>(expr)->getSymbol())) }
                };
            }
            case llvm::MCExpr::Target:
            {
                return applySerialize(expr, llvm_target_triple.getArch());
            }
            default: break;
        }

        return MCSerializeObject{};
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCInst& inst) const
    {
        MCSerializeObject buffer;
        {
            for (const llvm::MCOperand& op : inst)
            {
                buffer.push_back(applySerialize(op));
            }
        }

        return MCSerializeObject{ MCSerializeObject { "opcode", MCSerializeObject { MCSerializeObject { "name", llvm_instr_info->getName(inst.getOpcode()) },
                                                                                    MCSerializeObject { "value", inst.getOpcode() } } },
                                  MCSerializeObject { "flags", inst.getFlags() },
                                  MCSerializeObject { "operands", std::move(buffer) },
                                  MCSerializeObject { "desc", applySerialize(llvm_instr_info->get(inst.getOpcode())) }


        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCOperand& op) const
    {
        if (op.isValid())
        {
            if (op.isReg())
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Reg" },
                                          MCSerializeObject { "value", llvm::MCRegister(op.getReg()).isValid() ? llvm_register_info->getName(op.getReg()) : "" }
                };
            }

            if (op.isImm())
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Imm" },
                                          MCSerializeObject { "value", op.getImm() }
                };
            }

            if (op.isSFPImm())
            {
                return MCSerializeObject{ MCSerializeObject { "name", "SFPImm" },
                                          MCSerializeObject { "value", llvm::bit_cast<float>(op.getSFPImm()) }
                };
            }

            if (op.isDFPImm())
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DFPImm" },
                                          MCSerializeObject { "value", llvm::bit_cast<double>(op.getDFPImm()) }
                };
            }

            if (op.isExpr())
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Expr" },
                                          MCSerializeObject { "value", applySerialize(op.getExpr()) }
                };
            }

            if (op.isInst())
            {

                return MCSerializeObject{ MCSerializeObject { "name", "Inst" },
                                          MCSerializeObject { "value", 0 }
                };
            }
        }

        return MCSerializeObject{ MCSerializeObject { "name", "Invalid" },
                                  MCSerializeObject { "value", 0 }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCInstrDesc& desc) const
    {
        MCSerializeObject buffer_user, buffer_defs;

        for (uint32_t i = 0; i < desc.getNumImplicitUses(); ++i)
        {
            buffer_user.push_back(MCSerializeObject{ MCSerializeObject { "name", llvm_register_info->getName(reinterpret_cast<const llvm::MCPhysReg*>(desc.getImplicitUses())[i]) },
                                                     MCSerializeObject { "value", static_cast<llvm::MCPhysReg>(reinterpret_cast<const llvm::MCPhysReg*>(desc.getImplicitUses())[i]) } }
            );
        }

        for (uint32_t i = 0; i < desc.getNumImplicitDefs(); ++i)
        {
            buffer_user.push_back(MCSerializeObject{ MCSerializeObject { "name", llvm_register_info->getName(reinterpret_cast<const llvm::MCPhysReg*>(desc.getImplicitDefs())[i]) },
                                                     MCSerializeObject { "value", static_cast<llvm::MCPhysReg>(reinterpret_cast<const llvm::MCPhysReg*>(desc.getImplicitDefs())[i]) } }
            );
        }

        return MCSerializeObject{ MCSerializeObject { "opcode", desc.Opcode },
                                  MCSerializeObject { "numoperands", desc.NumOperands },
                                  MCSerializeObject { "numdefs", desc.NumDefs },
                                  MCSerializeObject { "size", desc.Size },
                                  MCSerializeObject { "schedclass", desc.SchedClass },
                                  MCSerializeObject { "flags", desc.Flags },
                                  MCSerializeObject { "tsflags", desc.TSFlags },
                                  MCSerializeObject { "value", MCSerializeObject { MCSerializeObject { "implicituses",             std::move(buffer_user) },
                                                                                   MCSerializeObject { "implicitdefs",             std::move(buffer_defs) },
                                                                                   MCSerializeObject { "numdefs",                  static_cast<std::uint32_t>(desc.getNumDefs()) },
                                                                                   MCSerializeObject { "is_variadic",              static_cast<std::uint32_t>(desc.isVariadic()) },
                                                                                   MCSerializeObject { "has_optionaldef",          static_cast<std::uint32_t>(desc.hasOptionalDef()) },
                                                                                   MCSerializeObject { "is_pseudo",                static_cast<std::uint32_t>(desc.isPseudo()) },
                                                                                   MCSerializeObject { "is_return",                static_cast<std::uint32_t>(desc.isReturn()) },
                                                                                   MCSerializeObject { "is_add",                   static_cast<std::uint32_t>(desc.isAdd()) },
                                                                                   MCSerializeObject { "is_trap",                  static_cast<std::uint32_t>(desc.isTrap()) },
                                                                                   MCSerializeObject { "is_movereg",               static_cast<std::uint32_t>(desc.isMoveReg()) },
                                                                                   MCSerializeObject { "is_call",                  static_cast<std::uint32_t>(desc.isCall()) },
                                                                                   MCSerializeObject { "is_barrier",               static_cast<std::uint32_t>(desc.isBarrier()) },
                                                                                   MCSerializeObject { "is_terminator",            static_cast<std::uint32_t>(desc.isTerminator()) },
                                                                                   MCSerializeObject { "is_branch",                static_cast<std::uint32_t>(desc.isBranch()) },
                                                                                   MCSerializeObject { "is_indirectbranch",        static_cast<std::uint32_t>(desc.isIndirectBranch()) },
                                                                                   MCSerializeObject { "is_conditionalbranch",     static_cast<std::uint32_t>(desc.isConditionalBranch()) },
                                                                                   MCSerializeObject { "is_unconditionalbranch",   static_cast<std::uint32_t>(desc.isUnconditionalBranch()) },
                                                                                   MCSerializeObject { "is_predicable",            static_cast<std::uint32_t>(desc.isPredicable()) },
                                                                                   MCSerializeObject { "is_compare",               static_cast<std::uint32_t>(desc.isCompare()) },
                                                                                   MCSerializeObject { "is_move_immediate",        static_cast<std::uint32_t>(desc.isMoveImmediate()) },
                                                                                   MCSerializeObject { "is_bitcast",               static_cast<std::uint32_t>(desc.isBitcast()) },
                                                                                   MCSerializeObject { "is_select",                static_cast<std::uint32_t>(desc.isSelect()) },
                                                                                   MCSerializeObject { "is_notduplicable",         static_cast<std::uint32_t>(desc.isNotDuplicable()) },
                                                                                   MCSerializeObject { "has_delayslot",            static_cast<std::uint32_t>(desc.hasDelaySlot()) },
                                                                                   MCSerializeObject { "can_foldAsLoad",           static_cast<std::uint32_t>(desc.canFoldAsLoad()) },
                                                                                   MCSerializeObject { "is_regsequencelike",       static_cast<std::uint32_t>(desc.isRegSequenceLike()) },
                                                                                   MCSerializeObject { "is_extractsubreglike",     static_cast<std::uint32_t>(desc.isExtractSubregLike()) },
                                                                                   MCSerializeObject { "is_insertsubreglike",      static_cast<std::uint32_t>(desc.isInsertSubregLike()) },
                                                                                   MCSerializeObject { "is_convergent",            static_cast<std::uint32_t>(desc.isConvergent()) },
                                                                                   MCSerializeObject { "variadicopsaredefs",       static_cast<std::uint32_t>(desc.variadicOpsAreDefs()) },
                                                                                   MCSerializeObject { "is_authenticated",         static_cast<std::uint32_t>(desc.isAuthenticated()) },
                                                                                   MCSerializeObject { "may_load",                 static_cast<std::uint32_t>(desc.mayLoad()) },
                                                                                   MCSerializeObject { "may_store",                static_cast<std::uint32_t>(desc.mayStore()) },
                                                                                   MCSerializeObject { "may_raisefpexception",     static_cast<std::uint32_t>(desc.mayRaiseFPException()) },
                                                                                   MCSerializeObject { "has_unmodeledsideeffects", static_cast<std::uint32_t>(desc.hasUnmodeledSideEffects()) } } }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCSection* sect) const
    {
        switch (sect->getVariant())
        {
            case llvm::MCSection::SV_COFF:
            {
                return MCSerializeObject{ MCSerializeObject { "kind", applySerialize(sect->getKind()) },
                                          MCSerializeObject { "class", "MCSectionCOFF" },
                                          MCSerializeObject { "name", llvm::dyn_cast<llvm::MCSectionCOFF>(sect)->getName() },
                                          MCSerializeObject { "variant", llvm::dyn_cast<llvm::MCSectionCOFF>(sect)->getVariant() },
                                          MCSerializeObject { "alignment", llvm::dyn_cast<llvm::MCSectionCOFF>(sect)->getAlignment() },
                                          MCSerializeObject { "characteristics", llvm::dyn_cast<llvm::MCSectionCOFF>(sect)->getCharacteristics() }

                };
            }
            case llvm::MCSection::SV_ELF:
            {
                return MCSerializeObject{ MCSerializeObject { "kind", applySerialize(sect->getKind()) },
                                          MCSerializeObject { "class", "MCSectionELF" },
                                          MCSerializeObject { "name", llvm::dyn_cast<llvm::MCSectionELF>(sect)->getName() },
                                          MCSerializeObject { "variant", llvm::dyn_cast<llvm::MCSectionELF>(sect)->getVariant() },
                                          MCSerializeObject { "alignment", llvm::dyn_cast<llvm::MCSectionELF>(sect)->getAlignment() },
                                          MCSerializeObject { "flags", llvm::dyn_cast<llvm::MCSectionELF>(sect)->getFlags() },
                                          MCSerializeObject { "entry_size", llvm::dyn_cast<llvm::MCSectionELF>(sect)->getEntrySize() },
                                          MCSerializeObject { "type", llvm::dyn_cast<llvm::MCSectionELF>(sect)->getType() }
                };
            }
            case llvm::MCSection::SV_MachO:
            {
                return MCSerializeObject{ MCSerializeObject { "kind", applySerialize(sect->getKind()) },
                                          MCSerializeObject { "class", "MCSectionMachO" },
                                          MCSerializeObject { "name", llvm::dyn_cast<llvm::MCSectionMachO>(sect)->getName() },
                                          MCSerializeObject { "variant", llvm::dyn_cast<llvm::MCSectionMachO>(sect)->getVariant() },
                                          MCSerializeObject { "alignment", llvm::dyn_cast<llvm::MCSectionMachO>(sect)->getAlignment() },
                                          MCSerializeObject { "type", llvm::dyn_cast<llvm::MCSectionMachO>(sect)->getType() },
                                          MCSerializeObject { "type_and_attributes", llvm::dyn_cast<llvm::MCSectionMachO>(sect)->getTypeAndAttributes() },
                                          MCSerializeObject { "segment_name", llvm::dyn_cast<llvm::MCSectionMachO>(sect)->getSegmentName() }
                };
            }
            default: break;
        }

        return MCSerializeObject{};
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::SectionKind& kind) const
    {
        return MCSerializeObject{ MCSerializeObject { "is_metadata",            static_cast<std::uint32_t>(kind.isMetadata()) },
                                  MCSerializeObject { "is_text",                static_cast<std::uint32_t>(kind.isText()) },
                                  MCSerializeObject { "is_executeonly",         static_cast<std::uint32_t>(kind.isExecuteOnly()) },
                                  MCSerializeObject { "is_readonly",            static_cast<std::uint32_t>(kind.isReadOnly()) },
                                  MCSerializeObject { "is_mergeablecstring",    static_cast<std::uint32_t>(kind.isMergeableCString()) },
                                  MCSerializeObject { "is_mergeableconst",      static_cast<std::uint32_t>(kind.isMergeableConst()) },
                                  MCSerializeObject { "is_writeable",           static_cast<std::uint32_t>(kind.isWriteable()) },
                                  MCSerializeObject { "is_threadlocal",         static_cast<std::uint32_t>(kind.isThreadLocal()) },
                                  MCSerializeObject { "is_threadbss",           static_cast<std::uint32_t>(kind.isThreadBSS()) },
                                  MCSerializeObject { "is_globalwriteabledata", static_cast<std::uint32_t>(kind.isGlobalWriteableData()) },
                                  MCSerializeObject { "is_bss",                 static_cast<std::uint32_t>(kind.isBSS()) },
                                  MCSerializeObject { "is_common",              static_cast<std::uint32_t>(kind.isCommon()) },
                                  MCSerializeObject { "is_data",                static_cast<std::uint32_t>(kind.isData()) },
                                  MCSerializeObject { "is_readonlywithrel",     static_cast<std::uint32_t>(kind.isReadOnlyWithRel()) }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCSymbol* symbol) const
    {
        if (nullptr == symbol)
        {
            return MCSerializeObject{};
        }

        return MCSerializeObject{ MCSerializeObject { "name",             symbol->getName() },
                                  MCSerializeObject { "is_variable",      static_cast<std::uint32_t>(symbol->isVariable()) },
                                  MCSerializeObject { "is_unset",         static_cast<std::uint32_t>(symbol->isUnset()) },
                                  MCSerializeObject { "is_common",        static_cast<std::uint32_t>(symbol->isCommon()) },
                                  MCSerializeObject { "is_targetcommon",  static_cast<std::uint32_t>(symbol->isTargetCommon()) },
                                  MCSerializeObject { "is_registered",    static_cast<std::uint32_t>(symbol->isRegistered()) },
                                  MCSerializeObject { "is_usedinreloc",   static_cast<std::uint32_t>(symbol->isUsedInReloc()) },
                                  MCSerializeObject { "is_temporary",     static_cast<std::uint32_t>(symbol->isTemporary()) },
                                  MCSerializeObject { "is_used",          static_cast<std::uint32_t>(symbol->isUsed()) },
                                  MCSerializeObject { "is_redefinable",   static_cast<std::uint32_t>(symbol->isRedefinable()) },
                                  MCSerializeObject { "is_defined",       static_cast<std::uint32_t>(symbol->isDefined()) },
                                  MCSerializeObject { "is_insection",     static_cast<std::uint32_t>(symbol->isInSection()) },
                                  MCSerializeObject { "is_absolute",      static_cast<std::uint32_t>(symbol->isAbsolute()) },
                                  MCSerializeObject { "is_defined",       static_cast<std::uint32_t>(symbol->isDefined()) },
                                  MCSerializeObject { "is_external",      static_cast<std::uint32_t>(symbol->isDefined()) },
                                  MCSerializeObject { "is_privateextern", static_cast<std::uint32_t>(symbol->isDefined()) }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCSymbolAttr& attr) const
    {
        switch (attr)
        {
            case llvm::MCSymbolAttr::MCSA_Invalid:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Invalid" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Invalid) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Cold:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Cold" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Cold) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_ELF_TypeFunction:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ELF_TypeFunction" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_ELF_TypeFunction) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_ELF_TypeIndFunction:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ELF_TypeIndFunction" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_ELF_TypeIndFunction) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_ELF_TypeObject:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TypeObject" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_ELF_TypeObject) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_ELF_TypeTLS:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ELF_TypeTLS" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_ELF_TypeTLS) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_ELF_TypeCommon:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ELF_TypeCommon" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_ELF_TypeCommon) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_ELF_TypeNoType:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ELF_TypeNoType" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_ELF_TypeNoType) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_ELF_TypeGnuUniqueObject:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ELF_TypeGnuUniqueObject" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_ELF_TypeGnuUniqueObject) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Global:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Global" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Global) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_LGlobal:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LGlobal" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_LGlobal) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Extern:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Extern" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Extern) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Hidden:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Hidden" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Hidden) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_IndirectSymbol:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "IndirectSymbol" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_IndirectSymbol) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Internal:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Internal" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Internal) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_LazyReference:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LazyReference" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_LazyReference) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Local:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Local" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Local) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_NoDeadStrip:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "NoDeadStrip" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_NoDeadStrip) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_SymbolResolver:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "SymbolResolver" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_SymbolResolver) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_AltEntry:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "AltEntry" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_AltEntry) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_PrivateExtern:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PrivateExtern" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_PrivateExtern) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Protected:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Protected" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Protected) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Reference:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Reference" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Reference) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_Weak:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Weak" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_Weak) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_WeakDefinition:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "WeakDefinition" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_WeakDefinition) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_WeakReference:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "WeakReference" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_WeakReference) }
                };
            }
            case llvm::MCSymbolAttr::MCSA_WeakDefAutoPrivate:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "WeakDefAutoPrivate" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCSymbolAttr::MCSA_WeakDefAutoPrivate) }
                };
            }
            default: break;
        }

        return MCSerializeObject{ MCSerializeObject { "name", "Unknown" },
                                  MCSerializeObject { "value", static_cast<std::uint32_t>(-1) }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCBinaryExpr::Opcode& op) const
    {
        switch (op)
        {
            case llvm::MCBinaryExpr::Add:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Add" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::Add) }
                };
            }
            case llvm::MCBinaryExpr::And:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "And" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::And) }
                };
            }
            case llvm::MCBinaryExpr::Div:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Div" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::Div) }
                };
            }
            case llvm::MCBinaryExpr::EQ:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "EQ" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::EQ) }
                };
            }
            case llvm::MCBinaryExpr::GT:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GT" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::GT) }
                };
            }
            case llvm::MCBinaryExpr::GTE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GTE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::GTE) }
                };
            }
            case llvm::MCBinaryExpr::LAnd:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LAnd" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::LAnd) }
                };
            }
            case llvm::MCBinaryExpr::LOr:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LOr" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::LOr) }
                };
            }
            case llvm::MCBinaryExpr::LT:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LT" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::LT) }
                };
            }
            case llvm::MCBinaryExpr::LTE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LTE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::LTE) }
                };
            }
            case llvm::MCBinaryExpr::Mod:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Mod" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::Mod) }
                };
            }
            case llvm::MCBinaryExpr::Mul:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Mul" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::Mul) }
                };
            }
            case llvm::MCBinaryExpr::NE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "NE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::NE) }
                };
            }
            case llvm::MCBinaryExpr::Or:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Or" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::Or) }
                };
            }
            case llvm::MCBinaryExpr::OrNot:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "OrNot" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::OrNot) }
                };
            }
            case llvm::MCBinaryExpr::Shl:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Shl" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::Shl) }
                };
            }
            case llvm::MCBinaryExpr::AShr:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "AShr" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::AShr) }
                };
            }
            case llvm::MCBinaryExpr::LShr:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LShr" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::LShr) }
                };
            }
            case llvm::MCBinaryExpr::Sub:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Sub" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::Sub) }
                };
            }
            case llvm::MCBinaryExpr::Xor:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Xor" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCBinaryExpr::Xor) }
                };
            }
            default: break;
        }

        return MCSerializeObject{ MCSerializeObject { "name", "Unknown" },
                                  MCSerializeObject { "value", static_cast<std::uint32_t>(-1) }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCUnaryExpr::Opcode& op) const
    {
        switch (op)
        {
            case llvm::MCUnaryExpr::LNot:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LNot" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCUnaryExpr::LNot) }
                };
            }
            case llvm::MCUnaryExpr::Minus:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Minus" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCUnaryExpr::Minus) }
                };
            }
            case llvm::MCUnaryExpr::Not:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Not" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCUnaryExpr::Not) }
                };
            }
            case llvm::MCUnaryExpr::Plus:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Plus" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MCUnaryExpr::Plus) }
                };
            }
            default: break;
        }

        return MCSerializeObject{ MCSerializeObject { "name", "Unknown" },
                          MCSerializeObject { "value", static_cast<std::uint32_t>(-1) }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MCExpr* expr, const llvm::Triple::ArchType& kind) const
    {
        switch (kind)
        {
            case llvm::Triple::UnknownArch:
            case llvm::Triple::arm:
            case llvm::Triple::armeb: break;
            case llvm::Triple::aarch64:
            case llvm::Triple::aarch64_be:
            case llvm::Triple::aarch64_32:
            {
                return MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject { MCSerializeObject { "name", "TargetExpr" },
                                                                                          MCSerializeObject { "value", static_cast<std::uint32_t>(kind) }}},
                                          MCSerializeObject { "target", "aarch64" },
                                          MCSerializeObject { "expr", applySerialize(llvm::dyn_cast<llvm::AArch64MCExpr>(expr)->getSubExpr()) },
                                          MCSerializeObject { "expr_kind", applySerialize(llvm::dyn_cast<llvm::AArch64MCExpr>(expr)->getKind()) },
                                          MCSerializeObject { "elf_name", llvm::dyn_cast<llvm::AArch64MCExpr>(expr)->getVariantKindName() }
                };
            }
            case llvm::Triple::arc:
            case llvm::Triple::avr:
            case llvm::Triple::bpfel:
            case llvm::Triple::bpfeb:
            case llvm::Triple::csky:
            case llvm::Triple::hexagon:
            case llvm::Triple::m68k: break;
            case llvm::Triple::mips:
            case llvm::Triple::mipsel:
            case llvm::Triple::mips64:
            case llvm::Triple::mips64el:
            {
                return MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject { MCSerializeObject { "name", "TargetExpr" },
                                                                                          MCSerializeObject { "value", static_cast<std::uint32_t>(kind) } } },
                                          MCSerializeObject { "target", "mips" },
                                          MCSerializeObject { "expr", applySerialize(llvm::dyn_cast<llvm::MipsMCExpr>(expr)->getSubExpr()) },
                                          MCSerializeObject { "expr_kind", applySerialize(llvm::dyn_cast<llvm::MipsMCExpr>(expr)->getKind()) }
                };
            }
            case llvm::Triple::msp430:
            case llvm::Triple::ppc:
            case llvm::Triple::ppcle:
            case llvm::Triple::ppc64:
            case llvm::Triple::ppc64le:
            case llvm::Triple::r600:
            case llvm::Triple::amdgcn:
            case llvm::Triple::riscv32:
            case llvm::Triple::riscv64:
            case llvm::Triple::sparc:
            case llvm::Triple::sparcv9:
            case llvm::Triple::sparcel:
            case llvm::Triple::systemz:
            case llvm::Triple::tce:
            case llvm::Triple::tcele:
            case llvm::Triple::thumb:
            case llvm::Triple::thumbeb:
            case llvm::Triple::x86:
            case llvm::Triple::x86_64:
            case llvm::Triple::xcore:
            case llvm::Triple::nvptx:
            case llvm::Triple::nvptx64:
            case llvm::Triple::le32:
            case llvm::Triple::le64:
            case llvm::Triple::amdil:
            case llvm::Triple::amdil64:
            case llvm::Triple::hsail:
            case llvm::Triple::hsail64:
            case llvm::Triple::spir:
            case llvm::Triple::spir64:
            case llvm::Triple::spirv32:
            case llvm::Triple::spirv64:
            case llvm::Triple::kalimba:
            case llvm::Triple::shave:
            case llvm::Triple::lanai:
            case llvm::Triple::wasm32:
            case llvm::Triple::wasm64:
            case llvm::Triple::renderscript32:
            case llvm::Triple::renderscript64:
            case llvm::Triple::ve: break;
            default: break;
        }

        return MCSerializeObject{};
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::AArch64MCExpr::VariantKind& kind) const
    {
        switch (kind)
        {
            case llvm::AArch64MCExpr::VK_ABS:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS) }
                };
            }
            case llvm::AArch64MCExpr::VK_SABS:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "SABS" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_SABS) }
                };
            }
            case llvm::AArch64MCExpr::VK_PREL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PREL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PREL) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOT:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOT) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOTTPREL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOTTPREL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOTTPREL) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL) }
                };
            }
            case llvm::AArch64MCExpr::VK_TLSDESC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TLSDESC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TLSDESC) }
                };
            }
            case llvm::AArch64MCExpr::VK_SECREL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "SECREL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_SECREL) }
                };
            }
            case llvm::AArch64MCExpr::VK_SymLocBits:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "SymLocBits" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_SymLocBits) }
                };
            }
            case llvm::AArch64MCExpr::VK_PAGE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PAGE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PAGE) }
                };
            }
            case llvm::AArch64MCExpr::VK_PAGEOFF:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PAGEOFF" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PAGEOFF) }
                };
            }
            case llvm::AArch64MCExpr::VK_HI12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "HI12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_HI12) }
                };
            }
            case llvm::AArch64MCExpr::VK_G0:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "G0" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_G0) }
                };
            }
            case llvm::AArch64MCExpr::VK_G1:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "G1" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_G1) }
                };
            }
            case llvm::AArch64MCExpr::VK_G2:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "G2" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_G2) }
                };
            }
            case llvm::AArch64MCExpr::VK_G3:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "G3" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_G3) }
                };
            }
            case llvm::AArch64MCExpr::VK_AddressFragBits:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "AddressFragBits" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_AddressFragBits) }
                };
            }
            case llvm::AArch64MCExpr::VK_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_PAGE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_PAGE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_PAGE) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_PAGE_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_PAGE_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_PAGE_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G3:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G3" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G3) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G2:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G2" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G2) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G2_S:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G2_S" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G2_S) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G2_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G2_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G2_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G1:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G1" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G1) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G1_S:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G1_S" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G1_S) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G1_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G1_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G1_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G0:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G0" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G0) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G0_S:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G0_S" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G0_S) }
                };
            }
            case llvm::AArch64MCExpr::VK_ABS_G0_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "ABS_G0_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_ABS_G0_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_LO12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LO12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_LO12) }
                };
            }
            case llvm::AArch64MCExpr::VK_PREL_G3:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PREL_G3" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PREL_G3) }
                };
            }
            case llvm::AArch64MCExpr::VK_PREL_G2:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PREL_G2" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PREL_G2) }
                };
            }
            case llvm::AArch64MCExpr::VK_PREL_G2_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PREL_G2_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PREL_G2_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_PREL_G1:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PREL_G1" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PREL_G1) }
                };
            }
            case llvm::AArch64MCExpr::VK_PREL_G1_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PREL_G1_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PREL_G1_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_PREL_G0:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PREL_G0" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PREL_G0) }
                };
            }
            case llvm::AArch64MCExpr::VK_PREL_G0_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PREL_G0_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_PREL_G0_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOT_LO12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_LO12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOT_LO12) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOT_PAGE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_PAGE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOT_PAGE) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOT_PAGE_LO15:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_PAGE_LO15" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOT_PAGE_LO15) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL_G2:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_G2" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL_G2) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL_G1:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_G1" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL_G1) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL_G1_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_G1_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL_G1_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL_G0:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_G0" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL_G0) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL_G0_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_G0_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL_G0_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL_HI12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_HI12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL_HI12) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL_LO12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_LO12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL_LO12) }
                };
            }
            case llvm::AArch64MCExpr::VK_DTPREL_LO12_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_LO12_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_DTPREL_LO12_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOTTPREL_PAGE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOTTPREL_PAGE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOTTPREL_PAGE) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOTTPREL_LO12_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOTTPREL_LO12_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOTTPREL_LO12_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOTTPREL_G1:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOTTPREL_G1" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOTTPREL_G1) }
                };
            }
            case llvm::AArch64MCExpr::VK_GOTTPREL_G0_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOTTPREL_G0_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_GOTTPREL_G0_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL_G2:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_G2" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL_G2) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL_G1:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_G1" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL_G1) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL_G1_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_G1_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL_G1_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL_G0:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_G0" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL_G0) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL_G0_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_G0_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL_G0_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL_HI12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_HI12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL_HI12) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL_LO12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_LO12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL_LO12) }
                };
            }
            case llvm::AArch64MCExpr::VK_TPREL_LO12_NC:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_LO12_NC" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TPREL_LO12_NC) }
                };
            }
            case llvm::AArch64MCExpr::VK_TLSDESC_LO12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TLSDESC_LO12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TLSDESC_LO12) }
                };
            }
            case llvm::AArch64MCExpr::VK_TLSDESC_PAGE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TLSDESC_PAGE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_TLSDESC_PAGE) }
                };
            }
            case llvm::AArch64MCExpr::VK_SECREL_LO12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "SECREL_LO12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_SECREL_LO12) }
                };
            }
            case llvm::AArch64MCExpr::VK_SECREL_HI12:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "SECREL_HI12" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_SECREL_HI12) }
                };
            }
            case llvm::AArch64MCExpr::VK_INVALID:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "INVALID" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::AArch64MCExpr::VK_INVALID) }
                };
            }
            default: break;
        }

        return MCSerializeObject{ MCSerializeObject { "name", "unknown" },
                                  MCSerializeObject { "value", static_cast<std::uint32_t>(-1) }
        };
    }

    MCSerializeObject MCStreamer::applySerialize(const llvm::MipsMCExpr::MipsExprKind& kind) const
    {
        switch (kind)
        {
            case llvm::MipsMCExpr::MEK_None:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "None" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_None) }
                };
            }
            case llvm::MipsMCExpr::MEK_CALL_HI16:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "CALL_HI16" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_CALL_HI16) }
                };
            }
            case llvm::MipsMCExpr::MEK_CALL_LO16:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "CALL_LO16" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_CALL_LO16) }
                };
            }
            case llvm::MipsMCExpr::MEK_DTPREL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_DTPREL) }
                };
            }
            case llvm::MipsMCExpr::MEK_DTPREL_HI:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_HI" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_DTPREL_HI) }
                };
            }
            case llvm::MipsMCExpr::MEK_DTPREL_LO:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "DTPREL_LO" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_DTPREL_LO) }
                };
            }
            case llvm::MipsMCExpr::MEK_GOT:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GOT) }
                };
            }
            case llvm::MipsMCExpr::MEK_GOTTPREL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOTTPREL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GOTTPREL) }
                };
            }
            case llvm::MipsMCExpr::MEK_GOT_CALL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_CALL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GOT_CALL) }
                };
            }
            case llvm::MipsMCExpr::MEK_GOT_DISP:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_DISP" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GOT_DISP) }
                };
            }
            case llvm::MipsMCExpr::MEK_GOT_HI16:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_HI16" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GOT_HI16) }
                };
            }
            case llvm::MipsMCExpr::MEK_GOT_LO16:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_LO16" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GOT_LO16) }
                };
            }
            case llvm::MipsMCExpr::MEK_GOT_OFST:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_OFST" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GOT_OFST) }
                };
            }
            case llvm::MipsMCExpr::MEK_GOT_PAGE:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GOT_PAGE" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GOT_PAGE) }
                };
            }
            case llvm::MipsMCExpr::MEK_GPREL:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "GPREL" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_GPREL) }
                };
            }
            case llvm::MipsMCExpr::MEK_HI:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "HI" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_HI) }
                };
            }
            case llvm::MipsMCExpr::MEK_HIGHER:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "HIGHER" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_HIGHER) }
                };
            }
            case llvm::MipsMCExpr::MEK_HIGHEST:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "HIGHEST" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_HIGHEST) }
                };
            }
            case llvm::MipsMCExpr::MEK_LO:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "LO" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_LO) }
                };
            }
            case llvm::MipsMCExpr::MEK_NEG:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "NEG" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_NEG) }
                };
            }
            case llvm::MipsMCExpr::MEK_PCREL_HI16:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PCREL_HI16" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_PCREL_HI16) }
                };
            }
            case llvm::MipsMCExpr::MEK_PCREL_LO16:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "PCREL_LO16" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_PCREL_LO16) }
                };
            }
            case llvm::MipsMCExpr::MEK_TLSGD:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TLSGD" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_TLSGD) }
                };
            }
            case llvm::MipsMCExpr::MEK_TLSLDM:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TLSLDM" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_TLSLDM) }
                };
            }
            case llvm::MipsMCExpr::MEK_TPREL_HI:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_HI" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_TPREL_HI) }
                };
            }
            case llvm::MipsMCExpr::MEK_TPREL_LO:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "TPREL_LO" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_TPREL_LO) }
                };
            }
            case llvm::MipsMCExpr::MEK_Special:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Special" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::MipsMCExpr::MEK_Special) }
                };
            }
            default: break;
        }

        return MCSerializeObject{ MCSerializeObject { "name", "Unknown" },
                                  MCSerializeObject { "value", static_cast<std::uint32_t>(-1) }
        };
    }
}