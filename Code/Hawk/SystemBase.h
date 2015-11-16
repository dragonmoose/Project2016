#pragma once
#include "DllExport.h"
#include <string>

namespace Hawk
{
	class Duration;

	class HAWK_DLL_EXPORT SystemBase
	{
	public:
		SystemBase();
		virtual ~SystemBase();

		virtual void RegisterEvents();
		virtual void UnregisterEvents();
		virtual std::string GetName() const = 0;

		void InternalUpdate(const Duration& p_Duration);
		virtual void Update(const Duration& p_Duration);

		SystemBase(const SystemBase&) = delete;
		SystemBase& operator=(const SystemBase&) = delete;
	};
}