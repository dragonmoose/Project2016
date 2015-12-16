#pragma once
#include "System/DllExport.h"
#include "Events/EventManager.h"
#ifdef HAWK_DEBUG
#include "Console/ModuleConsoleManager.h"
#include "Console/ModuleConsoleRouter.h"
#endif
#include <string>
#include <memory>

namespace Hawk
{
	class Duration;

	class HAWK_DLL_EXPORT Module
	{
	public:
		Module();
		virtual ~Module();

		Module(const Module&) = delete;
		Module& operator=(const Module&) = delete;

		template<class T>
		static std::unique_ptr<T> CreateInstance() { return std::make_unique<T>(); }
		virtual std::string GetName() const = 0;

		void _Initialize(std::unique_ptr<EventManager> p_EventManager);
		virtual void Initialize();

		void _InitializeConsole(std::shared_ptr<ModuleConsoleRouter>& p_ConsoleRouter);
		virtual void InitializeConsole();

		template<class Ret_t, class ObjPtr_t, class... Args_t>
		void RegisterConsole(const std::string& p_Name, ObjPtr_t* p_ObjPtr, Ret_t(ObjPtr_t::*p_Func)(Args_t...))
		{
#ifdef HAWK_DEBUG
			try
			{
				m_ConsoleManager->Register(p_Name, p_ObjPtr, p_Func);
				m_ConsoleRouter->Register(p_Name, m_ConsoleManager);
			}
			catch (Exception& e)
			{
				LOG("Failed to register console function. Exception: " << e.what(), "module", Fatal);
			}
#endif
		}

		virtual void RegisterEvents(EventManager& p_EventManager);

		void _Update(const Duration& p_Duration);
		virtual void Update(const Duration& p_Duration);

		void SetPaused(bool p_bPaused);
		bool IsPaused() const;

	protected:
		template<class T>
		void SendEvent(const T& p_Event)
		{
			m_EventManager->Send<T>(p_Event);
		}

	private:
		std::unique_ptr<EventManager> m_EventManager;
		bool m_bPaused;
		std::string m_Name;

#ifdef HAWK_DEBUG
		std::shared_ptr<ModuleConsoleManager> m_ConsoleManager;
		std::shared_ptr<ModuleConsoleRouter> m_ConsoleRouter;
#endif
	};
}