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
	IConsoleFunction(const std::string& p_Name, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs)
	: m_Name(p_Name)
	, m_Desc(p_Desc)
	, m_ArgsDesc(p_ArgsDesc)
	, m_bRequireArgs(p_bRequireArgs)
	{
	}

	virtual ~IConsoleFunction() = default;
	IConsoleFunction(const IConsoleFunction&) = delete;
	IConsoleFunction& operator=(const IConsoleFunction&) = delete;

	void _Call(const std::vector<std::string>& p_Args) { Call(p_Args); }

	const std::string& GetName() const { return m_Name; }
	const std::string& GetDesc() const { return m_Desc; }
	const std::string& GetArgsDesc() const { return m_ArgsDesc; }

protected:
	virtual void Call(const std::vector<std::string>& p_Args) = 0;
	bool m_bRequireArgs;

private:
	std::string m_Name;
	std::string m_Desc;
	std::string m_ArgsDesc;
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

inline std::vector<std::string> FillFuncArgs(const std::vector<std::string>& p_Args, size_t p_NumArgs)
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
	Dispatcher* p_Dispatcher,
	bool p_bRequireArgs)
{
	if (p_bRequireArgs)
	{
		p_Dispatcher->Post(
			std::bind(p_Func, p_Object, LexicalCastWithDefault<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
	else
	{
		p_Dispatcher->Post(
			std::bind(p_Func, p_Object, boost::lexical_cast<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
}

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
			std::bind(p_Func, LexicalCastWithDefault<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
	else
	{
		p_Dispatcher->Post(
			std::bind(p_Func, boost::lexical_cast<boost::mpl::at_c<TypeList_t, Index>::type>(p_Args[Index])...)
		);
	}
}

template<class Object_t, class... Args_t>
class MemberConsoleFunction final : public IConsoleFunction
{
public:
	MemberConsoleFunction(Object_t* p_Object, void(Object_t::*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_Name, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs)
	: IConsoleFunction(p_Name, p_Desc, p_ArgsDesc, p_bRequireArgs)
	, m_Object(p_Object)
	, m_Func(p_Func)
	, m_Dispatcher(p_Dispatcher)
	{
	}

	void Call(const std::vector<std::string>& p_Args) override
	{
		constexpr size_t c_NumArgs = sizeof...(Args_t);
		using Index_Seq_t = std::make_index_sequence<c_NumArgs>;

		if (m_bRequireArgs)
		{
			if (p_Args.size() == c_NumArgs)
			{
				DispatchMemberFunction<TypeList_t>(m_Func, m_Object, p_Args, Index_Seq_t(), m_Dispatcher, m_bRequireArgs);
			}
			else
			{
				std::cout << "Invalid number of arguments passed: " << p_Args.size() << "/" << c_NumArgs << "\n";
				if (!GetArgsDesc().empty())
				{
					std::cout << "Usage: " << GetName() << " " << GetArgsDesc() << "\n";
				}
			}
		}
		else
		{
			DispatchMemberFunction<TypeList_t>(m_Func, m_Object, FillFuncArgs(p_Args, c_NumArgs), Index_Seq_t(), m_Dispatcher, m_bRequireArgs);
		}
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
	FreeConsoleFunction(void(*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_Name, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs)
	: IConsoleFunction(p_Name, p_Desc, p_ArgsDesc, p_bRequireArgs)
	, m_Func(p_Func)
	, m_Dispatcher(p_Dispatcher)
	{
	}

	void Call(const std::vector<std::string>& p_Args) override
	{
		constexpr size_t c_NumArgs = sizeof...(Args_t);
		using Index_Seq_t = std::make_index_sequence<c_NumArgs>;

		if (m_bRequireArgs)
		{
			if (p_Args.size() == c_NumArgs)
			{
				DispatchFreeFunction<TypeList_t>(m_Func, p_Args, Index_Seq_t(), m_Dispatcher, m_bRequireArgs);
			}
			else
			{
				std::cout << "Invalid number of arguments passed: " << p_Args.size() << "/" << c_NumArgs << "\n";
				if (!GetArgsDesc().empty())
				{
					std::cout << "Usage: " << GetName() << " " << GetArgsDesc() << "\n";
				}
			}
		}
		else
		{
			DispatchFreeFunction<TypeList_t>(m_Func, FillFuncArgs(p_Args, c_NumArgs), Index_Seq_t(), m_Dispatcher, m_bRequireArgs);
		}
	}

private:
	void(*m_Func)(Args_t...);
	using TypeList_t = boost::mpl::vector<std::decay_t<Args_t>...>;
	Dispatcher* m_Dispatcher;
};

}
}
#endif