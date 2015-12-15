#pragma once
#include "System/DllExport.h"
#include "Events/EventManager.h"
#ifdef HAWK_DEBUG
#include "Console/ModuleConsoleManager.h"
#endif
#include <string>
#include <memory>

namespace Hawk
{
	class Duration;
	class ConsoleFunctionManager;

	class HAWK_DLL_EXPORT Module
	{
	public:
		Module();
		virtual ~Module();

		Module(const Module&) = delete;
		Module& operator=(const Module&) = delete;

		template<class T>
		static std::shared_ptr<T> CreateInstance() { return std::make_shared<T>(); }
		virtual std::string GetName() const = 0;

		void InternalInitialize(std::unique_ptr<EventManager> p_EventManager);

#ifdef HAWK_DEBUG
		virtual void RegisterConsole(ModuleConsoleManager& p_ConsoleFunctionManager);
#endif

		virtual void Initialize();
		virtual void RegisterEvents(EventManager& p_EventManager);

		void InternalUpdate(const Duration& p_Duration);
		virtual void Update(const Duration& p_Duration);

		void UpdateConsole();

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
#endif
	};
}