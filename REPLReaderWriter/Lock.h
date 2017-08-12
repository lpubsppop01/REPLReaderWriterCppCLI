#pragma once

namespace lpubsppop01 {
namespace REPLReaderWriter {

// How to : Implement the lock C# Keyword(C++ - CLI):
// https://msdn.microsoft.com/en-us/library/sy1y3y1t.aspx?f=255&MSPPError=-2147217396

ref class Lock
{
    Object^ m_pObject;
public:
    Lock(Object^ pObject);
    ~Lock();
};

}
}