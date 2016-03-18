#include "pch.h"
#include "RenderingModule.h"
#include "DebugTextSubModule.h"
#include "FPSTrackerSubModule.h"
#include "Base/WindowManager.h"
#if defined(HAWK_RENDERER_D3D12)
#include "D3D12/D3D12API.h"
#elif defined(HAWK_RENDERER_VULKAN)
#include "Vulkan/API.h"
#endif
#include <functional>

namespace Hawk {
namespace Gfx {

RenderingModule::RenderingModule()
: m_bWindowTransition(false)
, m_bWindowMinimized(false)
{
#if defined(HAWK_RENDERER_D3D12)
	m_API = std::make_shared<D3D12::D3D12API>();
#elif defined(HAWK_RENDERER_VULKAN)
	m_API = std::make_shared<Vulkan::API>();
#endif
	ASSERT(m_API, "No API");

	m_DebugTextSubModule = std::make_shared<DebugTextSubModule>(m_API);
	AddSubModule(std::make_shared<FPSTrackerSubModule>(m_DebugTextSubModule));
	AddSubModule(m_DebugTextSubModule);
}

RenderingModule::~RenderingModule()
{
	WindowManager::RegisterWindowManipulatedCallback(WindowManager::WindowManipulatedCallback());
	WindowManager::RegisterWindowSizeChanged(WindowManager::WindowSizeChangedCallback());
}

std::string RenderingModule::GetName() const
{
	return "RenderingModule";
}

void RenderingModule::Initialize()
{
	if (Config::Instance().Get("gfx.setFixedFPS", false))
	{
		SetFixedTimeStep(Config::Instance().Get("gfx.fixedFPS", 60.0f), Module::FixedTimeStepDecl::FramesPerSecond);
	}
	m_API->Initialize();

	WindowManager::RegisterWindowManipulatedCallback(std::bind(&RenderingModule::OnWindowManipulated, this));
	WindowManager::RegisterWindowSizeChanged(std::bind(&RenderingModule::OnWindowSizeChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void RenderingModule::RegisterEvents(EventManager& /*p_EventManager*/)
{
}

#ifdef HAWK_DEBUG
void RenderingModule::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("gfx.setFullscreen", this, &RenderingModule::SetFullscreenState, "Sets fullscreen/windowed", "[1=fullscreen, 0=windowed");
	m_API->InitializeConsole(p_Console);
}
#endif

void RenderingModule::Update(const Duration& /*p_Duration*/)
{
	if (ShouldRender())
	{
		m_API->Render();
	}
}

void RenderingModule::OnWindowManipulated()
{
	m_bWindowTransition = true;
}

void RenderingModule::OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight, bool p_bMinimized)
{
	LOGM("Window size changed event received. Width=" << p_uiWidth << " Height=" << p_uiHeight << " Minimized=" << p_bMinimized, Info);
	m_bWindowMinimized = p_bMinimized;
	m_API->OnWindowSizeChanged(p_uiWidth, p_uiHeight);
	m_bWindowTransition = false;
}

void RenderingModule::SetFullscreenState(bool p_bState)
{
	m_bWindowTransition = true;
	m_API->SetFullscreenState(p_bState);
}

bool RenderingModule::ShouldRender() const
{
	return !(m_bWindowTransition || m_bWindowMinimized);
}

}
}