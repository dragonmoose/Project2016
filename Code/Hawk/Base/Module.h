#pragma once
#include "System/DllExport.h"
#include "Threading/Dispatcher.h"
#include "Events/EventManager.h"
#include "System/Duration.h"
#include "System/Types.h"
#ifdef HAWK_DEBUG
#include "Console/ScopedConsoleCommands.h"
#endif
#include <atomic>
#include <string>
#include <memory>

namespace Hawk
{
	class Duration;
	class SubModule;
	class ConsoleCommandManager;

	class HAWK_DLL_EXPORT Module
	{
	public:
		Module();
		virtual ~Module();

		Module(const Module&) = delete;
		Module& operator=(const Module&) = delete;

		virtual std::string GetName() const = 0;
		ModuleID_t GetID() const;

		void _Initialize(std::unique_ptr<EventManager> p_EventManager, std::shared_ptr<Dispatcher> p_Dispatcher);
		virtual void Initialize();

#ifdef HAWK_DEBUG
		void _InitializeConsole(std::shared_ptr<ConsoleCommandManager>& p_ConsoleCommandManager);
		virtual void InitializeConsole(ScopedConsoleCommands* p_Console);
		void DebugPrint() const;
#endif
		const std::string& GetLogDesc() const;

		virtual void RegisterEvents(EventManager& p_EventManager);

		void _Update(const Duration& p_Duration);
		virtual void Update(const Duration& p_Duration);

		void SetPaused(bool p_bPaused);
		bool IsPaused() const;

		template<class T>
		void SendEvent(const T& p_Event)
		{
			m_EventManager->Send<T>(p_Event);
		}

	protected:
		enum class FixedTimeStepDecl
		{
			FramesPerSecond,
			SecondsPerFrame
		};
		void SetFixedTimeStep(float p_fValue, FixedTimeStepDecl p_Decl);

		void AddSubModule(std::shared_ptr<SubModule> p_SubModule);
		bool IsInitialized() const;
		EventManager& GetEventManager();

	private:
		std::unique_ptr<EventManager> m_EventManager;
		std::shared_ptr<Dispatcher> m_Dispatcher;
		std::atomic_bool m_bPaused;

		static std::atomic_uint s_uiNextModuleID;
		ModuleID_t m_ID;
		Duration m_TimePerFrame;
		Duration m_AccumulatedTime;

		using SubModules_t = std::vector<std::shared_ptr<SubModule>>;
		SubModules_t m_SubModules;
		bool m_bInitialized;
		std::string m_LogDesc;

#ifdef HAWK_DEBUG
		std::unique_ptr<ScopedConsoleCommands> m_Console;
#endif
	};
}