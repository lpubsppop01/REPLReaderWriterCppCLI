#include "REPLFunctions.h"
#include <vector>
#include <wchar.h>
#include "REPLManager.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace lpubsppop01::REPLReaderWriter;

const int WCHAR_ARRAY_COUNT_MAX = 100;
std::vector<wchar_t*> wcharArrays;

const wchar_t* NewWCharArray(String^ str)
{
    auto wcharArray = new wchar_t[str->Length + 1];
    for (int i = 0; i < str->Length; ++i) {
        wcharArray[i] = str[i];
    }
    wcharArray[str->Length] = L'\0';
    wcharArrays.push_back(wcharArray);
    while (wcharArrays.size() > WCHAR_ARRAY_COUNT_MAX) {
        auto i = wcharArrays.begin();
        delete *i;
        wcharArrays.erase(i);
    }
    return wcharArray;
}

void DeleteWCharArrays()
{
    for (auto i = wcharArrays.begin(); i != wcharArrays.end(); ++i) {
        delete *i;
    }
    wcharArrays.clear();
}

DECLSPEC_DLLPORT int REPLStartW(const wchar_t* command, const wchar_t* templateName)
{
    try {
        return REPLManager::Start(gcnew String(command), gcnew String(templateName));
    }
    catch (Exception^) {
        return -1;
    }
}

DECLSPEC_DLLPORT void REPLSetTimeout(int processID, int timeoutMilliseconds)
{
    REPLManager::SetTimeout(processID, timeoutMilliseconds);
}

DECLSPEC_DLLPORT void REPLWriteLineW(int processID, const wchar_t* inputText)
{
    REPLManager::WriteLine(processID, gcnew String(inputText));
}

DECLSPEC_DLLPORT int REPLWaitFor(int processID, const wchar_t* pattern)
{
    try {
        REPLManager::WaitFor(processID, gcnew String(pattern));
    } catch (Exception^) {
        return -1;
    }
    return 0;
}

DECLSPEC_DLLPORT int REPLWaitForPrompt(int processID)
{
    try {
        REPLManager::WaitForPrompt(processID);
    } catch (Exception^) {
        return -1;
    }
    return 0;
}

DECLSPEC_DLLPORT const wchar_t* REPLReadOutputLineW(int processID)
{
    auto lineStr = REPLManager::ReadOutputLine(processID);
    if (lineStr == nullptr) return NULL;
    auto lineWCharArray = NewWCharArray(lineStr);
    return lineWCharArray;
}

DECLSPEC_DLLPORT const wchar_t* REPLReadErrorLineW(int processID)
{
    auto lineStr = REPLManager::ReadErrorLine(processID);
    if (lineStr == nullptr) return NULL;
    auto lineWCharArray = NewWCharArray(lineStr);
    return lineWCharArray;
}

DECLSPEC_DLLPORT int REPLStop(int processID)
{
    try {
        REPLManager::Stop(processID);
        DeleteWCharArrays();
    } catch (Exception^) {
        return -1;
    }
    return 0;
}
