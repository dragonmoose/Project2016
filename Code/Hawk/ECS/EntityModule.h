#pragma once
#include "Base/Module.h"
#include "Components.h"
#include "ComponentMask.h"
#include "ComponentManager.h"
#include "Entity.h"
#include "Input/KeyCodes.h"
#include <vector>
#include <memory>

namespace Hawk {
namespace ECS {

class EntityModule : public Module
{
public:
	EntityModule();
	std::string GetName() const override;

	template<class T>
	T& AttachComponent(EntityID p_ID)
	{
		AttachComponent(p_ID, T::ID);
		return m_ComponentManager->Get(p_ID);
	}

	template<class T>
	void DetachComponent(EntityID p_ID)
	{
		DetachComponent(p_ID, T::ID);
	}

	std::shared_ptr<ComponentManager> GetComponentManager() const;

#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
	void CmdStats(bool p_bStates);
	void CmdAddRandom(std::size_t m_Count);
	void CmdList();
	void CmdPrint(EntityID p_ID);
#endif

	void Initialize() override;
	void RegisterEvents(EventManager& p_EventManager) override;
	void Update(const Duration& p_Duration) override;

private:
	void AttachComponent(EntityID p_EntityID, ComponentID p_ComponentID);
	void DetachComponent(EntityID p_EntityID, ComponentID p_ComponentID);
	std::size_t CreateEntity();
	bool IsKeyDown(KeyCode p_Key) const;
	void SortEntities();

	void CreateInteractiveCamera();
	void CreateFixedCamera();
	void CreateSuicidalEnemy(uint32 p_DurationSec);

	using EntityList = std::vector<Entity>;

	friend std::ostream& operator<<(std::ostream& os, const EntityList& p_Entities);

	EntityList m_Entities;
	std::size_t m_NextEntityIndex;

	static const ComponentMask sc_FixedCameraMask;
	static const ComponentMask sc_InteractiveCameraMask;
	static const ComponentMask sc_SuicidalEnemyMask;

	std::shared_ptr<ComponentManager> m_ComponentManager;

	std::bitset<256> m_Keys;
};

}
}