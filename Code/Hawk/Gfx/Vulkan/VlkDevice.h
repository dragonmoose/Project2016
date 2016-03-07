#pragma once
#include "VlkSystem.h"

namespace Hawk {
namespace Gfx {

class VlkDevice final
{
public:
	VlkDevice();
	~VlkDevice();
	VlkDevice(const VlkDevice&) = delete;
	VlkDevice& operator=(const VlkDevice&) = delete;

private:

#ifdef HAWK_DEBUG
#endif
};

}
}