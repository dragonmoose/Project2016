#pragma once
#include "Hawk/SystemBase.h"
#include "Hawk/Logger.h"
#include "Hawk/Duration.h"
#include <string>

class TestSystem : public Hawk::SystemBase
{
public:
	std::string GetName() const override { return "TestSystem";  }
	virtual void Update(const Hawk::Duration& p_Duration) { LOG_DEBUG("TestSystem update called"); }
};
