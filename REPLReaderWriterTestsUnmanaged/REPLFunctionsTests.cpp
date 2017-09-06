#include "stdafx.h"
#include "CppUnitTest.h"
#include "REPLFunctions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace REPLReaderWriterTests
{
    TEST_CLASS(REPLFunctionsTests)
    {
    public:

        TEST_METHOD(Unmanaged_TestWithPython)
        {
            int processID = REPLStartW(L"C:/tools/Anaconda3/python.exe", L"Python");
            Assert::IsTrue(processID > 0);
            REPLSetTimeout(processID, 3000);

            REPLWriteLineW(processID, L"1 + 1");
            if (REPLWaitFor(processID, L".*")) {
                Assert::Fail();
            }
            auto outputText = REPLReadLineW(processID);
            Assert::IsTrue(outputText != NULL && wcscmp(L"2", outputText) == 0);
            if (REPLWaitForPrompt(processID)) {
                Assert::Fail();
            }
            auto errorText = REPLReadErrorLineW(processID);
            Assert::AreEqual(NULL, errorText);

            int errorCode = REPLStop(processID);
            Assert::IsTrue(errorCode == 0);
        }

    };
}
