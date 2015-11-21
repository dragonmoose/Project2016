#pragma once
#include "Hawk/SystemBase.h"
#include "Hawk/Logger.h"
#include "Hawk/Duration.h"
#include "TestEvent.h"
#include <string>

class TestSystem2 : public Hawk::SystemBase
{
public:
	std::string GetName() const override { return "TestSystem2"; }
	void Update(const Hawk::Duration& p_Duration) override
	{
		LOG_DEBUG(GetName() << " update: " << p_Duration);
		TestEvent l_Event(123);
	}
};