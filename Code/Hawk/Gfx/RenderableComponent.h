#pragma once
#include "Scene/Component.h"
namespace Hawk {
namespace Gfx {

class RenderableComponent : public Component
{
public:
	void AddedToScene() override;
	void RemovedFromScene() override;
};
}
}
