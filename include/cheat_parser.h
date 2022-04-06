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

#ifndef __CHEAT_PARSER_H__
#define __CHEAT_PARSER_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <memory>

#include <cheat_platform.h>
#include <cheat_struct.h>

namespace cheat
{
    class MCParser
    {
    public:
        explicit MCParser(MCSerializeObject* serialize,
                          const std::shared_ptr<llvm::Triple>& triple);
        virtual ~MCParser();
    public:
        bool buildingObjects();
        // Run the parser on the input source buffer
        bool serializeParser(const std::string& puts,
                             const MCGenCodeSyntax& syntax,
                             const MCGenCodePretty& pretty);
    private:
        /// Clients that want to handle their own diagnostics in a custom way can
        /// register a function pointer+context as a diagnostic handler.
        /// It gets called each time PrintMessage is invoked.
        static void diagHandler(const llvm::SMDiagnostic& diag, void *context);
        void diagHandler(const llvm::SMDiagnostic& diag);
    private:
        MCSerializeObject applySerialize(const llvm::SMDiagnostic& diag) const;
        MCSerializeObject applySerialize(const llvm::SourceMgr::DiagKind& kind) const;
    private:
        MCSerializeObject*                          serialize;
        std::shared_ptr<llvm::Triple>               triple;
        llvm::Target*                               target;
        std::string                                 default_error;
        llvm::MCTargetOptions                       options;
        std::shared_ptr<llvm::MCRegisterInfo>       register_info;
        std::shared_ptr<llvm::MCAsmInfo>            asm_info;
        std::shared_ptr<llvm::MCInstrInfo>          instr_info;
        std::shared_ptr<llvm::MCSubtargetInfo>      subtarget_info;
        std::shared_ptr<llvm::MCAsmBackend>         asm_backend;
    };
}

#endif // __CHEAT_PARSER_H__