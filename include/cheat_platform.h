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

#ifndef __CHEAT_PLATFORM_H__
#define __CHEAT_PLATFORM_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <cheat_arch.h>
#include <cheat_config.h>
#include <iostream>
#if defined(HAVE_OS_WINDOWS)
#   if defined(HAVE_CPLUSPLUS)
#       ifndef NOMINMAX
#           define NOMINMAX
#       endif
#   endif
#   include <windows.h>
#   pragma warning(disable: 4996)
#   pragma comment(lib, "ws2_32.lib")
#endif

#ifdef HAVE_BACKWARD
#   if defined(HAVE_CPLUSPLUS)
#       include <backward.hpp>
#   endif
#endif

#define FORCESPLICING(...) #__VA_ARGS__

#include <cheat_party.h>

#endif  // __CHEAT_PLATFORM_H__