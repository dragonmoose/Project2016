#pragma once
#include "DllExport.h"
#include "EventManager.h"
#include <string>
#include <memory>

namespace Hawk
{
	class Duration;

	class HAWK_DLL_EXPORT SystemBase
	{
	public:
		SystemBase();
		virtual ~SystemBase();

		void InternalInitialize(std::unique_ptr<EventManager>&& p_EventManager);
		virtual void Initialize();
		virtual void RegisterEvents(EventManager& p_EventManager);
		virtual std::string GetName() const = 0;

		void InternalUpdate(const Duration& p_Duration);
		virtual void Update(const Duration& p_Duration);

		SystemBase(const SystemBase&) = delete;
		SystemBase& operator=(const SystemBase&) = delete;

	protected:
		template<class T>
		void SendEvent(const T& p_Event)
		{
			m_EventManager->Send<T>(p_Event);
		}

	private:
		std::unique_ptr<EventManager> m_EventManager;
	};
}