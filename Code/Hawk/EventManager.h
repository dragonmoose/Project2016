#pragma once
#include "IEvent.h"
#include "EventRouter.h"
#include "EventCollector.h"
#include "EventHandler.h"
#include "DllExport.h"
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <string>
#include <functional>
#include <mutex>

namespace Hawk {

class HAWK_DLL_EXPORT EventManager final
{
public:
	template<class T>
	void Register(std::function<void(const T&)> p_Func)
	{
		const std::type_index& l_TypeIndex = std::type_index(typeid(T));
		m_Events[l_TypeIndex] = std::make_unique<EventHandler<T>>(p_Func);
		EventRouter::Register(l_TypeIndex, &m_Collector);
	}

	void HandleQueued();

	template<class T>
	void Send(const T& p_Event)
	{
		EventRouter::Dispatch<T>(p_Event);
	}
	
private:
	typedef std::unordered_map<std::type_index, std::unique_ptr<EventHandlerBase>> EventHandlerMap_t;
	EventHandlerMap_t m_Events;
	EventCollector m_Collector;
};

}