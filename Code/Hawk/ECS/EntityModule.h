#pragma once
#include "Base/Module.h"
#include "Components.h"
#include "ComponentMask.h"
#include "Entity.h"
#include "Input/KeyCodes.h"
#include <vector>

namespace Hawk {
namespace ECS {

class EntityModule : public Module
{
public:
	EntityModule();
	std::string GetName() const override;

#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
	void CmdStats(bool p_bStates);
#endif

	void Initialize() override;
	void RegisterEvents(EventManager& p_EventManager) override;
	void Update(const Duration& p_Duration) override;

private:
	bool IsKeyDown(KeyCode p_Key) const;
	Entity& CreateEntity(const ComponentMask& p_Mask);
	void SortEntities();

	void CreateInteractiveCamera();
	void CreateFixedCamera();
	void CreateSuicidalEnemy(uint32 p_DurationSec);

	using EntityList = std::vector<Entity>;

	friend std::ostream& operator<<(std::ostream& os, const EntityList& p_Entities);

	EntityList m_Entities;
	std::size_t m_NextEntityIndex;

	std::vector<PositionComponent> m_Positions;
	std::vector<CameraComponent> m_Cameras;
	std::vector<KillTimeComponent> m_KillTimes;

	static const ComponentMask sc_FixedCameraMask;
	static const ComponentMask sc_InteractiveCameraMask;
	static const ComponentMask sc_SuicidalEnemyMask;

	std::bitset<256> m_Keys;
};

}
}