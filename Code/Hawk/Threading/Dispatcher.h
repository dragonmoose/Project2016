#pragma once
#include "System/DllExport.h"
#include "Threading/Mutex.h"
#include <functional>
#include <vector>

namespace Hawk {

class HAWK_DLL_EXPORT Dispatcher final
{
public:
	using Func_t = std::function<void(void)>;

	Dispatcher() = default;
	Dispatcher(const std::string& p_ThreadName);
	Dispatcher(const Dispatcher&) = delete;
	Dispatcher& operator=(const Dispatcher&) = delete;

	void Post(Func_t p_Func);
	void Execute();

private:
	using FuncVec_t = std::vector<Func_t>;

	FuncVec_t m_Functions;
	mutable Mutex m_Mutex;
};
}