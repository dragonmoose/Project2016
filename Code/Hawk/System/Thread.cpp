#include "pch.h"
#include "System/Thread.h"
#include "Console/Logger.h"

namespace Hawk {

Thread::Thread(const std::string& p_Name, UpdateFunc_t p_UpdateFunc)
: m_Name(p_Name)
, m_Dispatcher(std::make_shared<Dispatcher>())
, m_UpdateFunc(p_UpdateFunc)
, m_bStopSignal(false)
{
}

void Thread::Start()
{
	m_Thread = std::thread(&Thread::Run, this);
#ifdef HAWK_DEBUG
	Logger::RegisterThreadName(m_Name, m_Thread.get_id());
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

std::shared_ptr<Dispatcher> Thread::GetDispatcher() const
{
	return m_Dispatcher;
}

void Thread::Run()
{
	while (!m_bStopSignal)
	{
		m_Dispatcher->Execute();
		m_UpdateFunc();
	}
}

}