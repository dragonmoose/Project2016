#pragma once
#include "Hawk/SystemBase.h"
#include "Hawk/Logger.h"
#include "Hawk/Duration.h"
#include <string>

class TestSystem2 : public Hawk::SystemBase
{
public:
	std::string GetName() const override { return "TestSystem2"; }
	virtual void Update(const Hawk::Duration& p_Duration) {}
};