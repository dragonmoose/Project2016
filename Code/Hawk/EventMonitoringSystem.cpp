#include "pch.h"
#include "EventMonitoringSystem.h"
#include "EventManager.h"
#include "InputEvents.h"

namespace Hawk {

std::string EventMonitoringSystem::GetName() const
{
	return "EventMonitoringSystem";
}

void EventMonitoringSystem::RegisterEvents(EventManager& p_EventManager)
{
	p_EventManager.Register<InputEvents::KeyUpEvent>([this](const InputEvents::KeyUpEvent& p_Event) { LOG_SYS("KeyUp: " << p_Event.m_Code, Debug); });
	p_EventManager.Register<InputEvents::KeyDownEvent>([this](const InputEvents::KeyDownEvent& p_Event) { LOG_SYS("KeyDown: " << p_Event.m_Code, Debug); });
}

}