#pragma once
#include "System/DllExport.h"
#include "System/Dispatcher.h"
#include "Events/EventManager.h"
#include "System/Types.h"
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#endif
#include <atomic>
#include <string>
#include <memory>

namespace Hawk
{
	class Duration;

	class HAWK_DLL_EXPORT Module
	{
	public:
		Module();
		virtual ~Module() = default;

		Module(const Module&) = delete;
		Module& operator=(const Module&) = delete;

		virtual std::string GetName() const = 0;

		template<class T>
		static std::unique_ptr<T> CreateInstance() { return std::make_unique<T>(); }

		ModuleID GetID() const;

		void _Initialize(std::unique_ptr<EventManager> p_EventManager, std::shared_ptr<Dispatcher>& p_Dispatcher);
		virtual void Initialize();

#ifdef HAWK_DEBUG
		void _InitializeConsole(std::shared_ptr<ConsoleCommandManager>& p_ConsoleCommandManager);
		virtual void InitializeConsole();
		std::string GetLogDesc() const;
#endif

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

		template<class Object_t, class... Args_t>
		void RegisterConsole(const std::string& p_Name, Object_t* p_Object, void(Object_t::*p_Func)(Args_t...))
		{
#ifdef HAWK_DEBUG
			m_ConsoleCommandManager->Register(p_Name, p_Object, p_Func, m_Dispatcher.get());
#endif
		}

	private:
		std::unique_ptr<EventManager> m_EventManager;
		std::shared_ptr<Dispatcher> m_Dispatcher;
		bool m_bPaused;

		static std::atomic_uint s_uiNextModuleID;
		ModuleID m_ID;

#ifdef HAWK_DEBUG
		std::shared_ptr<ConsoleCommandManager> m_ConsoleCommandManager;
#endif
	};
}