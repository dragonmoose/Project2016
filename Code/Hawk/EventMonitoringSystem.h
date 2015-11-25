#pragma once

#include "SystemBase.h"

namespace Hawk {

class HAWK_DLL_EXPORT EventMonitoringSystem : public SystemBase
{
public:
	std::string GetName() const override;
	void RegisterEvents(EventManager& p_EventManager) override;
};

}