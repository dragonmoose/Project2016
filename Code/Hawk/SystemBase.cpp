#include "pch.h"
#include "SystemBase.h"
#include "Duration.h"

namespace Hawk {

SystemBase::SystemBase()
{
}

SystemBase::~SystemBase()
{
}

void SystemBase::InternalInitialize()
{
	LOG_INFO("Initializing system: " << GetName());
	Initialize();
}

void SystemBase::Initialize()
{
}

void SystemBase::RegisterEvents()
{
}

void SystemBase::UnregisterEvents()
{
}

void SystemBase::InternalUpdate(const Duration& p_Duration)
{
	LOG_DEBUG("Updating system: " << GetName() << " DeltaTime: " << p_Duration);
	Update(p_Duration);
}

void SystemBase::Update(const Duration& p_Duration)
{
}

}
