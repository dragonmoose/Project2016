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
	IConsoleFunction(Dispatcher* p_Dispatcher, const std::string& p_Name, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs, std::size_t p_NumArgs)
	: m_Dispatcher(p_Dispatcher)
	, m_Name(p_Name)
	, m_Desc(p_Desc)
	, m_ArgsDesc(p_ArgsDesc)
	, m_bRequireArgs(p_bRequireArgs)
	, m_NumArgs(p_NumArgs)
	{
	}

	virtual ~IConsoleFunction() = default;
	IConsoleFunction(const IConsoleFunction&) = delete;
	IConsoleFunction& operator=(const IConsoleFunction&) = delete;

	void _Call(const std::vector<std::string>& p_Args) const { Call(p_Args); }

	const std::string& GetName() const { return m_Name; }
	const std::string& GetDesc() const { return m_Desc; }
	const std::string& GetArgsDesc() const { return m_ArgsDesc; }
	bool RequiresArgs() const { return m_bRequireArgs; }
	std::size_t GetNumArgs() const { return m_NumArgs; }

protected:
	virtual void Call(const std::vector<std::string>& p_Args) const = 0;
	Dispatcher* m_Dispatcher;

private:
	std::string m_Name;
	std::string m_Desc;
	std::string m_ArgsDesc;
	bool m_bRequireArgs;
	std::size_t m_NumArgs;
};

template<class Target, class Source>
Target LexicalCast_Default(Source p_Value)
{
	try
	{
		return boost::lexical_cast<Target>(p_Value);
	}
	catch (...)
	{
		return Target();
	}
}

template<class TypeList_t, class Func, class Object_t, std::size_t... Index>
void DispatchMemberFunction(
	Func p_Func,
	Object_t* p_Object,
	const std::vector<std::string>& p_Args,
	std::index_sequence<Index...>,
	Dispatcher* p_Dispatcher,
	bool p_bRequireArgs)
{
	if (p_bRequireArgs)
	{
		p_Dispatcher->Post(
			std::bind(p_Func, p_Object, boost::lexical_cast<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
	else
	{
		p_Dispatcher->Post(
			std::bind(p_Func, p_Object, LexicalCast_Default<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
}

template<class Object_t, class... Args_t>
class MemberConsoleFunction final : public IConsoleFunction
{
public:
	MemberConsoleFunction(Object_t* p_Object, void(Object_t::*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_Name, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs)
	: IConsoleFunction(p_Dispatcher, p_Name, p_Desc, p_ArgsDesc, p_bRequireArgs, sizeof...(Args_t))
	, m_Object(p_Object)
	, m_Func(p_Func)
	{
	}

	void Call(const std::vector<std::string>& p_Args) const override
	{
		using Index_Seq_t = std::make_index_sequence<sizeof...(Args_t)>;
		DispatchMemberFunction<TypeList_t>(m_Func, m_Object, p_Args, Index_Seq_t(), m_Dispatcher, RequiresArgs());
	}

private:
	void(Object_t::*m_Func)(Args_t...);
	using TypeList_t = boost::mpl::vector<std::decay_t<Args_t>...>;
	Object_t* m_Object;
};

template<class TypeList_t, class Func, std::size_t... Index>
void DispatchFreeFunction(
	Func p_Func,
	const std::vector<std::string>& p_Args,
	std::index_sequence<Index...>,
	Dispatcher* p_Dispatcher,
	bool p_bRequireArgs)
{
	if (p_bRequireArgs)
	{
		p_Dispatcher->Post(
			std::bind(p_Func, boost::lexical_cast<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
	else
	{
		p_Dispatcher->Post(
			std::bind(p_Func, LexicalCast_Default<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
}

template<class... Args_t>
class FreeConsoleFunction final : public IConsoleFunction
{
public:
	FreeConsoleFunction(void(*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_Name, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs)
	: IConsoleFunction(p_Dispatcher, p_Name, p_Desc, p_ArgsDesc, p_bRequireArgs, sizeof...(Args_t))
	, m_Func(p_Func)
	{
	}

	void Call(const std::vector<std::string>& p_Args) const override
	{
		using Index_Seq_t = std::make_index_sequence<sizeof...(Args_t)>;
		DispatchFreeFunction<TypeList_t>(m_Func, p_Args, Index_Seq_t(), m_Dispatcher, RequiresArgs());
	}

private:
	void(*m_Func)(Args_t...);
	using TypeList_t = boost::mpl::vector<std::decay_t<Args_t>...>;
};

}
}
#endif