#include "../../features/features.hpp"

// removed

bool __fastcall hooks::player::hkSetupBones(void* ecx, void* edx, matrix_t* bone_to_world_out, int max_bones, int bone_mask, float curtime)
{
	BaseEntity* pPlayer = (BaseEntity*)((uintptr_t)(ecx)-0x4);
	if (!pPlayer || !pPlayer->IsPlayer() || pPlayer->EntIndex() - 1 > 63 || !pPlayer->IsAlive() || !g::pLocalPlayer)
		return o_setup_bones(ecx, edx, bone_to_world_out, max_bones, bone_mask, curtime);

	if (pPlayer->TeamNum() == g::pLocalPlayer->TeamNum())
		if (pPlayer != g::pLocalPlayer)
			return o_setup_bones(ecx, edx, bone_to_world_out, max_bones, bone_mask, curtime);

	if (g::bUpdateBones)
		return o_setup_bones(ecx, edx, bone_to_world_out, max_bones, bone_mask, curtime);
	else if (bone_to_world_out)
	{
		if (pPlayer == g::pLocalPlayer)
			return g_localanims->getCachedMatrix(bone_to_world_out);
		else
			return g_AnimationSync->GetCachedMatrix(pPlayer, bone_to_world_out);
	}

	return true;
}