#pragma once
#include "Base/Module.h"
#include "Base/SystemEvents.h"
#include <memory>
#include <string>

namespace Hawk {

class EventManager;

namespace Gfx {

class IRenderingAPI;
class DebugTextSubModule;

class RenderingModule final : public Module
{
public:
	RenderingModule();
	~RenderingModule();

	std::string GetName() const override;

	void Initialize() override;
	void RegisterEvents(EventManager& p_EventManager) override;

#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
#endif

	void Update(const Duration& p_Duration) override;

	RenderingModule(const RenderingModule&) = delete;
	RenderingModule& operator=(const RenderingModule&) = delete;

private:
	void OnWindowManipulated();
	void OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight, bool p_bMinimized);

	void SetFullscreenState(bool p_bState);
	bool ShouldRender() const;

	std::shared_ptr<IRenderingAPI> m_API;
	std::shared_ptr<DebugTextSubModule> m_DebugTextSubModule;
	bool m_bWindowTransition;
	bool m_bWindowMinimized;
};

}

}