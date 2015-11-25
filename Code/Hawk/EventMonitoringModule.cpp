#include "pch.h"
#include "EventMonitoringModule.h"
#include "EventManager.h"
#include "InputEvents.h"

namespace Hawk {

std::string EventMonitoringModule::GetName() const
{
	return "EventMonitoring";
}

void EventMonitoringModule::RegisterEvents(EventManager& p_EventManager)
{
	p_EventManager.Register<InputEvents::KeyUpEvent>([this](const InputEvents::KeyUpEvent& p_Event) { LOG_MOD("KeyUp: " << p_Event, Debug); });
	p_EventManager.Register<InputEvents::KeyDownEvent>([this](const InputEvents::KeyDownEvent& p_Event) { LOG_MOD("KeyDown: " << p_Event, Debug); });
}

}