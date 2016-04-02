#include "pch.h"
#include "Color.h"

namespace Hawk {
namespace Gfx {

const Color Color::Black	(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::White	(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::Red		(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Green	(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::Blue		(0.0f, 0.0f, 1.0f, 1.0f);
const Color Color::Magenta	(1.0f, 0.0f, 1.0f, 1.0f);

Color::Color(float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha)
{
	Set(p_fRed, p_fGreen, p_fBlue, p_fAlpha);
}

Color::Color(uint8 p_uiRed, uint8 p_uiGreen, uint8 p_uiBlue, uint8 p_uiAlpha)
{
	Set(ToFloat(p_uiRed), ToFloat(p_uiGreen), ToFloat(p_uiBlue), ToFloat(p_uiAlpha));
}

void Color::Set(float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha)
{
	m_Channels = {p_fRed, p_fGreen, p_fBlue, p_fAlpha};
}

const float& Color::R() const
{
	return m_Channels[0];
}

const float& Color::G() const
{
	return m_Channels[1];
}

const float& Color::B() const
{
	return m_Channels[2];
}

const float& Color::A() const
{
	return m_Channels[3];
}

float& Color::R()
{
	return m_Channels[0];
}

float& Color::G()
{
	return m_Channels[1];
}

float& Color::B()
{
	return m_Channels[2];
}

float& Color::A()
{
	return m_Channels[3];
}

float Color::ToFloat(uint8 p_uiValue) const
{
	return p_uiValue / 255.0f;
}

}
}