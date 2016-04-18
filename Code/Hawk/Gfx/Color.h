#pragma once
#include <array>
#include "System/Types.h"

namespace Hawk {
namespace Gfx {

class HAWK_DLL_EXPORT Color final
{
public:
	Color(float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha);
	Color(uint8 p_uiRed, uint8 p_uiGreen, uint8 p_uiBlue, uint8 p_uiAlpha);

	void Set(float p_fRed, float p_fGreen, float p_fBlue, float p_fAlpha);

	const float& R() const;
	const float& G() const;
	const float& B() const;
	const float& A() const;

	float& R();
	float& G();
	float& B();
	float& A();

	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Magenta;

private:
	float ToFloat(uint8 p_uiValue) const;
	std::array<float, 4> m_Channels;
};

}
}