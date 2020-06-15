#pragma once
#include <WS2tcpip.h>
class Lock
{
public:
    Lock()
    {
        ::InitializeCriticalSection(&m_cs);
    }

    ~Lock()
    {
        ::DeleteCriticalSection(&m_cs);
    }

    void acquire()
    {
        ::EnterCriticalSection(&m_cs);
    }

    void release()
    {
        ::LeaveCriticalSection(&m_cs);
    }

private:
    Lock(const Lock&);
    Lock& operator=(const Lock&);

    CRITICAL_SECTION m_cs;
};