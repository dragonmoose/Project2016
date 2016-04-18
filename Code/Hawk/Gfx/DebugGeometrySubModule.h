#pragma once
#include "Base/SubModule.h"
#include "Color.h"
#include "Math/Vec3.h"
#include <vector>
#include <memory>

namespace Hawk {
namespace Gfx {

class IRenderingAPI;

class DebugGeometrySubModule : public SubModule
{
public:
	DebugGeometrySubModule(std::shared_ptr<IRenderingAPI>& p_API);

	std::string GetName() const override;

	void RegisterEvents(EventManager& p_EventManager) override;
#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
#endif
	void Update(const Duration& p_Duration) override;

private:
	void CmdList();

	std::shared_ptr<IRenderingAPI> m_API;

	struct AABB
	{
		AABB(const Vec3& p_Min, const Vec3& p_Max, const Color& p_Color, uint32 p_uiLineWidth, bool p_bDepthTest, const Time& p_EndTime)
		: m_Min(p_Min)
		, m_Max(p_Max)
		, m_Color(p_Color)
		, m_uiLineWidth(p_uiLineWidth)
		, m_bDepthTest(p_bDepthTest)
		, m_EndTime(p_EndTime)
		{
		}

		Vec3 m_Min;
		Vec3 m_Max;
		Color m_Color;
		uint32 m_uiLineWidth;
		bool m_bDepthTest;
		Time m_EndTime;
	};
	using AABBs = std::vector<AABB>;
	AABBs m_AABBs;
};

}
}