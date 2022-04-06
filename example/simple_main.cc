#include <iostream>
#include <cheat_gencode.h>

int main()
{
    cheat::MCGenCode code;
    code.setOption(cheat::MCGenCodeOption::MC_OPTION_SYNTAX, cheat::MCGenCodeSyntax::MC_SYNTAX_ATT);
    for (;;)
    {
        std::string line;
        std::getline(std::cin, line);

        try
        {
            if (code.serializeParser(line, 0x7c33cea9, "i386-pc-windows-msvc"))
            {
                printf("%s\n", code.getDump().c_str());
                printf("%s\n", code.getFixDataString().c_str());
            }
        }
        catch (const std::exception& ex)
        {
            printf("%s\n", ex.what());
        }
    }
}