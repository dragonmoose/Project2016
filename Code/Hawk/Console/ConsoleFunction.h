#pragma once
#ifdef HAWK_DEBUG
#include "Logger.h"
#include "Threading/Dispatcher.h"
#include <boost/lexical_cast.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

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

#pragma warning (push)
#pragma warning (disable:4100) // Compiler flags p_Args as being unreferenced, although it isn't
template<class TypeList, class Func, class Object, std::size_t... Index>
void DispatchMemberFunction(
	Func p_Func,
	Object* p_Object,
	const std::vector<std::string>& p_Args,
	std::index_sequence<Index...>,
	Dispatcher* p_Dispatcher,
	bool p_bRequireArgs)
{
	if (p_bRequireArgs)
	{
		p_Dispatcher->Post(
			std::bind(p_Func, p_Object, boost::lexical_cast<boost::mpl::at_c<TypeList, Index>::type>(p_Args[Index])...)
		);
	}
	else
	{
		p_Dispatcher->Post(
			std::bind(p_Func, p_Object, LexicalCast_Default<boost::mpl::at_c<TypeList, Index>::type>(p_Args[Index])...)
		);
	}
}
#pragma warning (pop)

template<class Object, class... Args>
class MemberConsoleFunction final : public IConsoleFunction
{
public:
	MemberConsoleFunction(Object* p_Object, void(Object::*p_Func)(Args...), Dispatcher* p_Dispatcher, const std::string& p_Name, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs)
	: IConsoleFunction(p_Dispatcher, p_Name, p_Desc, p_ArgsDesc, p_bRequireArgs, sizeof...(Args))
	, m_Object(p_Object)
	, m_Func(p_Func)
	{
	}

	void Call(const std::vector<std::string>& p_Args) const override
	{
		using Index_Seq = std::make_index_sequence<sizeof...(Args)>;
		DispatchMemberFunction<TypeList>(m_Func, m_Object, p_Args, Index_Seq(), m_Dispatcher, RequiresArgs());
	}

private:
	void(Object::*m_Func)(Args...);
	using TypeList = boost::mpl::vector<std::decay_t<Args>...>;
	Object* m_Object;
};

#pragma warning (push)
#pragma warning (disable:4100) // Compiler flags p_Args as being unreferenced, although it isn't
template<class TypeList, class Func, std::size_t... Index>
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
			std::bind(p_Func, boost::lexical_cast<boost::mpl::at_c<TypeList, Index>::type>(p_Args[Index])...)
		);
	}
	else
	{
		p_Dispatcher->Post(
			std::bind(p_Func, LexicalCast_Default<boost::mpl::at_c<TypeList, Index>::type>(p_Args[Index])...)
		);
	}
}
#pragma warning (pop)

template<class... Args>
class FreeConsoleFunction final : public IConsoleFunction
{
public:
	FreeConsoleFunction(void(*p_Func)(Args...), Dispatcher* p_Dispatcher, const std::string& p_Name, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs)
	: IConsoleFunction(p_Dispatcher, p_Name, p_Desc, p_ArgsDesc, p_bRequireArgs, sizeof...(Args))
	, m_Func(p_Func)
	{
	}

	void Call(const std::vector<std::string>& p_Args) const override
	{
		using Index_Seq = std::make_index_sequence<sizeof...(Args)>;
		DispatchFreeFunction<TypeList>(m_Func, p_Args, Index_Seq(), m_Dispatcher, RequiresArgs());
	}

private:
	void(*m_Func)(Args...);
	using TypeList = boost::mpl::vector<std::decay_t<Args>...>;
};

}
}
#endif