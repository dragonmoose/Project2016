#pragma once
#include "Macros.h"

namespace Hawk {

class Core
{
private:
	Core();
	~Core();
public:
	HAWK_DLL_EXPORT static Core& Get();

	Core(const Core&)			= delete;
	void operator=(const Core&) = delete;
};

}