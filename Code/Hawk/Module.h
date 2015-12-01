#pragma once
#include "DllExport.h"
#include "EventManager.h"
#include <string>
#include <memory>

namespace Hawk
{
	class Duration;
	class ConsoleCmdManager;

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

		void InternalInitialize(std::unique_ptr<EventManager> p_EventManager, ConsoleCmdManager& p_ConsoleManager);
		virtual void Initialize();
		virtual void RegisterEvents(EventManager& p_EventManager);
		virtual void RegisterConsole(ConsoleCmdManager& p_ConsoleCmdManager);

		void InternalUpdate(const Duration& p_Duration);
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
	};
}