#pragma once

namespace Hawk {
namespace Gfx {

class IRenderingAPI
{
public:
	IRenderingAPI() = default;
	virtual ~IRenderingAPI() = default;

	virtual void Initialize() = 0;

	IRenderingAPI(const IRenderingAPI&) = delete;
	IRenderingAPI& operator=(const IRenderingAPI&) = delete;
};
}
}