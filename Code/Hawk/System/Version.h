#pragma once
#include <string>
#include "DllExport.h"
#include "Types.h"

namespace Hawk {

class HAWK_DLL_EXPORT Version final
{
public:
	Version(uint16 p_uiMajor, uint8 p_uiMinor, uint8 p_uiPatch);
	uint32 GetID() const;
	const std::string& GetString() const;
	bool IsValid() const;
private:
	union
	{
		struct
		{
			uint8 m_uiPatch;
			uint8 m_uiMinor;
			uint16 m_uiMajor;
		} m_Parts;
		uint32 m_ID;
	} m_Value;
	std::string m_Str;
};

}