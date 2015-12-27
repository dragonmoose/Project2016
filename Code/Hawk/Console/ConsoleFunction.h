#pragma once
#ifdef HAWK_DEBUG
#include "Console/Logger.h"
#include "System/Dispatcher.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>

namespace Hawk {

namespace CF {

class IConsoleFunction
{
public:
	IConsoleFunction(const std::string& p_HelpText) : m_HelpText(p_HelpText) {}
	virtual ~IConsoleFunction() = default;
	IConsoleFunction(const IConsoleFunction&) = delete;
	IConsoleFunction& operator=(const IConsoleFunction&) = delete;
	void _Call(const std::vector<std::string>& p_Args) { Call(p_Args); }
	const std::string& GetHelpText() const { return m_HelpText; }

protected:
	virtual void Call(const std::vector<std::string>& p_Args) = 0;

private:
	std::string m_HelpText;
};

template<class R, class T>
R LexicalCastWithDefault(T p_Value)
{
	try
	{
		return boost::lexical_cast<R>(p_Value);
	}
	catch (...)
	{
		return R();
	}
}

inline std::vector<std::string> GetFuncArgs(const std::vector<std::string>& p_Args, size_t p_NumArgs)
{
	std::vector<std::string> l_Args(p_Args);
	while (l_Args.size() < p_NumArgs)
	{
		l_Args.push_back(std::string());
	}
	return l_Args;
}
	
template<class TypeList_t, class Func, class Object_t, std::size_t... Index>
void DispatchMemberFunction(
	Func p_Func,
	Object_t* p_Object,
	const std::vector<std::string>& p_Args,
	std::index_sequence<Index...>,
	Dispatcher* p_Dispatcher)
{
	p_Dispatcher->Post(
		std::bind(p_Func, p_Object, LexicalCastWithDefault<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
	);
}

template<class TypeList_t, class Func, std::size_t... Index>
void DispatchFreeFunction(
	Func p_Func,
	const std::vector<std::string>& p_Args,
	std::index_sequence<Index...>,
	Dispatcher* p_Dispatcher)
{
	p_Dispatcher->Post(
		std::bind(p_Func, LexicalCastWithDefault<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
	);
}

template<class Object_t, class... Args_t>
class MemberConsoleFunction final : public IConsoleFunction
{
public:
	MemberConsoleFunction(Object_t* p_Object, void(Object_t::*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_HelpText)
	: IConsoleFunction(p_HelpText)
	, m_Object(p_Object)
	, m_Func(p_Func)
	, m_Dispatcher(p_Dispatcher)
	{
	}

	void Call(const std::vector<std::string>& p_Args) override
	{
		constexpr size_t c_NumArgs = sizeof...(Args_t);
		using Index_Seq_t = std::make_index_sequence<c_NumArgs>;
		DispatchMemberFunction<TypeList_t>(m_Func, m_Object, GetFuncArgs(p_Args, c_NumArgs), Index_Seq_t(), m_Dispatcher);
	}

private:
	void(Object_t::*m_Func)(Args_t...);
	using TypeList_t = boost::mpl::vector<std::decay_t<Args_t>...>;
	Object_t* m_Object;
	Dispatcher* m_Dispatcher;
};

template<class... Args_t>
class FreeConsoleFunction final : public IConsoleFunction
{
public:
	FreeConsoleFunction(void(*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_HelpText)
	: IConsoleFunction(p_HelpText)
	, m_Func(p_Func)
	, m_Dispatcher(p_Dispatcher)
	{
	}

	void Call(const std::vector<std::string>& p_Args) override
	{
		constexpr size_t c_NumArgs = sizeof...(Args_t);
		using Index_Seq_t = std::make_index_sequence<c_NumArgs>;
		DispatchFreeFunction<TypeList_t>(m_Func, GetFuncArgs(p_Args, c_NumArgs), Index_Seq_t(), m_Dispatcher);
	}

private:
	void(*m_Func)(Args_t...);
	using TypeList_t = boost::mpl::vector<std::decay_t<Args_t>...>;
	Dispatcher* m_Dispatcher;
};

}
}
#endif