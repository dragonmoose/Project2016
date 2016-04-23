#include "pch.h"
#include "FPSTrackerSubModule.h"
#include "DebugTextSubModule.h"
#include "Util/Algorithm.h"
#include <sstream>
#include <iomanip>

namespace Hawk {
namespace Gfx {

FPSTrackerSubModule::FPSTrackerSubModule(std::shared_ptr<DebugTextSubModule> p_DebugTextSubModule)
: m_DebugTextSubModule(p_DebugTextSubModule)
{
	Reset();
}

std::string FPSTrackerSubModule::GetName() const
{
	return "fps";
}

void FPSTrackerSubModule::Update(const Duration& /*p_Duration*/)
{
	Time l_CurrTime;
	l_CurrTime.SetToNow();
	m_Samples[m_iCurrSampleIndex] = (l_CurrTime - m_PrevTime).GetSecondsFloat();
	m_PrevTime = l_CurrTime;

	m_iCurrSampleIndex++;
	if (m_iCurrSampleIndex >= sc_iNumSamples)
	{
		m_iCurrSampleIndex = 0;
	}

	float l_fAvgDeltaSec = hwk::average(m_Samples);
	float l_fAvgDeltaMSec = l_fAvgDeltaSec * 1000.0f;
	float l_fMinDeltaMSec = *std::min_element(m_Samples.cbegin(), m_Samples.cend()) * 1000.0f;
	float l_fMaxDeltaMSec = *std::max_element(m_Samples.cbegin(), m_Samples.cend()) * 1000.0f;
	int32 l_iAvgFPS = static_cast<int32>(std::round(1.0f / l_fAvgDeltaSec));

	std::ostringstream l_Stream;
	l_Stream << std::setprecision(3) << l_iAvgFPS << " dt: " << l_fAvgDeltaMSec << " ms [" << l_fMinDeltaMSec << " .. " << l_fMaxDeltaMSec << "]";
	m_DebugTextSubModule->UpdateValue("FPS", l_Stream.str(), "gfx");
}

#ifdef HAWK_DEBUG
void FPSTrackerSubModule::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("fps.reset", this, &FPSTrackerSubModule::Reset, "Resets min/max values", "");
}
#endif

void FPSTrackerSubModule::Reset()
{
	m_iCurrSampleIndex = 0;
	m_Samples.assign(0);
	m_PrevTime = Time::Now();
}

}
}