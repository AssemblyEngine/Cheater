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

#ifndef __CHEAT_JSON_UTILS_H__
#define __CHEAT_JSON_UTILS_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#   pragma once
#endif

#include <string>
#include <functional>

#include <cheat_json.h>
#include <cheat_struct.h>

namespace cheat
{
    static void from_json(const MCSerializeObject& deserializer, struct MCKind& object)
    {
        if (deserializer.empty())
        {
            return;
        }

        try
        {
            std::string buffer;
            {
                deserializer.at("name").get_to(buffer);
                deserializer.at("value").get_to(object.value);

                std::copy(buffer.begin(), buffer.end(), object.name);
            }
        }
        catch (const MCSerializeObject::type_error& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::out_of_range& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::parse_error& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::exception& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (...)
        {
            printf("\n");
        }
    }

    static void from_json(const MCSerializeObject& deserializer, struct MCFixup& object)
    {
        if (deserializer.empty())
        {
            return;
        }

        try
        {
            deserializer.at("kind").at("value").get_to(object.kind.value);

            deserializer.at("info").at("target_offset").get_to(object.info.target_offset);
            deserializer.at("info").at("target_size").get_to(object.info.target_size);

            deserializer.at("offset").get_to(object.offset);

            deserializer.at("value").at("kind").at("value").get_to(object.value.kind.value);
            deserializer.at("value").at("opcode").at("value").get_to(object.value.opcode.value);

            deserializer.at("value").at("lhs").at("kind").at("value").get_to(object.value.lhs.kind.value);
            deserializer.at("value").at("lhs").at("value").get_to(object.value.lhs.value);
            deserializer.at("value").at("lhs").at("format").get_to(object.value.lhs.fotmat);

            deserializer.at("value").at("rhs").at("kind").at("value").get_to(object.value.rhs.kind.value);
            deserializer.at("value").at("rhs").at("value").get_to(object.value.rhs.value);
            deserializer.at("value").at("rhs").at("format").get_to(object.value.rhs.fotmat);
        }
        catch (const MCSerializeObject::type_error& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::out_of_range& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::parse_error& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::exception& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (...)
        {
            printf("\n");
        }
    }

    static void from_json(const MCSerializeObject& deserializer, struct MCInst& object)
    {
        try
        {
            deserializer.at("opcode").at("value").get_to(object.inst.opcode.value);
            deserializer.at("flags").get_to(object.inst.flags);
        }
        catch (const MCSerializeObject::type_error& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::out_of_range& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::parse_error& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (const MCSerializeObject::exception& ex)
        {
            printf("%s\n", ex.what());
        }
        catch (...)
        {
            printf("\n");
        }
    }
}

#endif // __CHEAT_JSON_UTILS_H__