#include "REPL.h"
#include "Lock.h"
#using "System.Core.dll"

using namespace System::Linq;

namespace lpubsppop01 {
namespace REPLReaderWriter {

void REPL::this_OutputDataReceived(Object ^ sender, DataReceivedEventArgs ^ e)
{
    if (!(e->Data)) return;
    Lock lock(m_RuntimeValues->OutputBufferLock);
    m_RuntimeValues->OutputBuffer->AppendLine(e->Data);
}

void REPL::this_ErrorDataReceived(Object ^ sender, DataReceivedEventArgs ^ e)
{
    if (!(e->Data)) return;
    Lock lock(m_RuntimeValues->ErrorBufferLock);
    m_RuntimeValues->ErrorBuffer->AppendLine(e->Data);
}

REPL::REPL(REPL^ src)
{
    Command = src->Command;
    Arguments = src->Arguments;
    NewLine = src->NewLine;
    PromptWithNewLine = src->PromptWithNewLine;
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
    PromptWithNewLine = gcnew String("");
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
    m_RuntimeValues->OutputBuffer = gcnew StringBuilder();
    m_RuntimeValues->OutputBufferLock = gcnew Object();
    m_RuntimeValues->ErrorBuffer = gcnew StringBuilder();
    m_RuntimeValues->ErrorBufferLock = gcnew Object();
    m_RuntimeValues->TimeoutMilliseconds = TimeoutMilliseconds;
    m_RuntimeValues->NewLine = NewLine;
    m_RuntimeValues->PromptWithNewLine = PromptWithNewLine;

    process->OutputDataReceived += gcnew DataReceivedEventHandler(this, &REPL::this_OutputDataReceived);
    process->ErrorDataReceived += gcnew DataReceivedEventHandler(this, &REPL::this_ErrorDataReceived);

    process->Start();
    process->BeginOutputReadLine();
    process->BeginErrorReadLine();

    if (ScriptToSetPrompt)
    {
        for (int i = 0; i < ScriptToSetPrompt->Length; ++i) {
            WriteLine(ScriptToSetPrompt[i]);
        }
    }

    return process->Id;
}

void REPL::WriteLine(String ^ inputText)
{
    if (!m_RuntimeValues) throw gcnew InvalidOperationException();
    m_RuntimeValues->Process->StandardInput->Write(inputText + NewLine);
    m_RuntimeValues->Process->StandardInput->Flush();
}

String ^ REPL::ReadLine()
{
    if (!m_RuntimeValues) throw gcnew InvalidOperationException();

    // Wait for prompt
    Stopwatch^ sw = nullptr;
    if (m_RuntimeValues->TimeoutMilliseconds > 0)
    {
        sw = gcnew Stopwatch();
        sw->Start();
    }
    do
    {
        Lock lock(m_RuntimeValues->ErrorBufferLock);
        auto errorText = m_RuntimeValues->ErrorBuffer->ToString();
        m_RuntimeValues->ErrorBuffer->Clear();
        if (errorText->EndsWith(PromptWithNewLine)) break;
        if (m_RuntimeValues->TimeoutMilliseconds > 0)
        {
            if (sw->ElapsedMilliseconds > m_RuntimeValues->TimeoutMilliseconds)
            {
                throw gcnew TimeoutException();
            }
        }
    } while (true);
    if (m_RuntimeValues->TimeoutMilliseconds > 0)
    {
        sw->Stop();
    }

    // Get standard output text
    auto outputText = gcnew String(L"");
    do
    {
        Lock lock(m_RuntimeValues->OutputBufferLock);
        outputText += m_RuntimeValues->OutputBuffer->ToString();
        m_RuntimeValues->OutputBuffer->Clear();
    } while (outputText->Length == 0);
    return outputText->TrimEnd(L'\r', L'\n');
}

void REPL::Stop(int processID)
{
    if (!m_RuntimeValues) throw gcnew InvalidOperationException();
    if (!m_RuntimeValues->Process->HasExited)
    {
        m_RuntimeValues->Process->Kill();
    }
    m_RuntimeValues->Process->Close();
    m_RuntimeValues = nullptr;
}

}
}