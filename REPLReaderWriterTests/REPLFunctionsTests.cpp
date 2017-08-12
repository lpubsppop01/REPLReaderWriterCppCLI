#include "stdafx.h"
#include "CppUnitTest.h"
#include "REPLFunctions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace REPLReaderWriterTests
{
    TEST_CLASS(REPLFunctionsTests)
    {
    public:

        TEST_METHOD(TestWithPython)
        {
            int processID = REPLStartW(L"C:\\tools\\Anaconda3\\python.exe", L"Python");
            Assert::IsTrue(processID > 0);
            REPLSetTimeout(processID, 1000);
            REPLWriteLineW(processID, L"1 + 1");
            auto outputText = REPLReadLineW(processID);
            Assert::IsTrue(wcscmp(L"2", outputText) == 0);
            int errorCode = REPLStop(processID);
            Assert::IsTrue(errorCode == 0);
        }

    };
}
