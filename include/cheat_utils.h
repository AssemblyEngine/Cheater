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

#ifndef __CHEAT_UTILS_H__
#define __CHEAT_UTILS_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <string>

#include <cheat_struct.h>

namespace cheat
{
    static constexpr uint8_t charhexset[] = "0123456789ABCDEFabcdef";

    static bool BinaryToString(const std::string& data, std::string& buffer)
    {
        if (data.empty())
        {
            return false;
        }

        if (buffer.size())
        {
            buffer.clear();
            buffer.shrink_to_fit();
        }

        for (const std::string::value_type& chars : data)
        {
            buffer += charhexset[static_cast<uint8_t>(chars) >> 0x4];
            buffer += charhexset[static_cast<uint8_t>(chars) & 0x0F];
        }

        return true;
    }
}

#endif // __CHEAT_UTILS_H__