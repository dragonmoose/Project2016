#pragma once
#include "Macros.h"
#include <string>

namespace Hawk
{
	class HAWK_DLL_EXPORT SystemBase
	{
	public:
		virtual void RegisterEvents();
		virtual void UnregisterEvents();
		virtual const std::string& GetName() const = 0;

		SystemBase(const SystemBase&) = delete;
		SystemBase& operator=(const SystemBase&) = delete;
	};
}