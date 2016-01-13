#include "pch.h"
#include "RenderingModule.h"
#include "IRenderingAPI.h"
#include "Debug/Assert.h"

namespace Hawk {
namespace Gfx {

RenderingModule::RenderingModule(std::unique_ptr<IRenderingAPI> p_RenderingAPI)
: m_API(std::move(p_RenderingAPI))
{
	ASSERT(m_API, "Rendering API is null - RenderingModule should not have been created");
}

std::string RenderingModule::GetName() const
{
	return "RenderingModule";
}

void RenderingModule::Initialize()
{
	try
	{
		if (Config::Instance().Get("gfx.setFixedFPS", false))
		{
			SetFixedTimeStep(Config::Instance().Get("gfx.fixedFPS", 60), Module::FixedTimeStepDecl::FramesPerSecond);
		}
		m_API->Initialize();

		LOGM("Init done", Info);
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION(e, "gfx", Fatal);
	}
}

void RenderingModule::RegisterEvents(EventManager& p_EventManager)
{
}

void RenderingModule::Update(const Duration& p_Duration)
{
	LOGM("Update", Trace);
}

}
}