#pragma once
#include "Base/Module.h"

namespace Hawk {

class EventManager;

namespace Gfx {

class RenderingModule : public Module
{
public:
	void Initialize() override;
	void RegisterEvents(EventManager& p_EventManager) override;

	RenderingModule(const RenderingModule&) = delete;
	RenderingModule& operator=(const RenderingModule&) = delete;
};

}

}