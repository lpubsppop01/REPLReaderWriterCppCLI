#pragma once

#using "System.dll"

using namespace System;
using namespace System::Diagnostics;
using namespace System::Text;

namespace lpubsppop01 {
namespace REPLReaderWriter {

ref class REPL : public ICloneable
{
    ref class RuntimeValueSet
    {
    public:
        property Process^ Process;
        property StringBuilder^ OutputBuffer;
        property StringBuilder^ ErrorBuffer;
        property Object^ OutputBufferLock;
        property Object^ ErrorBufferLock;
        property String^ NewLine;
        property String^ PromptWithNewLine;
        property int TimeoutMilliseconds;
    };
    RuntimeValueSet^ m_RuntimeValues;
    int m_TimeoutMilliseconds;
    void this_OutputDataReceived(Object^ sender, DataReceivedEventArgs^ e);
    void this_ErrorDataReceived(Object^ sender, DataReceivedEventArgs^ e);
protected:
    REPL(REPL^ src);
public:
    REPL();

    virtual System::Object^ Clone();

    property String^ Command;
    property String^ Arguments;
    property String^ NewLine;
    property String^ PromptWithNewLine;
    property array<String^>^ ScriptToSetPrompt;

    property int TimeoutMilliseconds
    {
        int get() { return m_TimeoutMilliseconds; }
        void set(int value)
        {
            m_TimeoutMilliseconds = value;
            if (m_RuntimeValues) {
                m_RuntimeValues->TimeoutMilliseconds = value;
            }
        }
    }

    property int ProcessID
    {
        int get()
        {
            if (!m_RuntimeValues) return 0;
            return m_RuntimeValues->Process->Id;
        }
    }

    int Start();
    void WriteLine(String^ inputText);
    String^ ReadLine();
    void Stop(int processID);
};

}
}