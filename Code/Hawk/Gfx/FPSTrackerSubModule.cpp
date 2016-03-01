#include "pch.h"
#include "FPSTrackerSubModule.h"
#include "DebugTextSubModule.h"
#include "Util/Algorithm.h"
#include <sstream>

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

void FPSTrackerSubModule::Update(const Duration& p_Duration)
{
	int32_t l_iFPS = static_cast<int32_t>(std::round(1.0f / (p_Duration.Get(Duration::Precision::MicroSecond) / 1000000.0f)));
	m_iMin = std::min(l_iFPS, m_iMin);
	m_iMax = std::max(l_iFPS, m_iMax);

	m_Samples[m_iCurrSampleIndex] = l_iFPS;
	m_iCurrSampleIndex++;
	if (m_iCurrSampleIndex >= sc_iNumSamples)
	{
		m_iCurrSampleIndex = 0;
	}

	std::ostringstream l_Stream;
	l_Stream << l_iFPS << " [" << m_iMin << ", " << m_iMax << "] avg: " << hwk::average(m_Samples) << " dt: " << p_Duration.Get(Duration::Precision::Millisecond) << " ms";
	m_DebugTextSubModule->UpdateValue("FPS", l_Stream.str(), "gfx");
}

#ifdef HAWK_DEBUG
void FPSTrackerSubModule::InitializeConsole()
{
	RegisterConsole("fps.reset", this, &FPSTrackerSubModule::Reset, "Resets min/max values", "");
}
#endif

void FPSTrackerSubModule::Reset()
{
	m_iCurrSampleIndex = 0;
	m_Samples.assign(sc_iNumSamples, 30);
	m_iMin = INT_MAX;
	m_iMax = INT_MIN;
}

}
}