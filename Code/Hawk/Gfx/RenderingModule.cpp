#include "pch.h"
#include "RenderingModule.h"
#include "DebugTextSubModule.h"
#include "FPSTrackerSubModule.h"
#include "Base/WindowManager.h"
#include "Debug/Assert.h"
#ifdef HAWK_RENDERER_D3D12
#include "D3D12/D3D12API.h"
#endif
#include <functional>

namespace Hawk {
namespace Gfx {

RenderingModule::RenderingModule()
: m_bWindowTransition(false)
, m_bWindowMinimized(false)
{
#ifdef HAWK_RENDERER_D3D12
	m_API = std::make_shared<D3D12::D3D12API>();
#endif
	m_DebugTextSubModule = std::make_shared<DebugTextSubModule>(m_API);
	AddSubModule(std::make_shared<FPSTrackerSubModule>(m_DebugTextSubModule));
	AddSubModule(m_DebugTextSubModule);
}

std::string RenderingModule::GetName() const
{
	return "RenderingModule";
}

void RenderingModule::Initialize()
{
	if (Config::Instance().Get("gfx.setFixedFPS", false))
	{
		SetFixedTimeStep(Config::Instance().Get("gfx.fixedFPS", 60), Module::FixedTimeStepDecl::FramesPerSecond);
	}
	m_API->Initialize();
}

void RenderingModule::RegisterEvents(EventManager& p_EventManager)
{
	p_EventManager.Register<WindowSizeChangedEvent>(std::bind(&RenderingModule::OnWindowSizeChanged, this, std::placeholders::_1));
	p_EventManager.Register<WindowManipulatedEvent>([&](const WindowManipulatedEvent&) { m_bWindowTransition = true; });
}

#ifdef HAWK_DEBUG
void RenderingModule::InitializeConsole()
{
	RegisterConsole("gfx.listDevices", m_API.get(), &IRenderingAPI::CmdListDevices, "Lists the available hardware adapters", "");
	RegisterConsole("gfx.setFullscreen", this, &RenderingModule::SetFullscreenState, "Sets fullscreen/windowed", "[1=fullscreen, 0=windowed");
}
#endif

void RenderingModule::Update(const Duration& p_Duration)
{
	if (ShouldRender())
	{
		m_API->Render();
	}
}

void RenderingModule::SetFullscreenState(bool p_bState)
{
	m_bWindowTransition = true;
	m_API->SetFullscreenState(p_bState);
}

void RenderingModule::OnWindowSizeChanged(const WindowSizeChangedEvent& p_Event)
{
	LOGM("Window size changed event received. Width=" << p_Event.m_uiWidth << " Height=" << p_Event.m_uiHeight << " Minimized=" << p_Event.m_bMinimized, Info);
	m_bWindowMinimized = p_Event.m_bMinimized;
	m_API->OnWindowSizeChanged(p_Event.m_uiWidth, p_Event.m_uiHeight);
	m_bWindowTransition = false;
}

bool RenderingModule::ShouldRender() const
{
	return !(m_bWindowTransition || m_bWindowMinimized);
}

}
}