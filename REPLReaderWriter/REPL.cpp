#include "REPL.h"
#include "Lock.h"
#using "System.Core.dll"

using namespace System::Linq;
using namespace System::Text::RegularExpressions;

namespace lpubsppop01 {
namespace REPLReaderWriter {

void REPL::this_OutputDataReceived(Object^ sender, DataReceivedEventArgs^ e)
{
    if (!(e->Data)) return;
    Lock lock(m_RuntimeValues->UntestedOutputLinesLock);
    m_RuntimeValues->UntestedOutputLines->Add(e->Data);
}

void REPL::this_ErrorDataReceived(Object^ sender, DataReceivedEventArgs^ e)
{
    if (!(e->Data)) return;
    Lock lock(m_RuntimeValues->UntestedErrorLinesLock);
    m_RuntimeValues->UntestedErrorLines->Add(e->Data);
}

void REPL::ClearBuffer()
{
    {
        Lock lock(m_RuntimeValues->UntestedOutputLinesLock);
        m_RuntimeValues->UntestedOutputLines->Clear();
    }
    {
        Lock lock(m_RuntimeValues->TestedOutputLinesLock);
        m_RuntimeValues->TestedOutputLines->Clear();
    }
    {
        Lock lock(m_RuntimeValues->UntestedErrorLinesLock);
        m_RuntimeValues->UntestedErrorLines->Clear();
    }
    {
        Lock lock(m_RuntimeValues->TestedErrorLinesLock);
        m_RuntimeValues->TestedErrorLines->Clear();
    }
}

REPL::REPL(REPL^ src)
{
    Command = src->Command;
    Arguments = src->Arguments;
    NewLine = src->NewLine;
    PromptWithoutNewLine = src->PromptWithoutNewLine;
    if (src->ScriptToSetPrompt) {
        auto copy = gcnew array<String^>(src->ScriptToSetPrompt->Length);
        for (int i = 0; i < src->ScriptToSetPrompt->Length; ++i) {
            copy[i] = src->ScriptToSetPrompt[i];
        }
        ScriptToSetPrompt = copy;
    }
}

REPL::REPL()
{
    Command = gcnew String("");
    Arguments = gcnew String("");
    NewLine = Environment::NewLine;
    PromptWithoutNewLine = gcnew String("");
    ScriptToSetPrompt = nullptr;
}

System::Object^ REPL::Clone()
{
    return gcnew REPL(this);
}

int REPL::Start()
{
    auto process = gcnew Process();
    process->StartInfo->FileName = Command;
    process->StartInfo->Arguments = Arguments;
    process->StartInfo->CreateNoWindow = true;
    process->StartInfo->UseShellExecute = false;
    process->StartInfo->RedirectStandardInput = true;
    process->StartInfo->RedirectStandardOutput = true;
    process->StartInfo->RedirectStandardError = true;
    process->StartInfo->StandardOutputEncoding = gcnew UTF8Encoding(false);
    process->StartInfo->StandardErrorEncoding = gcnew UTF8Encoding(false);

    m_RuntimeValues = gcnew RuntimeValueSet();
    m_RuntimeValues->Process = process;
    m_RuntimeValues->UntestedOutputLines = gcnew List<String^>();
    m_RuntimeValues->UntestedErrorLines = gcnew List<String^>();
    m_RuntimeValues->UntestedOutputLinesLock = gcnew Object();
    m_RuntimeValues->UntestedErrorLinesLock = gcnew Object();
    m_RuntimeValues->TestedOutputLines = gcnew List<String^>();
    m_RuntimeValues->TestedErrorLines = gcnew List<String^>();
    m_RuntimeValues->TestedOutputLinesLock = gcnew Object();
    m_RuntimeValues->TestedErrorLinesLock = gcnew Object();
    m_RuntimeValues->TimeoutMilliseconds = TimeoutMilliseconds;
    m_RuntimeValues->NewLine = NewLine;
    m_RuntimeValues->PromptWithNewLine = PromptWithoutNewLine;
    m_RuntimeValues->WriteLineCount = 0;

    process->OutputDataReceived += gcnew DataReceivedEventHandler(this, &REPL::this_OutputDataReceived);
    process->ErrorDataReceived += gcnew DataReceivedEventHandler(this, &REPL::this_ErrorDataReceived);

    process->Start();
    process->BeginOutputReadLine();
    process->BeginErrorReadLine();

    if (ScriptToSetPrompt) {
        for (int i = 0; i < ScriptToSetPrompt->Length; ++i) {
            WriteLine(ScriptToSetPrompt[i]);
        }
    }

    m_RuntimeValues->WriteLineCount = 1;
    WaitForPrompt();
    ClearBuffer();

    return process->Id;
}

void REPL::Stop(int processID)
{
    if (!m_RuntimeValues) throw gcnew InvalidOperationException();
    if (!m_RuntimeValues->Process->HasExited) {
        m_RuntimeValues->Process->Kill();
    }
    m_RuntimeValues->Process->Close();
    m_RuntimeValues = nullptr;
}

void REPL::WriteLine(String^ inputText)
{
    if (!m_RuntimeValues) throw gcnew InvalidOperationException();
    m_RuntimeValues->Process->StandardInput->Write(inputText + NewLine);
    m_RuntimeValues->Process->StandardInput->Flush();
    ++(m_RuntimeValues->WriteLineCount);
}

void REPL::WaitFor(String^ pattern)
{
    Stopwatch^ sw = nullptr;
    if (m_RuntimeValues->TimeoutMilliseconds > 0) {
        sw = gcnew Stopwatch();
        sw->Start();
    }

    do {
        // Check standard output text
        if (!String::IsNullOrEmpty(pattern)) {
            Lock lock1(m_RuntimeValues->UntestedOutputLinesLock);
            bool breaks = false;
            while (m_RuntimeValues->UntestedOutputLines->Count > 0) {
                auto outputLine = m_RuntimeValues->UntestedOutputLines[0];
                m_RuntimeValues->UntestedOutputLines->RemoveAt(0);
                {
                    Lock lock2(m_RuntimeValues->TestedOutputLinesLock);
                    m_RuntimeValues->TestedOutputLines->Add(outputLine);
                }
                if (Regex::IsMatch(outputLine, pattern)) {
                    breaks = true;
                    break;
                }
            }
            if (breaks) break;
        }

        // Check timeout
        if (m_RuntimeValues->TimeoutMilliseconds > 0) {
            if (sw->ElapsedMilliseconds > m_RuntimeValues->TimeoutMilliseconds) {
                throw gcnew TimeoutException();
            }
        }
    } while (true);

    if (m_RuntimeValues->TimeoutMilliseconds > 0) {
        sw->Stop();
    }
}

void REPL::WaitForPrompt()
{
    Stopwatch^ sw = nullptr;
    if (m_RuntimeValues->TimeoutMilliseconds > 0) {
        sw = gcnew Stopwatch();
        sw->Start();
    }

    do {
        // Check standard error text
        if (m_RuntimeValues->WriteLineCount > 0) {
            Lock lock1(m_RuntimeValues->UntestedErrorLinesLock);
            bool breaks = false;
            while (m_RuntimeValues->UntestedErrorLines->Count > 0) {
                auto errorLine = m_RuntimeValues->UntestedErrorLines[0];
                m_RuntimeValues->UntestedErrorLines->RemoveAt(0);
                if (errorLine->EndsWith(PromptWithoutNewLine)) {
                    errorLine = errorLine->Replace(PromptWithoutNewLine, L"");
                    --(m_RuntimeValues->WriteLineCount);
                    breaks = true;
                }
                if (!String::IsNullOrEmpty(errorLine)) {
                    Lock lock2(m_RuntimeValues->TestedErrorLinesLock);
                    m_RuntimeValues->TestedErrorLines->Add(errorLine);
                }
                if (breaks) break;
            }
            if (breaks) break;
        }

        // Check timeout
        if (m_RuntimeValues->TimeoutMilliseconds > 0) {
            if (sw->ElapsedMilliseconds > m_RuntimeValues->TimeoutMilliseconds) {
                throw gcnew TimeoutException();
            }
        }
    } while (true);

    if (m_RuntimeValues->TimeoutMilliseconds > 0) {
        sw->Stop();
    }
}

String^ REPL::ReadLine()
{
    {
        Lock lock(m_RuntimeValues->UntestedOutputLinesLock);
        if (m_RuntimeValues->UntestedOutputLines->Count > 0) {
            auto outputLine = m_RuntimeValues->UntestedOutputLines[0];
            m_RuntimeValues->UntestedOutputLines->RemoveAt(0);
            return outputLine;
        }
    }
    {
        Lock lock(m_RuntimeValues->TestedOutputLinesLock);
        if (m_RuntimeValues->TestedOutputLines->Count > 0) {
            auto outputLine = m_RuntimeValues->TestedOutputLines[0];
            m_RuntimeValues->TestedOutputLines->RemoveAt(0);
            return outputLine;
        }
    }
    return nullptr;
}

String^ REPL::ReadErrorLine()
{
    {
        Lock lock(m_RuntimeValues->UntestedErrorLinesLock);
        if (m_RuntimeValues->UntestedErrorLines->Count > 0) {
            auto errorLine = m_RuntimeValues->UntestedErrorLines[0];
            m_RuntimeValues->UntestedErrorLines->RemoveAt(0);
            return errorLine;
        }
    }
    {
        Lock lock(m_RuntimeValues->TestedErrorLinesLock);
        if (m_RuntimeValues->TestedErrorLines->Count > 0) {
            auto errorLine = m_RuntimeValues->TestedErrorLines[0];
            m_RuntimeValues->TestedErrorLines->RemoveAt(0);
            return errorLine;
        }
    }
    return nullptr;
}

}
}