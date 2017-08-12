#pragma once

#include "REPL.h"

using namespace System;
using namespace System::Collections::Generic;

namespace lpubsppop01 {
namespace REPLReaderWriter {

ref class REPLManager abstract sealed
{
    static List<REPL^>^ repls = gcnew List<REPL^>();
    static REPL^ FindREPL(int processID);
public:
    static int Start(String^ command, String^ templateName);
    static void Stop(int processID);
    static void SetTimeout(int processID, int timeoutMilliseconds);
    static void WriteLine(int processID, String^ inputText);
    static String^ ReadLine(int processID);
};

}
}