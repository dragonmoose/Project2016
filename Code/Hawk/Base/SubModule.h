#pragma once
#include "Module.h"
#include "System/DllExport.h"
#include <string>

namespace Hawk
{
	class Duration;
	class EventManager;

	class HAWK_DLL_EXPORT SubModule
	{
	public:
		SubModule();
		virtual ~SubModule() = default;

		SubModule(const SubModule&) = delete;
		SubModule& operator=(const SubModule&) = delete;

		void SetParentModule(Module* p_pModule);

		virtual std::string GetName() const = 0;
		void _Initialize();
		virtual void Initialize();

#ifdef HAWK_DEBUG
		virtual void InitializeConsole();
		const std::string& GetLogDesc() const;
#endif

		virtual void RegisterEvents(EventManager& p_EventManager);
		virtual void Update(const Duration& p_Duration);

	protected:
		template<class T>
		void SendEvent(const T& p_Event)
		{
			m_pParentModule->Send<T>(p_Event);
		}

		template<class Object_t, class... Args_t>
		void RegisterConsole(const std::string& p_Name, Object_t* p_Object, void(Object_t::*p_Func)(Args_t...), const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs = true)
		{
#ifdef HAWK_DEBUG
			m_pParentModule->RegisterConsole(p_Name, p_Object, p_Func, p_Desc, p_ArgsDesc, p_bRequireArgs);
#endif
		}

	private:
		Module* m_pParentModule;

#ifdef HAWK_DEBUG
		std::string m_LogDesc;
#endif
	};
}