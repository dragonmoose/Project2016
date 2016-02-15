#pragma once
#include "Entity.h"

namespace Hawk {

class SceneManager
{
public:
	Entity& GetRoot();

private:
	Entity m_Root;
};

}