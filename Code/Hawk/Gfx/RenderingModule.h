#pragma once
#include "Base/Module.h"
#include <memory>
#include <string>

namespace Hawk {

class EventManager;

namespace Gfx {

class IRenderingAPI;
class DebugTextSubModule;

class RenderingModule : public Module
{
public:
	RenderingModule();

	std::string GetName() const override;

	void Initialize() override;
	void RegisterEvents(EventManager& p_EventManager) override;

#ifdef HAWK_DEBUG
	void InitializeConsole() override;
#endif

	void Update(const Duration& p_Duration) override;

	RenderingModule(const RenderingModule&) = delete;
	RenderingModule& operator=(const RenderingModule&) = delete;

private:
	void OnWindowSizeChanged(unsigned int p_uiWidth, unsigned int p_uiHeight, bool p_bIsVisible);

	std::shared_ptr<IRenderingAPI> m_API;
	std::shared_ptr<DebugTextSubModule> m_DebugTextSubModule;
	bool m_bRender;
};

}

}