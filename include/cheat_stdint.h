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

#ifndef __CHEAT_STDINT_H__
#define __CHEAT_STDINT_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <stddef.h>
#include <stdint.h>

#include <cheat_config.h>

#ifndef HAVE_STDINT
    typedef char                                s8;
    typedef short                               s16;
    typedef int                                 s32;
    typedef long long                           s64;

    typedef unsigned char                       u8;
    typedef unsigned short                      u16;
    typedef unsigned int                        u32;
    typedef unsigned long long                  u64;
#endif

#endif // __CHEAT_STDINT_H__