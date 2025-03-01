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

#ifndef __CHEAT_CONFIG_H__
#define __CHEAT_CONFIG_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#ifndef HAVE_DISABLE_STDINT
#   undef HAVE_STDINT
#endif

#ifndef HAVE_DISABLE_BACKWARD
#   undef HAVE_BACKWARD
#endif

#if defined(__cplusplus)
#   define HAVE_CPLUSPLUS
#endif

#include <cheat_platform.h>

#endif  // __CHEAT_CONFIG_H__