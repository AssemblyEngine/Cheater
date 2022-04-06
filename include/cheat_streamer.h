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

#ifndef __CHEAT_STREAMER_H__
#define __CHEAT_STREAMER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <cheat_platform.h>
#include <cheat_struct.h>

namespace cheat
{
    class MCStreamer : public llvm::MCStreamer
    {
    public:
        explicit MCStreamer(const llvm::Triple& target_triple,
                            const std::shared_ptr<llvm::MCContext> ctx,
                            const std::shared_ptr<llvm::MCCodeEmitter>& ce,
                            const std::shared_ptr<llvm::MCAsmBackend>& mab,
                            const std::shared_ptr<llvm::MCInstrInfo>& mcii,
                            const std::shared_ptr<llvm::MCRegisterInfo>& mri,
                            MCSerializeObject* serialize
        );
        virtual ~MCStreamer();
    private:
        virtual void emitLabel(llvm::MCSymbol* symbol,
                               llvm::SMLoc loc) override;

        virtual void emitSymbolDesc(llvm::MCSymbol* symbol,
                                    std::uint32_t desc) override;

        virtual bool emitSymbolAttribute(llvm::MCSymbol* symbol, llvm::MCSymbolAttr attr) override;

        virtual void emitCommonSymbol(llvm::MCSymbol* symbol,
                                      std::uint64_t size,
                                      std::uint32_t alignment) override;

        virtual void emitZerofill(llvm::MCSection* section, llvm::MCSymbol* symbol = nullptr,
                                  std::uint64_t size = 0, std::uint32_t alignment = 0,
                                  llvm::SMLoc loc = llvm::SMLoc()) override;

        virtual void changeSection(llvm::MCSection* section,
                                   const llvm::MCExpr* sub_section) override;

        virtual void emitBytes(llvm::StringRef data) override;

        virtual void emitValueImpl(const llvm::MCExpr* value,
                                   std::uint32_t size,
                                   llvm::SMLoc loc) override;

        virtual void emitInstruction(const llvm::MCInst& inst,
                                     const llvm::MCSubtargetInfo& sit) override;
    private:
        MCSerializeObject applySerialize(const llvm::MCFixup& fixup, const llvm::MCFixupKindInfo& kind_info) const;
        MCSerializeObject applySerialize(const llvm::MCFixupKindInfo& kind_info) const;
        MCSerializeObject applySerialize(const llvm::MCExpr* expr) const;
        MCSerializeObject applySerialize(const llvm::MCInst& inst) const;
        MCSerializeObject applySerialize(const llvm::MCOperand& op) const;
        MCSerializeObject applySerialize(const llvm::MCInstrDesc& desc) const;
        MCSerializeObject applySerialize(const llvm::MCSection* sect) const;
        MCSerializeObject applySerialize(const llvm::SectionKind& kind) const;
        MCSerializeObject applySerialize(const llvm::MCSymbol* symbol) const;
        MCSerializeObject applySerialize(const llvm::MCSymbolAttr& attr) const;
        MCSerializeObject applySerialize(const llvm::MCBinaryExpr::Opcode& op) const;
        MCSerializeObject applySerialize(const llvm::MCUnaryExpr::Opcode& op) const;

        MCSerializeObject applySerialize(const llvm::MCExpr* expr, const llvm::Triple::ArchType& kind) const;

        MCSerializeObject applySerialize(const llvm::AArch64MCExpr::VariantKind& kind) const;
        MCSerializeObject applySerialize(const llvm::MipsMCExpr::MipsExprKind& kind) const;
    private:
        llvm::Triple                                                        llvm_target_triple;
        std::shared_ptr<llvm::MCContext>                                    llvm_context;
        std::shared_ptr<llvm::MCCodeEmitter>                                llvm_code_emitter;
        std::shared_ptr<llvm::MCAsmBackend>                                 llvm_asm_backend;
        std::shared_ptr<llvm::MCInstrInfo>                                  llvm_instr_info;
        std::shared_ptr<llvm::MCRegisterInfo>                               llvm_register_info;
        MCSerializeObject*                                                  llvm_serialize;
    };
}

#endif // __CHEAT_STREAMER_H__