#include "pch.h"
#include "SystemBase.h"
#include "Duration.h"

namespace Hawk {

SystemBase::SystemBase()
: m_bPaused(false)
{
}

SystemBase::~SystemBase()
{
}

void SystemBase::InternalInitialize(std::unique_ptr<EventManager>&& p_EventManager)
{
	m_EventManager = std::move(p_EventManager);
	LOG_INFO("Initializing system: " << GetName());
	Initialize();
	RegisterEvents(*m_EventManager);
}

void SystemBase::Initialize()
{
}

void SystemBase::RegisterEvents(EventManager& p_EventManager)
{
}

void SystemBase::InternalUpdate(const Duration& p_Duration)
{
	m_EventManager->HandleQueued();
	Update(p_Duration);
}

void SystemBase::Update(const Duration& p_Duration)
{
}

void SystemBase::SetPaused(bool p_bPaused)
{
	m_bPaused = p_bPaused;
}
bool SystemBase::IsPaused() const
{
	return m_bPaused;
}


}
