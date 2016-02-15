#include "pch.h"
#include "Entity.h"
#include "Debug/Assert.h"

namespace Hawk {

unsigned int Entity::s_uiNextObjectID = 1;

Entity::Entity()
: m_Name(std::string("Entity_") + std::to_string(s_uiNextObjectID++))
, m_bInitialized(false)
{
}

void Entity::AddChild(EntityPtr_t p_Entity)
{
	if (p_Entity->HasParent())
	{
		//p_Entity
	}
}

Entity* Entity::GetParent() const
{
	return m_Parent.get();
}

bool Entity::HasParent() const
{
	return m_Parent != nullptr;
}

void Entity::Initialize()
{
	ASSERT(!m_bInitialized, "Already initialized");
	for (auto& l_Component : m_Components)
	{
		l_Component.Initialize();
	}
	m_bInitialized = true;
}

void Entity::Update(const Duration& p_Duration)
{
	for (auto& l_Component : m_Components)
	{
		l_Component.Update(p_Duration);
	}
}

const glm::mat4x4& Entity::GetFrameWorldMatrix()
{
	return m_FrameWorldMatrix;
}

}