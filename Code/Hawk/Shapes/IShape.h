#pragma once

namespace Hawk
{
	class IShape
	{
	public:
		virtual bool IsEnclosing(const IShape& p_Other) const = 0;
	};
}