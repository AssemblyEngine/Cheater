﻿// Copyright 2022, Zephyr Authors
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

#ifndef __CHEAT_JSON_H__
#define __CHEAT_JSON_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <stdint.h>
#include <vector>

#include <json.hpp>

namespace cheat
{
    using MCSerializeObject   = typename::nlohmann::basic_json<nlohmann::ordered_map>;
    using MCSerializeData     = std::vector<uint8_t>;
}

#endif // __CHEAT_JSON_H__