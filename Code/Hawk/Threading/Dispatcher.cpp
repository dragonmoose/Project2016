#include "pch.h"
#include "Dispatcher.h"

namespace Hawk {

Dispatcher::Dispatcher(const std::string& p_ThreadName)
: m_Mutex(std::string("Dispatcher") + "->" + p_ThreadName)
{
}

void Dispatcher::Post(Func p_Func)
{
	MutexScope l_MutexScope(m_Mutex);
	m_Functions.push_back(p_Func);
}

void Dispatcher::Execute()
{
	FuncVec l_Functions;
	{
		MutexScope l_MutexScope(m_Mutex);
		m_Functions.swap(l_Functions);
	}

	for (auto& l_Function : l_Functions)
	{
		try
		{
			l_Function();
		}
		catch (Exception& e)
		{
			LOG_EXCEPTION(e, "dispatcher", Error);
		}
	}
}

}