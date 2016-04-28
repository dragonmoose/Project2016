#pragma once
#include <bitset>

namespace Hawk {
namespace ECS {

enum class ComponentFlags : uint32
{
	Position,
	Rotation,
	RenderableModel,
	RenderableManual,
	KeyInput,
	KillTime,
	Camera,

	_Count
};

}
}