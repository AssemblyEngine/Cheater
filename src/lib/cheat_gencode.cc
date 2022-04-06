#include <cheat_gencode.h>
#include <cheat_parser.h>
#include <cheat_utils.h>
#include <cheat_json_utils.h>

namespace cheat
{
    MCGenCode::MCGenCode()
        : MCGenCode(llvm::sys::getDefaultTargetTriple())
    {

    }

    MCGenCode::MCGenCode(const std::string& target_triple)
        : default_triple(target_triple)
        , serialize_context()
        , option(MCGenCodeOption::MC_OPTION_SYNTAX)
        , syntax(MCGenCodeSyntax::MC_SYNTAX_INTEL)
        , pretty(MCGenCodePretty::MC_OPTION_PRINT)
    {

    }

    MCGenCode::~MCGenCode()
    {

    }

    MCGenCodeError MCGenCode::setOption(MCGenCodeOption op, std::size_t value)
    {
        switch (op)
        {
            case MCGenCodeOption::MC_OPTION_SYNTAX:
            {
                switch (value)
                {
                    case MCGenCodeSyntax::MC_SYNTAX_ATT:
                    case MCGenCodeSyntax::MC_SYNTAX_INTEL: syntax = static_cast<MCGenCodeSyntax>(value); return MCGenCodeError::MC_SUCCESS;
                    default: break;
                }

                return MC_INVALID_OPTION_VALUE;
            }
            case MCGenCodeOption::MC_OPTION_PRETTY_PRINT:
            {
                switch (value)
                {
                    case MCGenCodePretty::MC_OPTION_PRINT:   pretty = MC_OPTION_PRINT;   return MCGenCodeError::MC_SUCCESS;
                    case MCGenCodePretty::MC_OPTION_NOPRINT: pretty = MC_OPTION_NOPRINT; return MCGenCodeError::MC_SUCCESS;
                    default: break;
                }

                return MC_INVALID_OPTION_VALUE;
            }
            default: break;
        }

        return MC_INVALID_OPTION;
    }

    bool MCGenCode::serializeParser(const std::string& puts, std::uint64_t runtime, const std::string& arch_name, uint32_t clear)
    {
        if (puts.empty() || 0 == runtime || arch_name.empty())
        {
            return false;
        }

        if (clear)
        {
            clearData();
        }

        if (serialize_context.size())
        {
            serialize_context.clear();
        }

        if (nullptr == parser)
        {
            parser.reset(new MCParser(std::addressof(serialize_context),
                                      std::shared_ptr<llvm::Triple>(new llvm::Triple(llvm::Triple::normalize(arch_name == default_triple ? default_triple : arch_name)))));
        }

        if (parser->serializeParser(puts, syntax, pretty))
        {
            return false;
        }
        else
        {
            applyDeserializer(serialize_context, runtime);
        }

        return true;
    }

    void MCGenCode::applyDeserializer(const MCSerializeObject& insts, std::uint64_t runtime)
    {
        for (const MCSerializeObject &inst : insts)
        {
            try
            {
                if (inst.at("kind").get<struct MCKind>().value == MCObjectType::MC_INSTRUCTION)
                {
                    if (inst.at("fixups").size())
                    {
                        applyDeserializer(inst.at("fixups"), inst.at("data"), inst.at("binary"), runtime);
                    }
                    else
                    {
                        applyDeserializer(inst.at("data"), inst.at("binary"), runtime);
                    }
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
    }

    void MCGenCode::applyDeserializer(const MCSerializeObject& fixups, const MCSerializeData& data, const std::string& binary, uint64_t runtime)
    {
        for (const struct MCFixup& fixup : fixups)
        {
            if (fixup.kind.value & llvm::MCFixupKindInfo::FKF_IsPCRel)
            {
                if (fixup.value.opcode.value == llvm::MCBinaryExpr::Add)
                {
                    std::uint32_t adjust_bytes = llvm::alignTo(fixup.info.target_size, 8) / 8;
                    // Shift the value into position.
                    std::uint64_t adjust_fixup_value = static_cast<std::int64_t>(static_cast<std::int64_t>(static_cast<std::int64_t>(fixup.value.lhs.value) + static_cast<std::int64_t>(fixup.value.rhs.value - static_cast<std::int64_t>(fixup.offset))) - static_cast<std::int64_t>(runtime)) - parser_data.size();
                    appendData(data);
                    parser_data_string.append(binary);
                    // For each byte of the fragment that the fixup touches, mask in the
                    // bits from the fixup value.
                    for (std::size_t i = 0, j = parser_data.size() - adjust_bytes; i < adjust_bytes; ++i)
                    {
                        parser_data[j + i] = static_cast<std::uint8_t>((adjust_fixup_value >> (i * 8)) & 0xff);
                    }

                    BinaryToString(std::string(parser_data.begin(), parser_data.end()), parser_fix_data_string);
                }
            }
        }
    }

    void MCGenCode::applyDeserializer(const MCSerializeData& data, const std::string& binary, std::uint64_t runtime)
    {
        appendData(data);
        parser_data_string.append(binary);
    }
}