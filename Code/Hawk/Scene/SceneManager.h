#pragma once
#include "Entity.h"
#include "Base/Module.h"
#include "System/Types.h"

namespace Hawk {

class SceneManager : public Module
{
public:
	SceneManager();
	std::string GetName() const override;
	Entity* GetRoot();

#ifdef HAWK_DEBUG
	void InitializeConsole() override;
#endif

	void Initialize() override;
	void RegisterEvents(EventManager& p_EventManager) override;
	void Update(const Duration& p_Duration) override;
	void AddToScene(Entity::EntityPtr_t p_Entity);
	void RemoveFromScene(Entity::EntityPtr_t p_Entity);
	void Clear();

	void CmdPrint();
	void CmdRemoveEntity(EntityID_t p_EntityID);
	void CmdReparentEntity(EntityID_t p_EntityID, EntityID_t p_NewParentID);
	void CmdAncestorOf(EntityID_t p_EntityID1, EntityID_t p_EntityID2);
	void CmdClear();
	void CmdPopulate(INT32 p_iDepth, INT32 p_iLevelMax);

private:
	void CreateRoot();

	Entity::EntityPtr_t m_Root;
	Entity::EntityVec_t m_Entities;

	using EntityIDMap_t = std::unordered_map<EntityID_t, Entity::EntityPtr_t>;
	EntityIDMap_t m_EntityIDMap;	
};

}