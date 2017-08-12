#include "Lock.h"

using namespace System::Threading;

namespace lpubsppop01 {
namespace REPLReaderWriter {

// How to : Implement the lock C# Keyword(C++ - CLI):
// https://msdn.microsoft.com/en-us/library/sy1y3y1t.aspx?f=255&MSPPError=-2147217396

Lock::Lock(Object ^ pObject) : m_pObject(pObject)
{
    Monitor::Enter(m_pObject);
}

Lock::~Lock()
{
    Monitor::Exit(m_pObject);
}

}
}