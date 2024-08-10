#include "../../features/features.hpp"


_declspec(noinline)void hkStandardBlendingRulesDT(BaseEntity* player, int i, studio_hdr_t* hdr, vec3_t* pos, quaternion* q, float curtime, int boneMask)
{
	static auto original = reinterpret_cast<bool(__thiscall*)(BaseEntity * player, int i, studio_hdr_t * hdr, vec3_t * pos, quaternion * q, float curtime, int boneMask)>(hooks::player::oStandardBlendingRules);

	original(player, i, hdr, pos, q, curtime, boneMask);
}

void __fastcall hooks::player::hkStandardBlendingRules(BaseEntity* player, int i, studio_hdr_t* hdr, vec3_t* pos, quaternion* q, float curtime, int boneMask)
{
	return hkStandardBlendingRulesDT(player, i, hdr, pos, q, curtime, boneMask);
}