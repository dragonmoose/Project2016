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
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
#endif

	void Initialize() override;
	void RegisterEvents(EventManager& p_EventManager) override;
	void Update(const Duration& p_Duration) override;
	void AddToScene(Entity::EntityPtr p_Entity);
	void RemoveFromScene(Entity::EntityPtr p_Entity);
	void Clear();

	void CmdPrint();
	void CmdRemoveEntity(EntityID p_EntityID);
	void CmdReparentEntity(EntityID p_EntityID, EntityID p_NewParentID);
	void CmdAncestorOf(EntityID p_EntityID1, EntityID p_EntityID2);
	void CmdClear();
	void CmdPopulate(int32 p_iDepth, int32 p_iLevelMax);

private:
	void CreateRoot();

	Entity::EntityPtr m_Root;
	Entity::EntityVec m_Entities;

	using EntityIDMap = std::unordered_map<EntityID, Entity::EntityPtr>;
	EntityIDMap m_EntityIDMap;	
};

}