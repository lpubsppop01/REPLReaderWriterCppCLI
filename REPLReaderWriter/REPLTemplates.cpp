#include "REPLTemplates.h"

namespace lpubsppop01 {
namespace REPLReaderWriter {

REPL^ REPLTemplates::GetByName(String^ templateName)
{
    if (templateName == gcnew String(L"Python")) {
        auto python = gcnew REPL();
        python->Command = L"python";
        python->Arguments = L"-i";
        python->NewLine = L"\n";
        python->PromptWithoutNewLine = L"lpubsppop01.REPLReaderWriter";
        python->ScriptToSetPrompt = gcnew array<String^>(2);
        python->ScriptToSetPrompt[0] = L"import sys";
        python->ScriptToSetPrompt[1] = L"sys.ps1 = 'lpubsppop01.REPLReaderWriter\\n'";
        return python;
    }
    return nullptr;
}

}
}
