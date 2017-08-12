#include "REPLManager.h"
#include "REPLTemplates.h"

namespace lpubsppop01 {
namespace REPLReaderWriter {

REPL ^ REPLManager::FindREPL(int processID)
{
    for (int i = 0; i < repls->Count; ++i) {
        if (repls[i]->ProcessID == processID) {
            return repls[i];
        }
    }
    return nullptr;
}

int REPLManager::Start(String ^ command, String ^ templateName)
{
    auto repl = (REPL^)REPLTemplates::GetByName(templateName)->Clone();
    repl->Command = command;
    repls->Add(repl);
    return repl->Start();
}

void REPLManager::Stop(int processID)
{
    auto repl = FindREPL(processID);
    if (repl == nullptr) throw gcnew InvalidOperationException();
    repl->Stop(processID);
}

void REPLManager::SetTimeout(int processID, int timeoutMilliseconds)
{
    auto repl = FindREPL(processID);
    if (repl == nullptr) throw gcnew InvalidOperationException();
    repl->TimeoutMilliseconds = timeoutMilliseconds;
}

void REPLManager::WriteLine(int processID, String^ inputText)
{
    auto repl = FindREPL(processID);
    if (repl == nullptr) throw gcnew InvalidOperationException();
    repl->WriteLine(inputText);
}

String^ REPLManager::ReadLine(int processID)
{
    auto repl = FindREPL(processID);
    if (repl == nullptr) throw gcnew InvalidOperationException();
    return repl->ReadLine();
}

}
}