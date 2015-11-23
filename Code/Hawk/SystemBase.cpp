#include "pch.h"
#include "SystemBase.h"
#include "Duration.h"
#include <typeinfo>

namespace Hawk {

SystemBase::SystemBase()
: m_bPaused(false)
, m_Name(typeid(this).name())
{
}

SystemBase::~SystemBase()
{
}

void SystemBase::InternalInitialize(std::unique_ptr<EventManager>&& p_EventManager)
{
	LOG_SYS("Initializing system", Info);
	m_EventManager = std::move(p_EventManager);
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
	if (!IsPaused())
	{
		m_EventManager->HandleQueued();
		Update(p_Duration);
	}
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
