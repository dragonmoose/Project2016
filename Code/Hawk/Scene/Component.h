#pragma once

namespace Hawk {

class Duration;
class EventManager;

class Component
{
public:
	virtual void Initialize() {}
	virtual void RegisterEvents(EventManager& /*p_EventManager*/) {}
	virtual void Update(const Duration& /*p_Duration*/) {}

	virtual void AddedToScene() {}
	virtual void RemovedFromScene() {}
};
}