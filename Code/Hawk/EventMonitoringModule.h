#pragma once

#include "Module.h"

namespace Hawk {

class HAWK_DLL_EXPORT EventMonitoringModule : public Module
{
public:
	std::string GetName() const override;
	void RegisterEvents(EventManager& p_EventManager) override;
};

}