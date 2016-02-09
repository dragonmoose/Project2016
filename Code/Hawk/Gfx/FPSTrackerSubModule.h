#pragma once
#include "Base/SubModule.h"

namespace Hawk {
namespace Gfx {

class DebugTextSubModule;

class FPSTrackerSubModule : public SubModule
{
public:
	FPSTrackerSubModule(std::shared_ptr<DebugTextSubModule> p_DebugTextSubModule);
	std::string GetName() const override;
	void Update(const Duration& p_Duration);

#ifdef HAWK_DEBUG
	void InitializeConsole() override;
#endif

private:
	void Reset();

	std::shared_ptr<DebugTextSubModule> m_DebugTextSubModule;
	int m_iMin;
	int m_iMax;
	
	static const int sc_iNumValues = 10;
	int m_iCurrIndex;
	std::vector<int> m_Values;
};

}}