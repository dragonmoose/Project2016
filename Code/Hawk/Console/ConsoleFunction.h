#pragma once
#include "System/Exception.h"
#include "Console/Logger.h"
#include <string>
#include <functional>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>

namespace Hawk {

class Module;

template<class TypeList_t, class Func, std::size_t... Index>
void CallFunction(Func p_Func, const std::vector<std::string>& p_Args, std::index_sequence<Index...>)
{
	try
	{
		p_Func(boost::lexical_cast<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...);
	}
	catch (boost::bad_lexical_cast&)
	{
		THROW("Failed to parse arguments when attempting to invoke console function");
	}
}

class ConsoleFunctionBase
{
public:
	virtual ~ConsoleFunctionBase() = default;
	ConsoleFunctionBase(const ConsoleFunctionBase&) = delete;
	ConsoleFunctionBase& operator=(const ConsoleFunctionBase&) = delete;
	void Call(const std::vector<std::string>& p_Args) { CallImpl(p_Args); }

protected:
	virtual void CallImpl(const std::vector<std::string>& p_Args) = 0;
};

template<class Ret_t, class ObjPtr_t, class... Args_t>
class ConsoleFunction final : public ConsoleFunctionBase
{
public:
	ConsoleFunction(ObjPtr_t* p_ObjPtr, Ret_t(ObjPtr_t::*p_Func)(Args_t...))
	: m_Func([p_ObjPtr, p_Func](Args_t... p_Args) { (p_ObjPtr->*p_Func)(p_Args...); })
	{
	}

	void CallImpl(const std::vector<std::string>& p_Args) override
	{
		constexpr size_t c_NumArgs = sizeof...(Args_t);
		THROW_IF_NOT(p_Args.size() == c_NumArgs, "Invalid number of arguments passed. Required=" << c_NumArgs);

		using Index_Seq_t = std::make_index_sequence<c_NumArgs>;
		CallFunction<TypeList_t>(m_Func, p_Args, Index_Seq_t());
	}

private:
	std::function<Ret_t(Args_t...)> m_Func;
	using TypeList_t = boost::mpl::vector<std::decay_t<Args_t>...>;
};

}