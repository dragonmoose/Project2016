#include "pch.h"
#include "Entity.h"
#include "Debug/Assert.h"
#include "SceneManager.h"

namespace Hawk {

EntityID_t Entity::s_NextID = 1;

Entity::Entity()
: Entity(std::string("Entity_") + std::to_string(s_NextID))
{
}

Entity::Entity(const std::string& p_Name)
: m_Name(p_Name)
, m_bInitialized(false)
, m_pSceneManager(nullptr)
, m_ID(s_NextID)
{
	s_NextID++;
}

Entity::~Entity()
{
	LOG("Entity deleted. Name=" << m_Name << " ID=" << m_ID, "scene", Debug);
}

void Entity::SetParent(EntityPtr_t p_Parent)
{
	ASSERT(p_Parent.get() != this, "Cannot set parent to itself");
	if (HasParent())
	{
		m_Parent->DetachChild(shared_from_this());
	}
	if (p_Parent)
	{
		p_Parent->AddChild(shared_from_this());
	}
}

const std::string& Entity::GetName() const
{
	return m_Name;
}

void Entity::AddChild(EntityPtr_t p_Entity)
{
	ASSERT(std::find(m_Children.begin(), m_Children.end(), p_Entity) == m_Children.end(), "Cannot add entity to itself");
	//ASSERT_THREAD("scene");
	bool l_bHadParent = p_Entity->HasParent();
	if (p_Entity->HasParent())
	{
		p_Entity->m_Parent->DetachChild(p_Entity);
	}

	m_Children.push_back(p_Entity);
	p_Entity->m_Parent = shared_from_this();

	if (InScene())
	{
		p_Entity->AddToScene(m_pSceneManager);
	}
}

void Entity::RemoveChild(EntityPtr_t p_Entity)
{
	//ASSERT_THREAD("scene");
	DetachChild(p_Entity);
	p_Entity->RemoveFromScene();
}

Entity::EntityPtr_t Entity::GetParent() const
{
	return m_Parent;
}

bool Entity::HasParent() const
{
	return m_Parent != nullptr;
}

bool Entity::HasChild(EntityPtr_t p_Entity) const
{
	return std::find(m_Children.begin(), m_Children.end(), p_Entity) != m_Children.end();
}

void Entity::AddToScene(SceneManager* p_pSceneManager)
{
	ASSERT(p_pSceneManager, "SceneManager should not be null");
	if (!InScene())
	{
		m_pSceneManager = p_pSceneManager;
		m_pSceneManager->AddToScene(shared_from_this());
	}
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

void Entity::RegisterEvents(EventManager& p_EventManager)
{
	for (auto& l_Component : m_Components)
	{
		l_Component.RegisterEvents(p_EventManager);
	}
}

void Entity::Update(const Duration& p_Duration)
{
	for (auto& l_Component : m_Components)
	{
		l_Component.Update(p_Duration);
	}
}

EntityID_t Entity::GetID() const
{
	return m_ID;
}

const glm::mat4x4& Entity::GetFrameWorldMatrix()
{
	return m_FrameWorldMatrix;
}

void Entity::DebugPrint(unsigned int p_uiDepth) const
{
	std::fill_n(std::ostreambuf_iterator<char>(std::cout), p_uiDepth * 3, ' ');
	std::cout << GetName() << "\t#" << m_ID << "\n";
	unsigned int l_uiChildDepth = p_uiDepth + 1;
	for (auto& l_Child : m_Children)
	{
		l_Child->DebugPrint(l_uiChildDepth);
	}
}

void Entity::DetachChild(EntityPtr_t p_Entity)
{
	auto l_Itr = std::find(m_Children.begin(), m_Children.end(), p_Entity);
	ASSERT(l_Itr != m_Children.end(), "Entity is not a child of this entity. Parent=" << m_Name << " Child=" << p_Entity->m_Name);

	p_Entity->m_Parent = nullptr;
	m_Children.erase(l_Itr);
}

bool Entity::InScene() const
{
	return m_pSceneManager != nullptr;
}

void Entity::RemoveFromScene()
{
	if (InScene())
	{
		m_pSceneManager->RemoveFromScene(shared_from_this());
	}
}

}