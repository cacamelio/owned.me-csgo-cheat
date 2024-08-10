#pragma once
#include "../../client/math/math.hpp"
#include <array>
#include "collideable.hpp"
#include "client_class.hpp"
#include "../structs/animstate.hpp"
#include "../../client/utilities/netvars/netvars.hpp"
#include "../../client/utilities/xor.h"
#include "client_entity.hpp"
class CBaseViewModel;
class BaseWeapon;
class BaseEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable {
private:
	template <typename T>
	T& read(uintptr_t offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	template <typename T>
	void write(uintptr_t offset, T data) {
		*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset) = data;
	}
	template <typename T>
	T& set(uintptr_t offset)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}
	template< typename t >
	__forceinline t& get(size_t offset) {
		return *(t*)((uintptr_t)this + offset);
	}
public:

	// netvars
	NETVAR("DT_Inferno", "m_nFireEffectTickBegin", FireEffectTickBegin, int);
	NETVAR("DT_PlantedC4", "m_bBombTicking", BombTicking, bool);
	NETVAR("DT_BaseEntity", "m_nModelIndex", GetModelIdx, unsigned)
	NETVAR("DT_BasePlayer", "m_hViewModel[0]", GetViewModel, CHandle<CBaseViewModel>);
	NETVAR("DT_PlantedC4", "m_bBombDefused", BombDefused, bool);
	NETVAR("DT_PlantedC4", "m_flC4Blow", BlowTime, float);
	NETVAR("DT_PlantedC4", "m_nBombSite", BombSite, int);
	NETVAR("DT_CSPlayer", "m_hOwnerEntity", OwnerHandle, unsigned long)
	NETVAR("DT_CSPlayer", "m_flSimulationTime", SimulationTime, float)
	NETVAR("DT_BasePlayer", "m_vecOrigin", Origin, vec3_t)
	NETVAR("DT_BasePlayer", "m_vecViewOffset[0]", ViewOffset, vec3_t)
	NETVAR("DT_CSPlayer", "m_iTeamNum", TeamNum, int)
	NETVAR("DT_CSPlayer", "m_flThirdpersonRecoil", ThirdpersonRecoil, float)
	NETVAR("DT_BaseEntity", "m_bSpotted", Spotted, bool)
	NETVAR("DT_CSPlayer", "m_nSurvivalTeam", SurvivalTeam, int)
	NETVAR("DT_CSPlayer", "m_flHealthShotBoostExpirationTime", HealthBoostTime, float)
	NETVAR("DT_CSPlayer", "m_bHasDefuser", HasDefuser, bool)
	NETVAR("DT_CSPlayer", "m_bGunGameImmunity", HasGunGameImmunity, bool)
	NETVAR("DT_CSPlayer", "m_iShotsFired", ShotsFired, int)
    NETVAR("DT_CSPlayer", "m_bWaitForNoAttack", WaitNextAttack, bool)
	NETVAR("DT_CSPlayer", "m_angEyeAngles[0]", EyeAngles, vec3_t)
	NETVAR("DT_CSPlayer", "m_ArmorValue", Armor, int)
	NETVAR("DT_CSPlayer", "m_bHasHelmet", HasHelmet, bool)
	NETVAR("DT_CSPlayer", "m_bIsScoped", IsScoped, bool)
	NETVAR("DT_CSPlayer", "m_bIsDefusing", IsDefusing, bool)
	NETVAR("DT_CSPlayer", "m_iAccount", Money, int)
	NETVAR("DT_CSPlayer", "m_flLowerBodyYawTarget", LowerBodyYaw, float)
	NETVAR("DT_CSPlayer", "m_flNextAttack", NextAttack, float)
	NETVAR("DT_CSPlayer", "m_flFlashDuration", FlashDuration, float)
	NETVAR("DT_CSPlayer", "m_flFlashMaxAlpha", FlashAlpha, float)
	NETVAR("DT_CSPlayer", "m_bHasNightVision", HasNightVision, bool)
	NETVAR("DT_CSPlayer", "m_bNightVisionOn", NightVisionOn, bool)
	NETVAR("DT_CSPlayer", "m_iHealth", Health, int)
	NETVAR("DT_CSPlayer", "m_lifeState", LifeState, int)
	NETVAR("DT_CSPlayer", "m_fFlags", Flags, int)
	NETVAR("DT_CSPlayer", "m_flLastExoJumpTime", LastExoJumpTime, float)
	NETVAR("DT_BasePlayer", "m_iObserverMode", ObserverMode, int)
	NETVAR("DT_BasePlayer", "m_viewPunchAngle", PunchAngle, vec3_t)
	NETVAR("DT_BasePlayer", "m_aimPunchAngleVel", AimPunchAngleVel, vec3_t);
	NETVAR("DT_BasePlayer", "m_aimPunchAngle", AimPunchAngle, vec3_t)
	NETVAR("DT_BasePlayer", "m_vecVelocity[0]", Velocity, vec3_t)
	NETVAR("DT_BasePlayer", "m_flFallVelocity", FallVelocity, float_t);
	NETVAR("DT_BasePlayer", "m_flMaxspeed", MaxSpeed, float)
	NETVAR("DT_BaseEntity", "m_flShadowCastDistance", FovTime, float)
	NETVAR("DT_BasePlayer", "m_hObserverTarget", ObserverTarget, unsigned long)
	NETVAR("DT_BasePlayer", "m_nHitboxSet", HitboxSet, int)
	NETVAR("DT_CSPlayer", "m_flDuckSpeed", DuckSpeed, float)
	NETVAR("DT_CSPlayer", "m_flVelocityModifier", VelocityModifier, float)
	NETVAR("DT_CSPlayer", "m_flDuckAmount", DuckAmount, float)
	NETVAR("DT_BasePlayer", "m_hGroundEntity", GroundEntity, CHandle < C_BaseEntity >);
	NETVAR("DT_CSPlayer", "m_bHasHeavyArmor", HasHeavyArmor, bool)
	NETVAR("DT_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin", SmokeGrenadeTickBegin, int)
	NETVAR("DT_CSPlayer", "m_nTickBase", GetTickBase, int)
	NETVAR("DT_BaseEntity", "m_vecMins", Mins, vec3_t);
	NETVAR("DT_BaseEntity", "m_vecMaxs", Maxs, vec3_t);
	NETVAR("DT_BaseAnimating", "m_bClientSideAnimation", ClientSideAnimation, bool);
	NETVAR("DT_BasePlayer", "m_nNextThinkTick", GetNextThinkTick, int);
	NETVAR("DT_Inferno", "m_fireCount", FireCount, int);

	NETVARA("DT_CSPlayer", "m_flLastExoJumpTime", IsUsedNewAnimState, 0x8, int);

	// functions
	VFUNC(139, Think(), void(__thiscall*)(void*))()
	VFUNC(318, PreThink(), void(__thiscall*)(void*))()
	VFUNC(319, PostThink(), void(__thiscall*)(void*))()
	VFUNC(224, UpdateClientSideAnimation(), void(__thiscall*)(void*))()

	// offsets
	OFFSET(int, Effects, 0xF0)
	OFFSET(bool, Rescuing, 0x3938)
	OFFSET(bool, Grabbing, 0x3931)
	OFFSET(vec3_t, AbsVelocity, 0x94);
	OFFSET(vec3_t, BaseVelocity, 0x120);
	OFFSET(UserCmd*, m_PlayerCommand, 0x3298);
	OFFSET(UserCmd*, m_pCurrentCommand, 0x3348);
	OFFSET(uint32_t, OcclusionFlags, 0xA28);
	OFFSET(uint32_t, OcclusionFrame, 0xA30);
	OFFSET(uint32_t, ReadableBones, 0x26AC);
	OFFSET(uint32_t, WriteableBones, 0x26B0);
	OFFSET(uint32_t, MostRecentModelBoneCounter, 0x2690);
	OFFSET(int, LastSetupBonesFrame, 0xA68);
	OFFSET(int, PreviousBoneMask, 0x269C);
	OFFSET(int, BoneCount, 0x2920);
	OFFSET(int, ForceBone, 0x268C);
	OFFSET(vec3_t, m_angVisualAngles, 0x31E8);
	OFFSET(float, OldSimulationTime, 0x26C);
	OFFSET(float, SurfaceFriction, 0x323C);

	CUSTOM_OFFSET(m_nClientEffects, int32_t, FNV32("ClientEffects"), 0x68);
	CUSTOM_OFFSET(m_nLastSkipFramecount, int32_t, FNV32("LastSkipFramecount"), 0xA68);
	CUSTOM_OFFSET(m_pInverseKinematics, LPVOID, FNV32("InverseKinematics"), 9840);
	CUSTOM_OFFSET(m_bMaintainSequenceTransition, bool, FNV32("MaintainSequenceTransition"), 0x9F0);
	CUSTOM_OFFSET(m_nOcclusionMask, int32_t, FNV32("OcclusionMask"), 2600);
	CUSTOM_OFFSET(m_bJiggleBones, bool, FNV32("JiggleBones"), 0x2930);
	CUSTOM_OFFSET(m_nOcclusionFrame, int32_t, FNV32("OcclusionFrame"), 2608);

	__forceinline float& m_flCollisionChangeTime()
	{
		return *(float*)((uint32_t)(this) + 0x9924);
	}
	__forceinline float& m_flCollisionChangeOriginZ()
	{
		return *(float*)((uint32_t)(this) + 0x9920);
	}

	// others
	__forceinline void* Animating()
	{
		return reinterpret_cast<void*>(uintptr_t(this) + 0x4);
	}

	__forceinline void* Networkable()
	{
		return reinterpret_cast<void*>(uintptr_t(this) + 0x8);
	}

	__forceinline Collideable* Collideables()
	{
		using original_fn = Collideable * (__thiscall*)(void*);
		return (*(original_fn**)this)[3](this);
	}

	__forceinline ClientClass* GetClientClass()
	{
		using original_fn = ClientClass * (__thiscall*)(void*);
		return (*(original_fn**)Networkable())[2](Networkable());
	}

	__forceinline int GetClassId()
	{
		ClientClass* cc{ GetClientClass() };

		return (cc) ? cc->m_ClassID : -1;
	}
	
	__forceinline float SpawnTime() {
		return *(float*)((uintptr_t)this + 0x103C0);
	}

	//vec3_t m_angVisualAngles() {
	//	return *(vec3_t*)((uintptr_t)this + 0x31E8);
	//}

	typedef void(__thiscall* GetShootPosition_t)(LPVOID, vec3_t*);
	__forceinline vec3_t GetShootPosition()
	{
		vec3_t vecShootPosition = vec3_t(0, 0, 0);
		if (!this)
			return vecShootPosition;

		utilities::CallVirtual < GetShootPosition_t >(this, 285)(this, &vecShootPosition);
		return vecShootPosition;
	}

	vec3_t GetEyePosition(bool bShouldCorrect = true)
	{
		vec3_t vecPosition = { };

		// get eye position
		utilities::CallVFunc<void>(this, 169, std::ref(vecPosition));

		// correct this like it do weapon_shootpos
		// @ida weapon_shootpos: 55 8B EC 56 8B 75 08 57 8B F9 56 8B 07 FF 90
		if (IsUsedNewAnimState() && bShouldCorrect)
		{
			if (CSGOPlayerAnimState* pAnimState = this->AnimState(); pAnimState != nullptr)
				ModifyEyePosition(pAnimState, &vecPosition);
		}

		// return corrected position
		return vecPosition;
	}

	__forceinline int Index()
	{
		using original_fn = int(__thiscall*)(void*);
		return (*(original_fn**)Networkable())[10](Networkable());
	}

	__forceinline bool IsPlayer()
	{
		return GetClientClass()->m_ClassID == 40;
	}

	__forceinline bool IsWeapon()
	{
		using original_fn = bool(__thiscall*)(BaseEntity*);
		return (*(original_fn**)this)[165](this);
	}

	__forceinline bool SetupBones(matrix_t* out, int max_bones, int mask, float time)
	{
		if (!this)
			return false;

		using original_fn = bool(__thiscall*)(void*, matrix_t*, int, int, float);
		return (*(original_fn**)Animating())[13](Animating(), out, max_bones, mask, time);
	}

	__forceinline model_t* Model()
	{
		using original_fn = model_t * (__thiscall*)(void*);
		return (*(original_fn**)Animating())[8](Animating());
	}

	__forceinline void Update()
	{
		using original_fn = void(__thiscall*)(BaseEntity*);
		(*(original_fn**)this)[218](this);
	}

	__forceinline float GetLayerSequenceCycleRate(AnimationLayer* layer, int LayerSequence)
	{
		return utilities::CallVirtual< float(__thiscall*)(void*, AnimationLayer*, int)>(this, 223)(this,layer,LayerSequence);
	}

	__forceinline int DrawModel(int flags, uint8_t alpha)
	{
		using original_fn = int(__thiscall*)(void*, int, uint8_t);
		return (*(original_fn**)Animating())[9](Animating(), flags, alpha);
	}

	__forceinline void SetAbsAngles(vec3_t angles)
	{
		using original_fn = void(__thiscall*)(void*, const vec3_t&);
		static original_fn SetAngles_fn = (original_fn)((DWORD)pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8")));
		SetAngles_fn(this, angles);
	}

	__forceinline void SetPosition(vec3_t position) {

		using original_fn = void(__thiscall*)(void*, const vec3_t&);
		static original_fn set_position_fn = (original_fn)((DWORD)pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8")));
		set_position_fn(this, position);
	}

	__forceinline void SetModelIndex(int index)
	{
		using original_fn = void(__thiscall*)(void*, int);
		return (*(original_fn**)this)[75](this, index);
	}

	__forceinline void NetPreDataUpdate(int update_type)
	{
		using original_fn = void(__thiscall*)(void*, int);
		return (*(original_fn**)Networkable())[6](Networkable(), update_type);
	}

	__forceinline void NetRelease()
	{
		using original_fn = void(__thiscall*)(void*);
		return (*(original_fn**)Networkable())[1](Networkable());
	}

	__forceinline int NetSetDestoryedOnRecreateEntities()
	{
		using original_fn = int(__thiscall*)(void*);
		return (*(original_fn**)Networkable())[13](Networkable());
	}

	__forceinline bool Dormant()
	{
		return utilities::CallVirtual< bool(__thiscall*)(void*) >(Networkable(), 9)(Networkable());
	}

	__forceinline bool IsBaseCombatWeapon()
	{
		return utilities::GetMethod< bool(__thiscall*)(void*) >(this, 166)(this);
	}

	__forceinline vec3_t& AbsOrigin()
	{
		using original_fn = vec3_t & (__thiscall*)(void*);
		return (*(original_fn**)this)[10](this);;
	}

	__forceinline CUtlVector <matrix_t>& BoneCache() {
		static auto m_CachedBoneData = *(DWORD*)(pattern::Scan(("client.dll"), ("FF B7 ?? ?? ?? ?? 52")) + 0x2) + 0x4;
		return *(CUtlVector <matrix_t>*)(uintptr_t(this) + m_CachedBoneData);
	}

	__forceinline const vec3_t& WorldSpaceCenter()
	{
		vec3_t vecOrigin = Origin();

		vec3_t vecMins = this->GetCollideable()->OBBMins() + vecOrigin;
		vec3_t vecMaxs = this->GetCollideable()->OBBMaxs() + vecOrigin;

		vec3_t vecSize = vecMaxs - vecMins;
		vecSize /= 2.0f;
		vecSize += vecMins;
		return vecSize;
	}

	__forceinline bool is(hash32_t hash)
	{
		return netvars.GetClientID(hash) == GetClassId();
	}

	__forceinline int TotalScore()
	{
		return *(int*)((uintptr_t)this + 0x0B40);
	}

	__forceinline int GetTeamNum()
	{
		return *(int*)((uintptr_t)this + 0x0B68);
	}

	__forceinline int& FlagsPtr()
	{
		return *(int*)((DWORD)this + 0x104);
	}

	__forceinline int& EFlags()
	{
		return set<int>(0xE4);
	}

	__forceinline size_t* FireXDelta()
	{
		return (size_t*)((uintptr_t)this + netvars.GetNetVar(fnv::hash("DT_Inferno"), fnv::hash("m_fireXDelta")));
	}

	__forceinline int* FireYDelta()
	{
		return (int*)((uintptr_t)this + netvars.GetNetVar(fnv::hash("DT_Inferno"), fnv::hash("m_fireYDelta")));
	}

	__forceinline int* FireZDelta()
	{
		return (int*)((uintptr_t)this + netvars.GetNetVar(fnv::hash("DT_Inferno"), fnv::hash("m_fireZDelta")));
	}

	__forceinline bool* FireIsBurning()
	{
		return (bool*)((uintptr_t)this + netvars.GetNetVar(fnv::hash("DT_Inferno"), fnv::hash("m_bFireIsBurning")));
	}

	__forceinline float* m_flPoseParameter()
	{
		return (float*)((uintptr_t)this + netvars.GetNetVar(fnv::hash("DT_CSPlayer"), fnv::hash("m_flPoseParameter")));
	}

	__forceinline void SetPoseParameters(float* poses)
	{
		std::memcpy(m_flPoseParameter(), poses, sizeof(float) * 24);
	}

	__forceinline void GetPoseParameters(float* poses)
	{
		std::memcpy(poses, m_flPoseParameter(), sizeof(float) * 24);
	}

	__forceinline std::array < float_t, 24 >& PoseParameters()
	{
		return *(std::array < float_t, 24 >*)((uintptr_t)(this) + 0x2778);
	}

	__forceinline void SetAnimLayers(AnimationLayer* layers)
	{
		std::memcpy(AnimOverlays(), layers, sizeof(AnimationLayer) * AnimLayerCount());
	}

	__forceinline void GetAnimLayers(AnimationLayer* layers)
	{
		std::memcpy(layers, AnimOverlays(), sizeof(AnimationLayer) * AnimLayerCount());
	}

	__forceinline void InvalidatePhysicsRecursive(int32_t flags)
	{
		static const auto invalidate_physics_recursive = reinterpret_cast<void(__thiscall*)(BaseEntity*, int32_t)>(pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3")));
		invalidate_physics_recursive(this, flags);
	}

	void SetupBones_AttachemtnHelper() {
		static const auto sig_fn = reinterpret_cast<void(__thiscall*)(void*, c_studio_hdr*)>(pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4")));
		return sig_fn(this, this->ModelPtr());
	}

	matrix_t*& BoneArrayForWrite()
	{
		return *(matrix_t**)((DWORD)this + 0x26A8);
	}

	__forceinline void InvalidateBoneCache()
	{
		static auto invalidate_bone_cache_fn = pattern::Scan(XOR("client.dll"), XOR("80 ? ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 ? ? ? ? ? ? ? ? ? 89 ? ? ? ? ? C3"));
		
		reinterpret_cast<void(__fastcall*) (void*)> (invalidate_bone_cache_fn) (this);
	}

	__forceinline void BuildTransformations(c_studio_hdr* hdr, vec3_t* pos, quaternion* q, const matrix3x4a_t& transform, int mask, uint8_t* computed)
	{
		using BuildTransformations_t = void(__thiscall*)(decltype(this), c_studio_hdr*, vec3_t*, quaternion*, matrix3x4a_t const&, int, uint8_t*);
		return utilities::GetMethod< BuildTransformations_t >(this, 189)(this, hdr, pos, q, transform, mask, computed);
	}

	__forceinline void StandardBlendingRules(c_studio_hdr* hdr, vec3_t* pos, quaternion* q, float time, int mask)
	{
		using StandardBlendingRules_t = void(__thiscall*)(decltype(this), c_studio_hdr*, vec3_t*, quaternion*, float, int);
		return utilities::GetMethod< StandardBlendingRules_t >(this, 205)(this, hdr, pos, q, time, mask);
	}

	__forceinline [[nodiscard]] int ButtonDisabled()
	{
		return *(int*)((std::uintptr_t)this + 0x3330);
	}

	__forceinline [[nodiscard]] int ButtonForced()
	{
		return *(int*)((std::uintptr_t)this + 0x3334);
	}

	__forceinline [[nodiscard]] int& TakeDamage()
	{
		return *(int*)((std::uintptr_t)this + 0x280);
	}

	__forceinline UserCmd** CurrentCommand()
	{
		static std::uintptr_t m_pCurrentCommand = netvars.GetNetVar(fnv::hash("DT_BasePlayer"), fnv::hash("m_hConstraintEntity")) - 0xC;
		return (UserCmd**)((std::uintptr_t)this + m_pCurrentCommand);
	}

	__forceinline UserCmd& LastCommand()
	{
		return *(UserCmd*)((std::uintptr_t)this + 0x3288);
	}

	__forceinline bool Reloading()
	{
		AnimationLayer* animlayer = &AnimOverlays()[1];
		if (animlayer->pOwner)
		{
			auto activity = SequenceActivity(animlayer->nSequence);
			if ((activity == 967 && animlayer->flWeight != 0.0f) && animlayer->flCycle < 0.99)
				return true;
			else
				return false;
		}
	}

	__forceinline bool PhysicsRunThink(int nThinkMethod)
	{
		using PhysicsRunThinkFn = bool(__thiscall*)(void*, int);
		static auto oPhysicsRunThink = (PhysicsRunThinkFn)pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 EC 10 53 56 57 8B F9 8B 87 ? ? ? ? C1 E8 16"));
		return oPhysicsRunThink(this, nThinkMethod);
	}

	__forceinline bool Enemy(BaseEntity* from)
	{
		if (TeamNum() != from->TeamNum())
			return true;

		else if (convars::mp_teammates_are_enemies->GetInt())
			return true;

		return false;
	}

	__forceinline BaseWeapon* ActiveWeapon()
	{
		auto ActiveWeapon = read<uintptr_t>(netvars.GetNetVar(fnv::hash("DT_CSPlayer"), fnv::hash("m_hActiveWeapon"))) & 0xFFF;
		return reinterpret_cast<BaseWeapon*>(interfaces::entity_list->get_client_entity(ActiveWeapon));
	}

	__forceinline UINT* Wearables()
	{
		return (UINT*)((uintptr_t)this + (netvars.GetNetVar(fnv::hash("DT_CSPlayer"), fnv::hash("m_hMyWearables"))));
	}

	__forceinline CBaseHandle2* MyWeapons()
	{
		return (CBaseHandle2*)((uintptr_t)this + 0x2DF8);
	}

	__forceinline vec3_t EyePos()
	{
		return Origin() + ViewOffset();
	}

	__forceinline AnimationLayer* AnimOverlays()
	{
		return *(AnimationLayer**)((std::uintptr_t)this + 0x2990);
	}

	__forceinline int SequenceActivity(int sequence)
	{
		studio_hdr_t* pStudioHdr = interfaces::model_info->get_studio_model(this->Model());
		if (pStudioHdr == nullptr)
			return -1;

		using SequenceActivityFn = int(__fastcall*)(void*, void*, int);
		static auto oSequenceActivity = (SequenceActivityFn)(pattern::Scan(XOR("client.dll"), ("55 8B EC 53 8B 5D 08 56 8B F1 83"))); // @xref: "Need to handle the activity %d\n"
		return oSequenceActivity(this, pStudioHdr, sequence);
	}

	__forceinline AnimationLayer* AnimLayer(int nLayer)
	{
		if (nLayer >= 0 && nLayer < 15)
			return &AnimOverlays()[nLayer];
		return nullptr;
	}

	__forceinline int AnimLayerCount()
	{
		return *(int*)((std::uintptr_t)this + 0x299C);
	}

	__forceinline bool CanSeePlayerPos(BaseEntity* player, const vec3_t& pos)
	{
		trace_t tr;
		ray_t ray;
		trace_filter filter;
		filter.skip = this;

		auto start = EyePos();
		auto dir = (pos - start).normalized();

		ray.initialize(start, pos);
		interfaces::trace_ray->trace_ray(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

		return tr.entity == player || tr.flFraction > 0.97f;
	}

	__forceinline vec3_t GetBonePos(int bone)
	{
		matrix_t bone_matrices[128];
		if (SetupBones(bone_matrices, 128, 256, 0.0f))
			return vec3_t{ bone_matrices[bone][0][3], bone_matrices[bone][1][3], bone_matrices[bone][2][3] };
		else
			return vec3_t{ };
	}

	int GetBoneByHash(const FNV1A_t uBoneHash) const
	{
		if (const auto pModel = this->GetModel(); pModel != nullptr)
		{
			if (const auto pStudioHdr = interfaces::model_info->get_studio_model(pModel); pStudioHdr != nullptr)
			{
				for (int i = 0; i < pStudioHdr->bones_count; i++)
				{
					if (const auto pBone = pStudioHdr->bone(i); pBone != nullptr && FNV1A::Hash(pBone->name()) == uBoneHash)
						return i;
				}
			}
		}

		return -1;
	}

	__forceinline vec3_t GetHitboxPos(int hitbox_id)
	{
		matrix_t bone_matrix[MAXSTUDIOBONES];

		if (SetupBones(bone_matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
			auto studio_model = interfaces::model_info->get_studio_model(Model());

			if (studio_model) {
				auto hitbox = studio_model->hitbox_set(0)->hitbox(hitbox_id);

				if (hitbox) {
					auto min = vec3_t{}, max = vec3_t{};

					math::transform_vector(hitbox->mins, bone_matrix[hitbox->bone], min);
					math::transform_vector(hitbox->maxs, bone_matrix[hitbox->bone], max);

					return vec3_t((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
				}
			}
		}
		return vec3_t{};
	}

	__forceinline float GetMaxDesyncDelta()
	{
		if (!this)
			return 0.0f;
		
		auto animstate = this->AnimState();

		if (!animstate)
			return 0.0f;

		auto speedfactor = std::clamp(animstate->m_flSpeedAsPortionOfWalkTopSpeed, 0.0f, 1.0f);
		auto avg_speedfactor = (animstate->m_flWalkToRunTransition * -0.3f - 0.2f) * speedfactor + 1.0f;

		auto duckamount = animstate->m_flAnimDuckAmount;

		if (duckamount)
		{
			auto max_velocity = std::clamp(animstate->m_flSpeedAsPortionOfCrouchTopSpeed, 0.0f, 1.0f);
			auto duck_speed = duckamount * max_velocity;

			avg_speedfactor += duck_speed * (0.5f - avg_speedfactor);
		}

		return animstate->yaw_desync_adjustment() * avg_speedfactor;
	}

	__forceinline vec3_t GetHitboxPos(BaseEntity* entity, int hitbox_id)
	{
		matrix_t bone_matrix[MAXSTUDIOBONES];

		if (entity->SetupBones(bone_matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
			auto studio_model = interfaces::model_info->get_studio_model(entity->Model());

			if (studio_model) {
				auto hitbox = studio_model->hitbox_set(0)->hitbox(hitbox_id);

				if (hitbox) {
					auto min = vec3_t{}, max = vec3_t{};

					math::transform_vector(hitbox->mins, bone_matrix[hitbox->bone], min);
					math::transform_vector(hitbox->maxs, bone_matrix[hitbox->bone], max);

					return vec3_t((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
				}
			}
		}
		return vec3_t{};
	}


	__forceinline bool IsAlive()
	{
		if (!this) return false;
		return this->Health() > 0;
	}

	__forceinline bool IsMoving()
	{
		if (!this) return false;
		return this->Velocity().length() > 0.1f;
	}

	__forceinline bool IsInAir()
	{
		if (!this) return false;
		return !(this->Flags() & FL_ONGROUND);
	}

	__forceinline bool IsFlashed()
	{
		if (!this) return false;
		return this->FlashDuration() > 0.0f;
	}

	__forceinline vec3_t& AbsAngles()
	{
		using original_fn = vec3_t & (__thiscall*)(void*);
		return (*(original_fn**)this)[11](this);;
	}

	__forceinline int MoveType()
	{
		static int type = netvars.GetNetVar(fnv::hash("DT_BaseEntity"), fnv::hash("m_nRenderMode")) + 1;
		return read<int>(type);
	}

	bool is_breakable_entity()
	{
		if (!this || !this->EntIndex())
			return false;

		static auto is_breakable = pattern::Scan(XOR("client.dll"), XOR("55 8B EC 51 56 8B F1 85 F6 74 68"));

		const auto szObjectName = this->GetClientClass( )->m_pNetworkName;
		if ( szObjectName[ 0 ] == 'C' )
		{
			if ( szObjectName[ 7 ] == 't' || szObjectName[ 7 ] == 'b' )
				return true;
		}

		return ( ( bool( __thiscall* )( void* ) )( is_breakable ) )( this );
	}

	bool HasC4();
	void SetAbsOrigin(const vec3_t& origin);
	bool CanFire(int32_t ShiftAmount = 0, bool bCheckRevolver = false);
	float GetMaxPlayerSpeed();
	void ModifyEyePosition(const CSGOPlayerAnimState* pAnimState, vec3_t* vecPosition) const;

	c_studio_hdr* StudioHdr();
	c_studio_hdr* ModelPtr();
	CBoneAccessor& BoneAccessor();
	CSGOPlayerAnimState* AnimState();
};

class BaseWeapon : public BaseEntity
{
public:
	NETVAR("DT_BaseCombatWeapon", "m_flNextPrimaryAttack", NextPrimaryAttack, float)
	NETVAR("DT_BaseCombatWeapon", "m_flNextSecondaryAttack", NextSecondaryAttack, float)
	NETVAR("DT_BaseCombatWeapon", "m_flPostponeFireReadyTime", PostpineFireReadyTime, float)
	NETVAR("DT_BaseCombatWeapon", "m_iClip1", Clip1Count, int)
	NETVAR("DT_BaseCombatWeapon", "m_iClip2", Clip2Count, int)
	NETVAR("DT_BaseCombatWeapon", "m_fLastShotTime", LastShotTime, float)
	NETVAR("DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount", PrimaryReserveAmmoCount, int)
	NETVAR("DT_BaseCSGrenade", "m_flThrowStrength", ThrowStrength, float)
	NETVAR("DT_BaseCSGrenade", "m_fThrowTime", ThrowTime, float)
	NETVAR("DT_WeaponCSBase", "m_flRecoilIndex", RecoilIndex, float)
	NETVAR("DT_WeaponCSBase", "m_fAccuracyPenalty", AccuracyPenalty, float);
	NETVAR("DT_BaseCSGrenade", "m_bPinPulled", pin_pulled, bool)
	NETVAR("DT_WeaponCSBaseGun", "m_zoomLevel", ZoomLevel, float)
	NETVAR("DT_BaseAttributableItem", "m_iItemDefinitionIndex", ItemDefinitionIndex, short)

	VFUNC(453, GetSpread(), float(__thiscall*)(void*))()
	VFUNC(483, GetInaccuracy(), float(__thiscall*)(void*))()
	VFUNC(484, UpdateAccuracyPenalty(), void(__thiscall*)(void*))()
	VFUNC(461, GetWpnData(), CWeaponData* (__thiscall*)(void*))()

	__forceinline bool IsNonAim()
	{
		if (!this)
			return true;

		auto idx = ItemDefinitionIndex();

		if (idx == WEAPON_C4 || idx == WEAPON_HEALTHSHOT)
			return true;

		if (IsKnife())
			return true;

		if (IsGrenade())
			return true;

		return false;
	}

	__forceinline bool IsGrenade() 
	{
		if (!this)
			return false;

		auto current_weapon = reinterpret_cast<BaseWeapon*>(this);
		int id = current_weapon->ItemDefinitionIndex();
		static const std::vector<int> v = { WEAPON_FRAG_GRENADE, WEAPON_HEGRENADE, WEAPON_INCGRENADE, WEAPON_SMOKEGRENADE, WEAPON_TAGRENADE, WEAPON_MOLOTOV, WEAPON_FLASHBANG };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	__forceinline bool IsZoomable(bool extra_check = true) 
	{
		return ItemDefinitionIndex() == WEAPON_SSG08
			|| ItemDefinitionIndex() == WEAPON_SCAR20
			|| ItemDefinitionIndex() == WEAPON_AWP
			|| ItemDefinitionIndex() == WEAPON_G3SG1
			|| (extra_check && (ItemDefinitionIndex() == WEAPON_SG556 || ItemDefinitionIndex() == WEAPON_AUG));
	}

	__forceinline bool IsKnife() {

		if ( !this || !this->GetWpnData())
			return false;
		
		return this->GetWpnData()->weapon_type == WEAPONTYPE_KNIFE;
	}

	__forceinline bool IsGun()
	{
		if (!this)
			return false;

		int id = this->ItemDefinitionIndex();

		if (id == WEAPON_C4 || IsKnife() || IsGrenade())
			return false;

		return true;
	}

	__forceinline std::string GetLocalizedName() 
	{
		if ( !this )
			return XOR( "ERROR" );

		auto weapondata = this->GetWpnData();
		if (!weapondata)
			return XOR("ERROR");

		return utilities::WideToMultiByte(interfaces::localize->find(weapondata->weapon_hud_name));
	}

	CBaseHandle2 WeaponWorldModel()
	{
		return *(CBaseHandle2*)((uintptr_t)this + 0x3254);
	}

	__forceinline std::string GetWeaponIcon() 
	{
		if ( !this )
			return XOR( "" );
		auto weapondata = this->GetWpnData();
		if (!weapondata)
			return XOR("");
		auto index = this->ItemDefinitionIndex();
		if (!index)
			return XOR("");


		switch (index)
		{
		case WEAPON_SSG08:
			return XOR("a");
		case WEAPON_XM1014:
			return XOR("b");
		case WEAPON_SAWEDOFF:
			return XOR("c");
		case WEAPON_MAG7:
			return XOR("d");
		case WEAPON_NOVA:
			return XOR("e");
		case WEAPON_NEGEV:
			return XOR("f");
		case WEAPON_M249:
			return XOR("g");
		case WEAPON_TASER:
			return XOR("h");
		case WEAPON_FLASHBANG:
			return XOR("i");
		case WEAPON_HEGRENADE:
			return XOR("j");
		case WEAPON_SMOKEGRENADE:
			return XOR("k");
		case WEAPON_MOLOTOV:
			return XOR("l");
		case WEAPON_DECOY:
			return XOR("m");
		case WEAPON_INCGRENADE:
			return XOR("n");
		case WEAPON_C4:
			return XOR("o");
		case WEAPON_DEAGLE:
			return XOR("A");
		case WEAPON_ELITE:
			return XOR("B");
		case WEAPON_FIVESEVEN:
			return XOR("C");
		case WEAPON_GLOCK:
			return XOR("D");
		case WEAPON_HKP2000:
			return XOR("E");
		case WEAPON_P250:
			return XOR("F");
		case WEAPON_USP_SILENCER:
			return XOR("G");
		case WEAPON_TEC9:
			return XOR("H");
		case WEAPON_CZ75A:
			return XOR("I");
		case WEAPON_REVOLVER:
			return XOR("J");
		case WEAPON_MAC10:
			return XOR("K");
		case WEAPON_UMP45:
			return XOR("L");
		case WEAPON_BIZON:
			return XOR("M");
		case WEAPON_MP7:
			return XOR("N");
		case WEAPON_MP9:
			return XOR("O");
		case WEAPON_P90:
			return XOR("P");
		case WEAPON_GALILAR:
			return XOR("Q");
		case WEAPON_FAMAS:
			return XOR("R");
		case WEAPON_M4A1:
			return XOR("S");
		case WEAPON_M4A1_SILENCER:
			return XOR("T");
		case WEAPON_AUG:
			return XOR("U");
		case WEAPON_SG556:
			return XOR("V");
		case WEAPON_AK47:
			return XOR("W");
		case WEAPON_G3SG1:
			return XOR("X");
		case WEAPON_SCAR20:
			return XOR("Y");
		case WEAPON_AWP:
			return XOR("Z");
		default:
			return XOR("");
		}

	}
};
class BaseAttributableItem : public BaseEntity
{
private:
	using str_32 = char[32];
	template<typename FuncType>
	__forceinline static FuncType call(void* ppClass, int index)
	{
		int* pVTable = *(int**)ppClass;
		int dwAddress = pVTable[index];
		return (FuncType)(dwAddress);
	}
public:
	NETVAR("DT_BaseAttributableItem", "m_iAccountID", get_account_id, int);
	NETVAR("DT_BaseAttributableItem", "m_iItemIDLow", get_item_id, uint64_t);
	NETVAR("DT_BaseAttributableItem", "m_iItemIDHigh", get_item_id_high, uint32_t);
	NETVAR("DT_BaseAttributableItem", "m_iItemIDLow", get_item_id_low, uint32_t);
	NETVAR("DT_BaseAttributableItem", "m_iEntityQuality", get_entity_quality, int);
	NETVAR("DT_BaseAttributableItem", "m_szCustomName", get_custom_name, str_32);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackPaintKit", get_fallback_paint_kit, unsigned);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackSeed", get_fallback_seed, unsigned);
	NETVAR("DT_BaseAttributableItem", "m_flFallbackWear", get_fallback_wear, float);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackStatTrak", get_fallback_stat_trak, unsigned);
	NETVAR("DT_BaseAttributableItem", "m_iItemDefinitionIndex", get_item_definiton_index, short);
	NETVAR("DT_BaseAttributableItem", "m_OriginalOwnerXuidLow", get_xuid_low, int);
	NETVAR("DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh", get_xuid_high, int);

	void SetGloveModelIndex(int modelIndex)
	{
		return call<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
	}
};

class BaseViewModel : public BaseEntity
{
public:
	NETVAR("DT_BaseViewModel", "m_hOwner", Owner, int32_t);
	NETVAR("DT_BaseViewModel", "m_hWeapon", m_hWeapon, CHandle<BaseWeapon>);
	NETVAR("DT_BaseViewModel", "m_hWeapon", GetWeapon, CBaseHandle);
	NETVAR("DT_BaseViewModel", "m_nModelIndex", ViewModelIndex, int);
};

class BaseWeaponWorldModel : public BaseEntity
{
private:
	using str_32 = char[32];
	template<typename FuncType>
	__forceinline static FuncType call(void* ppClass, int index)
	{
		int* pVTable = *(int**)ppClass;
		int dwAddress = pVTable[index];
		return (FuncType)(dwAddress);
	}
public:
	inline int* ModelIndex()
	{
		return (int*)((DWORD)this + 0x258);
	}

	CBaseHandle2 WorldModel()
	{
		return *(CBaseHandle2*)((uintptr_t)this + 0x3254);
	}

};

