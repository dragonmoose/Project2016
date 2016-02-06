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
: m_bRender(true)
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
	WindowManager::RegisterWindowSizeChanged(std::bind(&RenderingModule::OnWindowSizeChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void RenderingModule::RegisterEvents(EventManager& p_EventManager)
{
}

#ifdef HAWK_DEBUG
void RenderingModule::InitializeConsole()
{
	RegisterConsole("gfx.listDevices", m_API.get(), &IRenderingAPI::CmdListDevices, "Lists the available hardware adapters", "");
	RegisterConsole("gfx.setFullscreen", m_API.get(), &IRenderingAPI::SetFullscreenState, "Sets fullscreen/windowed", "[1=fullscreen, 0=windowed");
}
#endif

void RenderingModule::Update(const Duration& p_Duration)
{
	m_API->Render();
}

void RenderingModule::OnWindowSizeChanged(unsigned int p_uiWidth, unsigned int p_uiHeight, bool p_bIsVisible)
{
	SetPaused(!p_bIsVisible);
	m_API->OnWindowSizeChanged(p_uiWidth, p_uiHeight);
}

}
}