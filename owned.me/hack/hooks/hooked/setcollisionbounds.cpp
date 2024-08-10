#include "../../features/features.hpp"

float flCollisionChangeTime = 0.0f;
float flCollisionChangeOriginZ = 0.0f;

void __fastcall hooks::player::hkSetCollisionBounds(void* ecx, void* edx, vec3_t mins, vec3_t maxs)
{
	auto PerformCollision = [&](bool bPost) -> void
	{
		if (!ecx || (DWORD)(ecx) == 0xFFFFFCE0)
			return;

		BaseEntity* pPlayer = (BaseEntity*)((std::uint32_t)(ecx)-0x320);
		if (!pPlayer || !pPlayer->IsPlayer() || pPlayer->EntIndex() < 1 || pPlayer->EntIndex() > 64 || !g::pLocalPlayer || !g::pLocalPlayer->IsPlayer() || pPlayer != g::pLocalPlayer)
			return;

		if (bPost)
		{
			if (g::bUpdateCollision)
				return;

			pPlayer->m_flCollisionChangeTime() = flCollisionChangeTime;
			pPlayer->m_flCollisionChangeOriginZ() = flCollisionChangeOriginZ;
		}
		else
		{
			flCollisionChangeTime = pPlayer->m_flCollisionChangeTime();
			flCollisionChangeOriginZ = pPlayer->m_flCollisionChangeOriginZ();
		}
	};

	PerformCollision(false);
	o_setcol_bounds(ecx,mins, maxs);
	PerformCollision(true);
}