#include "pch.h"
#include "Thread.h"
#include "Console/Logger.h"

namespace Hawk {

std::string Thread::MainThreadName("Thread_Main");
std::atomic_uint Thread::s_uiNextThreadID = 1;

Thread::Thread(const std::string& p_Name, UpdateFunc_t p_UpdateFunc)
: m_Name(p_Name)
, m_Dispatcher(std::make_shared<Dispatcher>(p_Name))
, m_UpdateFunc(p_UpdateFunc)
, m_bStopSignal(false)
, m_ID(s_uiNextThreadID++)
{
}

void Thread::RegisterFrameBegin(FrameBeginCallback_t p_Callback)
{
	m_FrameBegin = p_Callback;
}
void Thread::RegisterFrameEnd(FrameBeginCallback_t p_Callback)
{
	m_FrameEnd = p_Callback;
}

void Thread::Start()
{
	m_Thread = std::thread(&Thread::Run, this);
#ifdef HAWK_DEBUG
	Logger::RegisterThread(m_Name, m_Thread.get_id(), m_ID);
#endif
}

void Thread::Stop()
{
	m_bStopSignal = true;
	m_Thread.join();
	LOG("*** Thread stopped ***", GetName(), Debug);
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
	unsigned int l_uiThreadSleepTimeMS = Config::Instance().Get("dev.threadSleepTimeMS", 0);
	if (l_uiThreadSleepTimeMS > 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(l_uiThreadSleepTimeMS));
	}
#endif
}

void Thread::Run()
{
	while (!m_bStopSignal)
	{
		if (m_FrameBegin)
		{
			m_FrameBegin();
		}

		m_Dispatcher->Execute();
		m_UpdateFunc();

		if (m_FrameEnd)
		{
			m_FrameEnd();
		}
		Sleep();
	}
}

}