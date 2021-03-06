#include "pch.h"
#include "Thread.h"
#include "ThreadInfoManager.h"
#include "Console/Logger.h"

namespace Hawk {

const std::string Thread::sc_MainThreadName("Thread_Main");
const ThreadID Thread::sc_MainThreadID = 1;
std::atomic_uint Thread::s_uiNextThreadID = 2;

Thread::Thread(const std::string& p_Name, UpdateFunc p_UpdateFunc)
: m_Name(p_Name)
, m_Dispatcher(std::make_shared<Dispatcher>(p_Name))
, m_UpdateFunc(p_UpdateFunc)
, m_bStopSignal(false)
, m_ID(s_uiNextThreadID++)
{
}

void Thread::Start()
{
	m_Thread = std::thread(&Thread::Run, this);
#ifdef HAWK_DEBUG
	ThreadInfoManager::RegisterThread(m_Name, m_Thread.get_id(), m_ID);
#endif
}

void Thread::Stop()
{
	m_bStopSignal = true;
	m_Thread.join();
	LOG("*** Thread [" << GetName() << "] stopped ***", GetName(), Debug);
}

const std::string& Thread::GetName() const
{
	return m_Name;
}

ThreadID Thread::GetID() const
{
	return m_ID;
}

std::shared_ptr<Dispatcher> Thread::GetDispatcher() const
{
	return m_Dispatcher;
}

void Thread::Sleep()
{
#ifdef HAWK_DEBUG
	uint32 l_uiThreadSleepTimeMS = Config::Instance().Get("dev.threadSleepTimeMS", 0);
	if (l_uiThreadSleepTimeMS > 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(l_uiThreadSleepTimeMS));
	}
#endif
}

void Thread::Run()
{
	LOG("Thread started: " << GetName(), "core", Debug);
	while (!m_bStopSignal)
	{
		m_Dispatcher->Execute();
		m_UpdateFunc();
		Sleep();
	}
}

}