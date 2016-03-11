#pragma once
#include "Module.h"
#include "System/DllExport.h"
#include <string>

namespace Hawk
{
	class Duration;
	class EventManager;
	class ScopedConsoleCommands;

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
		virtual void InitializeConsole(ScopedConsoleCommands* p_Console);
#endif
		const std::string& GetLogDesc() const;

		virtual void RegisterEvents(EventManager& p_EventManager);
		virtual void Update(const Duration& p_Duration);

	protected:
		template<class T>
		void SendEvent(const T& p_Event)
		{
			m_pParentModule->SendEvent<T>(p_Event);
		}

	private:
		Module* m_pParentModule;
		std::string m_LogDesc;
	};
}