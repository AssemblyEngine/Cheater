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

#ifndef __CHEAT_GENCODE_H__
#define __CHEAT_GENCODE_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <memory>

#include <cheat_platform.h>
#include <cheat_struct.h>
#include <cheat_initial.h>

namespace cheat
{
    class MCParser;
    class MCGenCode : public MCInitialize
    {
    public:
        explicit MCGenCode();
        explicit MCGenCode(const std::string& target_triple);
        virtual ~MCGenCode();
    public:
        MCGenCodeError setOption(MCGenCodeOption op, std::size_t value);
    public:
        bool serializeParser(const std::string& puts,
                             std::uint64_t runtime = 0x400000,
                             const std::string& arch_name = llvm::sys::getDefaultTargetTriple(),
                             uint32_t clear = 1);
        std::string getDump() const { return serialize_context.dump(); }
        const MCSerializeData& getData() const { return parser_data; }
        const std::string& getDataString() const { return parser_data_string; }
        const std::string& getFixDataString() const { return parser_fix_data_string; }
        void clearData() { parser_data.clear(); parser_data_string.clear(); parser_fix_data_string.clear(); };
    private:
        void applyDeserializer(const MCSerializeObject& insts, std::uint64_t runtime);
        void applyDeserializer(const MCSerializeObject& fixups, const MCSerializeData& data, const std::string& binary, uint64_t runtime);
        void applyDeserializer(const MCSerializeData& data, const std::string& binary, std::uint64_t runtime);
    private:
        void appendData(const MCSerializeData::value_type& chars) { parser_data.push_back(chars); };
        void appendData(const MCSerializeData& chars) { parser_data.insert(parser_data.end(), chars.begin(), chars.end()); };
    private:
        std::string                             default_triple;
        std::shared_ptr<llvm::Triple>           triple;
        MCSerializeObject                       serialize_context;
        MCGenCodeOption                         option;
        MCGenCodeSyntax                         syntax;
        MCGenCodePretty                         pretty;
        std::unique_ptr<MCParser>               parser;
        MCSerializeData                         parser_data;
        std::string                             parser_data_string;
        std::string                             parser_fix_data_string;
    };
}

#endif // __CHEAT_GENCODE_H__