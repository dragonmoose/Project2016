#include "pch.h"
#include "System/Dispatcher.h"

namespace Hawk {

void Dispatcher::Post(Func_t p_Func)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	m_Functions.push_back(p_Func);
}

void Dispatcher::Execute()
{
	FuncVec_t l_Functions;
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_Functions.swap(l_Functions);
	}

	for (auto& l_Function : l_Functions)
	{
		l_Function();
	}
}

}