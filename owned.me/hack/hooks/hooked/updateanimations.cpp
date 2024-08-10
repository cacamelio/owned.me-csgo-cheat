#include "../../features/features.hpp"

void __fastcall hooks::player::hkUpdateAnims(void* ecx, void* edx)
{
	BaseEntity* pPlayer = (BaseEntity*)(ecx);
	if (!pPlayer || !pPlayer->IsPlayer() || pPlayer->EntIndex() - 1 > 63 || !pPlayer->IsAlive() || !g::pLocalPlayer)
		return o_update_anims(ecx,edx);

	if (pPlayer->TeamNum() == g::pLocalPlayer->TeamNum())
	{
		if (pPlayer != g::pLocalPlayer)
			return o_update_anims(ecx, edx);
	}

	if (!g::bUpdateAnims)
	{
		if (pPlayer == g::pLocalPlayer)
			return g_localanims->onUpdateClientAnims();

		return g_AnimationSync->OnUpdateClientSideAnimation((BaseEntity*)(ecx));
	}

	return o_update_anims(ecx, edx);
}