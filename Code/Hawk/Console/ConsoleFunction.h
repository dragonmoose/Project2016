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

class IConsoleFunction
{
public:
	IConsoleFunction() = default;
	virtual ~IConsoleFunction() = default;
	IConsoleFunction(const IConsoleFunction&) = delete;
	IConsoleFunction& operator=(const IConsoleFunction&) = delete;
	bool _Call(const std::vector<std::string>& p_Args) { return Call(p_Args); }

protected:
	virtual bool Call(const std::vector<std::string>& p_Args) = 0;
};
	
template<class TypeList_t, class Func, class Object_t, std::size_t... Index>
bool DispatchMemberFunction(
	Func p_Func,
	Object_t* p_Object,
	const std::vector<std::string>& p_Args,
	std::index_sequence<Index...>,
	Dispatcher* p_Dispatcher)
{
	try
	{
		p_Dispatcher->Post(
			std::bind(p_Func, p_Object, boost::lexical_cast<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
	catch (boost::bad_lexical_cast&)
	{
		LOG("Failed to parse arguments when attempting to dispatch member console function", "console", Info);
	}
	return false;
}

template<class TypeList_t, class Func, std::size_t... Index>
bool DispatchFreeFunction(
	Func p_Func,
	const std::vector<std::string>& p_Args,
	std::index_sequence<Index...>,
	Dispatcher* p_Dispatcher)
{
	try
	{
		p_Dispatcher->Post(
			std::bind(p_Func, boost::lexical_cast<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
			);
	}
	catch (boost::bad_lexical_cast&)
	{
		LOG("Failed to parse arguments when attempting to dispatch free console function", "console", Info);
	}
	return false;
}

template<class Object_t, class... Args_t>
class MemberConsoleFunction final : public IConsoleFunction
{
public:
	MemberConsoleFunction(Object_t* p_Object, void(Object_t::*p_Func)(Args_t...), Dispatcher* p_Dispatcher)
	: m_Object(p_Object)
	, m_Func(p_Func)
	, m_Dispatcher(p_Dispatcher)
	{
	}

	bool Call(const std::vector<std::string>& p_Args) override
	{
		constexpr size_t c_NumArgs = sizeof...(Args_t);
		if (p_Args.size() == c_NumArgs)
		{
			using Index_Seq_t = std::make_index_sequence<c_NumArgs>;
			return DispatchMemberFunction<TypeList_t>(m_Func, m_Object, p_Args, Index_Seq_t(), m_Dispatcher);
		}
		else
		{
			LOG("Wrong number of arguments passed to member function: " << p_Args.size() << "/" << c_NumArgs, "console", Info);
		}
		return false;
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
	FreeConsoleFunction(void(*p_Func)(Args_t...), Dispatcher* p_Dispatcher)
	: m_Func(p_Func)
	, m_Dispatcher(p_Dispatcher)
	{
	}

	bool Call(const std::vector<std::string>& p_Args) override
	{
		constexpr size_t c_NumArgs = sizeof...(Args_t);
		if (p_Args.size() == c_NumArgs)
		{
			using Index_Seq_t = std::make_index_sequence<c_NumArgs>;
			return DispatchFreeFunction<TypeList_t>(m_Func, p_Args, Index_Seq_t(), m_Dispatcher);
		}
		else
		{
			LOG("Wrong number of arguments passed to free function: " << p_Args.size() << "/" << c_NumArgs, "console", Info);
		}
		return false;
	}

private:
	void(*m_Func)(Args_t...);
	using TypeList_t = boost::mpl::vector<std::decay_t<Args_t>...>;
	Dispatcher* m_Dispatcher;
};

}
#endif