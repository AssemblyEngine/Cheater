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

#ifndef __CHEAT_INITIAL_H__
#define __CHEAT_INITIAL_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <cheat_platform.h>

namespace cheat
{
    class MCInitialize
    {
    public:
        explicit MCInitialize()
        {
            llvm::InitializeAllAsmParsers();
            llvm::InitializeAllTargetInfos();
            llvm::InitializeAllTargetMCs();

            printf(R"(LLVM_DEFAULT_TARGET_TRIPLE: %s
LLVM_HOST_TRIPLE: %s
LLVM_NATIVE_ARCH: %s
LLVM_VERSION_STRING: %s
LLVM_VERSION_MAJOR: %d
LLVM_VERSION_MINOR: %d
LLVM_VERSION_PATCH: %d
)", LLVM_DEFAULT_TARGET_TRIPLE, 
LLVM_HOST_TRIPLE, 
FORCESPLICING(LLVM_NATIVE_ARCH),
LLVM_VERSION_STRING,
LLVM_VERSION_MAJOR,
LLVM_VERSION_MINOR,
LLVM_VERSION_PATCH
);
        }

        virtual ~MCInitialize()
        {
            
        }
    };
}

#endif // __CHEAT_INITIAL_H__