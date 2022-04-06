#include <cheat_parser.h>
#include <cheat_streamer.h>

namespace cheat
{
    MCParser::MCParser(MCSerializeObject* serialize,
                       const std::shared_ptr<llvm::Triple>& triple)
        : serialize(serialize)
        , triple(triple)
        , target(const_cast<llvm::Target*>(llvm::TargetRegistry::lookupTarget(triple->getTriple(), default_error)))
    {
        if (nullptr == target)
        {
            throw std::exception("the target platform you entered is incorrect please check");
        }

        if (nullptr != target && default_error.size())
        {
            throw std::exception(default_error.data());
        }
    }

    MCParser::~MCParser()
    {

    }

    bool MCParser::buildingObjects()
    {
        if (nullptr == triple || nullptr == target)
        {
            return false;
        }

        if (nullptr == register_info)
        {
            register_info.reset(target->createMCRegInfo(triple->getTriple()));
        }

        if (nullptr == asm_info)
        {
            asm_info.reset(target->createMCAsmInfo(*register_info, triple->getTriple(), options));
        }

        if (nullptr == instr_info)
        {
            instr_info.reset(target->createMCInstrInfo());
        }

        if (nullptr == subtarget_info)
        {
            subtarget_info.reset(target->createMCSubtargetInfo(triple->getTriple(), "", ""));
        }

        if (nullptr == asm_backend)
        {
            asm_backend.reset(target->createMCAsmBackend(*subtarget_info, *register_info, options));
        }

        return true;
    }

    bool MCParser::serializeParser(const std::string& puts,
                                   const MCGenCodeSyntax& syntax,
                                   const MCGenCodePretty& pretty)
    {
        if (buildingObjects())
        {
            std::unique_ptr<llvm::SourceMgr> source_mgr(new llvm::SourceMgr());
            {
                source_mgr->AddNewSourceBuffer(llvm::MemoryBuffer::getMemBuffer(puts), llvm::SMLoc());
                source_mgr->setDiagHandler(&MCParser::diagHandler, this);
            }

            std::shared_ptr<llvm::MCContext> context(new llvm::MCContext(*triple, asm_info.get(), register_info.get(), subtarget_info.get(), source_mgr.get(), &options));

            std::unique_ptr<llvm::MCObjectFileInfo> object_file_info(target->createMCObjectFileInfo(*context, false));
            {
                context->setObjectFileInfo(object_file_info.get());
            }

            std::shared_ptr<llvm::MCCodeEmitter> code_emitter(target->createMCCodeEmitter(*instr_info, *register_info, *context));

            std::unique_ptr<cheat::MCStreamer> exec_stream = std::make_unique<cheat::MCStreamer>(*triple, context, code_emitter, asm_backend, instr_info, register_info, serialize);
            {
                exec_stream->setUseAssemblerInfoForParsing(true);
            }

            std::unique_ptr<llvm::MCAsmParser> asm_parser(llvm::createMCAsmParser(*source_mgr, *context, *exec_stream, *asm_info));

            std::unique_ptr<llvm::MCTargetAsmParser> target_asm_parser(target->createMCAsmParser(*subtarget_info, *asm_parser, *instr_info, options));
            {
                asm_parser->setTargetParser(*target_asm_parser);
                target_asm_parser->getLexer().setMasmDefaultRadix(16);
            }

            switch (syntax)
            {
                case MCGenCodeSyntax::MC_SYNTAX_ATT:   asm_parser->setAssemblerDialect(MCGenCodeSyntax::MC_SYNTAX_ATT);   break;
                case MCGenCodeSyntax::MC_SYNTAX_INTEL:
                {
                    options.MCRelaxAll = true;
                    options.MCNoTypeCheck = true;
                    options.AssemblyLanguage = "masm";
                    asm_parser->setAssemblerDialect(MCGenCodeSyntax::MC_SYNTAX_INTEL);
                    break;
                }
                default: break;
            }

            if (asm_parser->Run(false))
            {
                return true;
            }

            return false;
        }

        return true;
    }

    void MCParser::diagHandler(const llvm::SMDiagnostic& diag, void *context)
    {
        if (nullptr == context)
        {
            return;
        }

        return reinterpret_cast<MCParser*>(context)->diagHandler(diag);
    }

    void MCParser::diagHandler(const llvm::SMDiagnostic& diag)
    {
        if (nullptr == serialize)
        {
            return;
        }

        serialize->push_back(MCSerializeObject{ MCSerializeObject { "kind", MCSerializeObject{ MCSerializeObject{ "name", "DiagHandler" },
                                                                                               MCSerializeObject{ "value", 0 }
                                                                                             } },
                                                MCSerializeObject { "msgs", applySerialize(diag) } }
        );
    }

    MCSerializeObject MCParser::applySerialize(const llvm::SMDiagnostic& diag) const
    {
        return MCSerializeObject{ MCSerializeObject { "kind", applySerialize(diag.getKind()) },
                                  MCSerializeObject { "message", diag.getMessage() },
                                  MCSerializeObject { "line", diag.getLineNo() },
                                  MCSerializeObject { "column", diag.getColumnNo() }
        };
    }

    MCSerializeObject MCParser::applySerialize(const llvm::SourceMgr::DiagKind& kind) const
    {
        switch (kind)
        {
            case llvm::SourceMgr::DK_Error:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Error" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::SourceMgr::DK_Error) }
                };
            }
            case llvm::SourceMgr::DK_Warning:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Warning" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::SourceMgr::DK_Warning) }
                };
            }
            case llvm::SourceMgr::DK_Remark:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Remark" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::SourceMgr::DK_Remark) }
                };
            }
            case llvm::SourceMgr::DK_Note:
            {
                return MCSerializeObject{ MCSerializeObject { "name", "Note" },
                                          MCSerializeObject { "value", static_cast<std::uint32_t>(llvm::SourceMgr::DK_Note) }
                };
            }
            default: break;
        }

        llvm_unreachable("unknown diagnostic kind");

        return MCSerializeObject{ MCSerializeObject { "name", "Unknown" },
                                  MCSerializeObject { "value", static_cast<std::uint32_t>(-1) }
        };
    }
}