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

#ifndef __CHEAT_ARCHAVE__
#define __CHEAT_ARCHAVE__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <ctype.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef __CYGWIN__
#   define HAVE_OS_CYGWIN                                                                          1
#   elif defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
#       define HAVE_OS_WINDOWS                                                                     1
#   elif defined(_MSC_VER) && (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(_WIN64_) || defined(WIN64))
#       define HAVE_OS_WINDOWS                                                                     1
#   ifdef _WIN32_WCE
#       define HAVE_OS_WINDOWS_MOBILE                                                              1
#   elif defined(WINAPI_FAMILY)
#       include <winapifamily.h>
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#       define HAVE_OS_WINDOWS_DESKTOP                                                             1
#   elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)
#       define HAVE_OS_WINDOWS_PHONE                                                               1
#   elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#       defineHAVE_OS_WINDOWS_RT                                                                   1
#   elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_TV_TITLE)
#       define HAVE_OS_WINDOWS_PHONE                                                               1
#       define HAVE_OS_WINDOWS_TV_TITLE                                                            1
#   else
#       define HAVE_OS_WINDOWS_DESKTOP                                                             1
#   endif
#   else
#       define HAVE_OS_WINDOWS_DESKTOP                                                             1
#   endif  // _WIN32_WCE
#   elif defined __OS2__
#       define HAVE_OS_OS2                                                                         1
#   elif defined __APPLE__
#       define HAVE_OS_MAC                                                                         1
#       include <TargetConditionals.h>
#   if TARGET_OS_IPHONE
#       define HAVE_OS_IOS                                                                         1
#   endif
#   elif defined __DragonFly__
#       define HAVE_OS_DRAGONFLY                                                                   1
#   elif defined __FreeBSD__
#       define HAVE_OS_FREEBSD                                                                     1
#   elif defined __Fuchsia__
#       define HAVE_OS_FUCHSIA                                                                     1
#   elif defined(__GLIBC__) && defined(__FreeBSD_kernel__)
#       define HAVE_OS_GNU_KFREEBSD                                                                1
#   elif defined __linux__
#       if defined(__arm__) || defined(__thumb__) || defined(__aarch64__)
#           define HAVE_OS_LINUX_ARM                                                               1
#       endif
#       define HAVE_OS_LINUX                                                                       1
#       if defined __ANDROID__
#           define HAVE_OS_LINUX_ANDROID                                                           1
#       endif
#   elif defined __MVS__
#       define HAVE_OS_ZOS                                                                         1
#   elif defined(__sun) && defined(__SVR4)
#       define HAVE_OS_SOLARIS                                                                     1
#   elif defined(_AIX)
#       define HAVE_OS_AIX                                                                         1
#   elif defined(__hpux)
#       define HAVE_OS_HPUX                                                                        1
#   elif defined __native_client__
#       define HAVE_OS_NACL                                                                        1
#   elif defined __NetBSD__
#       define HAVE_OS_NETBSD                                                                      1
#   elif defined __OpenBSD__
#       define HAVE_OS_OPENBSD                                                                     1
#   elif defined __QNX__
#       define HAVE_OS_QNX                                                                         1
#   elif defined(__HAIKU__)
#       define HAVE_OS_HAIKU                                                                       1
#   elif defined ESP8266
#       define HAVE_OS_ESP8266                                                                     1
#   elif defined ESP32
#       define HAVE_OS_ESP32                                                                       1
#endif  // __CYGWIN__

#endif // __CHEAT_ARCHAVE__