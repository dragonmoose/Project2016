#pragma once

#include "Events/IEvent.h"
#include "Math/Vec3.h"
#include "Color.h"
#include <string>

namespace Hawk {

namespace Gfx {

struct UpdateDebugTextEvent : public IEvent
{
	UpdateDebugTextEvent(std::string&& p_Label, std::string&& p_Value, std::string&& p_Category)
	: m_Label(std::move(p_Label))
	, m_Value(std::move(p_Value))
	, m_Category(std::move(p_Category)) {}

	std::string m_Label;
	std::string m_Value;
	std::string m_Category;
};

struct RemoveDebugTextEvent : public IEvent
{
	RemoveDebugTextEvent(std::string&& p_Label, std::string&& p_Category)
	: m_Label(std::move(p_Label))
	, m_Category(std::move(p_Category)) {}

	std::string m_Label;
	std::string m_Category;
};

struct AddDebugAABBEvent : public IEvent
{
	AddDebugAABBEvent(const Vec3& p_Min, const Vec3& p_Max, const Color& p_Color = Color::Red, uint32 p_uiLineWidth = 1, bool p_bDepthTest = false, const Duration& p_Duration = Duration(24, Duration::Precision::Hour))
	: m_Min(p_Min)
	, m_Max(p_Max)
	, m_Color(p_Color)
	, m_uiLineWidth(p_uiLineWidth)
	, m_bDepthTest(p_bDepthTest)
	, m_Duration(p_Duration)
	{
	}

	Vec3 m_Min;
	Vec3 m_Max;
	Color m_Color;
	uint32 m_uiLineWidth;
	bool m_bDepthTest;
	Duration m_Duration;
};

}}