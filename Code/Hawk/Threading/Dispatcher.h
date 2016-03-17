#pragma once
#include "System/DllExport.h"
#include "Threading/Mutex.h"
#include <functional>
#include <vector>

namespace Hawk {

class HAWK_DLL_EXPORT Dispatcher final
{
public:
	using Func = std::function<void(void)>;

	Dispatcher() = default;
	Dispatcher(const std::string& p_ThreadName);
	Dispatcher(const Dispatcher&) = delete;
	Dispatcher& operator=(const Dispatcher&) = delete;

	void Post(Func p_Func);
	void Execute();

private:
	using FuncVec = std::vector<Func>;

	FuncVec m_Functions;
	mutable Mutex m_Mutex;
};
}