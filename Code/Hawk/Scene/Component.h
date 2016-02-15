#pragma once

namespace Hawk {

class Duration;

class Component
{
public:
	virtual void Initialize();
	virtual void Update(const Duration& p_Duration);
};
}