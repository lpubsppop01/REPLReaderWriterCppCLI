#include "REPLTemplates.h"

namespace lpubsppop01 {
namespace REPLReaderWriter {

REPL^ REPLTemplates::GetByName(String^ templateName)
{
    if (templateName == gcnew String(L"Python")) {
        auto python = gcnew REPL();
        python->Command = "python";
        python->Arguments = "-i";
        python->NewLine = "\n";
        python->PromptWithoutNewLine = "lpubsppop01.REPLReaderWriter";
        python->ScriptToSetPrompt = gcnew array<String^>(2);
        python->ScriptToSetPrompt[0] = "import sys";
        python->ScriptToSetPrompt[1] = "sys.ps1 = 'lpubsppop01.REPLReaderWriter\\n'";
        return python;
    }
    return nullptr;
}

}
}
