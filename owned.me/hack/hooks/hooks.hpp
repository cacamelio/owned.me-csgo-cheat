#pragma once

#include "../../client/utilities/detourhook.h"

namespace detour
{
	inline CDetourHook frame_stage_notify;
	inline CDetourHook create_move;
	inline CDetourHook paint_traverse;
	inline CDetourHook lock_cursor;
	inline CDetourHook post_entity;
	inline CDetourHook shutdown;
	inline CDetourHook post_screen_effects;
	inline CDetourHook draw_model;
	inline CDetourHook present;
	inline CDetourHook reset;
	inline CDetourHook override_config;
	inline CDetourHook on_screen_size_changed;
	inline CDetourHook list_leaves_in_box;
	inline CDetourHook is_hl_tv;
	inline CDetourHook render_smoke_overlay;
	inline CDetourHook effects_hud;
	inline CDetourHook view_model_fov;
	inline CDetourHook override_view;
	inline CDetourHook scene_end;
	inline CDetourHook emit_sound;
	inline CDetourHook in_prediction;
	inline CDetourHook run_command;
	inline CDetourHook cl_move;
}

class CParticleCollection;
class C_INIT_RandomColor {
	BYTE pad_0[92];
public:
	vec3_t	m_flNormColorMin;
	vec3_t	m_flNormColorMax;
};


namespace hooks 
{
	inline constexpr void* GetVFunc(void* thisptr, std::size_t nIndex)
	{
		return (void*)((*(std::uintptr_t**)thisptr)[nIndex]);
	}

	bool initialize();

	HRESULT	D3DAPI		hkPresent(IDirect3DDevice9Ex*, const RECT*, const RECT*, HWND, const RGNDATA*);
	HRESULT	D3DAPI		hkReset(IDirect3DDevice9Ex* device, D3DPRESENT_PARAMETERS* params);
	LRESULT WINAPI		WndProc(HWND wnd, uint32_t msg, WPARAM wp, LPARAM lp);

	bool	__fastcall	hkCreateMove(void*, int, float, UserCmd*);
	void	__fastcall	hkPaintTraverse(void*, int, unsigned int, bool, bool);
	void	__fastcall	hkLockCursor(void*, int);
	void	__fastcall	hkFrameStageNotify(void*, int, int);
	void	__fastcall	hkLevelInitPostEntity(void*, void*);
	void	__fastcall	hkLevelShutdown(void*, void*);
	int		__fastcall	hkDoPostScreenEffects(void*, int, void*);
	bool    __fastcall  hkOverrideConfig(void*, void*, MaterialSystemConfig_t*, bool);
	void	__fastcall	hkDrawModel(void*, int, DrawModelResults_t*, const DrawModelInfo_t&, matrix_t*, float*, float*, const vec3_t&, int);
	int		__fastcall  hkListLeavesInBox(std::uintptr_t ecx, std::uintptr_t edx, vec3_t& mins, vec3_t& maxs, unsigned short* list, int list_max);
	bool	__fastcall	hkIsHLTV(IVEngineClient* IEngineClient);
	void   __fastcall   hkRenderSmokeOverlay(bool unk);
	void   __fastcall	hkRender2DEffectsPostHUD(const CViewSetup& setup);
	float  __fastcall	hkViewModel();
	void   __fastcall   hkOverrideView(void* _this, void* _edx, CViewSetup* setup);
	void   __fastcall   hkSceneEnd(void* thisptr, void* edx);
	bool __fastcall   hkInPrediction(PlayerPrediction* prediction, uint32_t e);
	void   __fastcall	hkEmitSound(void* pThis, int edx, void* filter, int iEntIndex, int iChannel,
		const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample,
		float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
		const void* pOrigin, const void* pDirection, void* pUtlVecOrigins,
		bool bUpdatePositions, float soundtime, int speakerentity, int something);

	namespace vars
	{
		bool Hook();
		void UnHook();

		inline void* hCsmShadowGetInt;
		using CsmShadowGetInt = bool(__thiscall*)(void*);
		inline CsmShadowGetInt oCsmShadowGetInt;
		int		__fastcall  hkCsmShadowGetInt();
	
		inline void* ParticleCollectionSimulateAdr;
		inline PVOID oParticleCollectionSimulate;
		void __fastcall hkParticleCollectionSimulate(CParticleCollection* thisPtr, void* edx);
	}

	namespace player
	{
		bool Hook();
		void UnHook();

		inline void* DoExtraBoneProcessing;
		inline PVOID oDoExtraBoneProcessing;
		void	__fastcall	hkDoExtraBoneProcessing(void* ecx, void* edx, studio_hdr_t* hdr, vec3_t* pos, quaternion* q, const matrix_t& matrix, byte* boneComputed, void* context);
		
		inline void* StandardBlendingRules;
		inline PVOID oStandardBlendingRules;
		void	__fastcall	hkStandardBlendingRules(BaseEntity* player, int i, studio_hdr_t* hdr, vec3_t* pos, quaternion* q, float curtime, int boneMask);

	/*	inline void* SetupBonesAdr;
		inline PVOID oSetupBones;
		bool __fastcall hkSetupBones(void* ecx, void* edx, matrix_t* bone_to_world_out, int max_bones, int bone_bask, float current_time);
	*/
		using fnBuildTransformations = void(__thiscall*)(BaseEntity*, c_studio_hdr*, vec3_t*, quaternion*, const matrix_t&, int32_t, byte*);
		inline fnBuildTransformations oBuildTransformations;
		void __fastcall hkBuildTransformations(BaseEntity* player, uint32_t, c_studio_hdr* hdr, vec3_t* pos, quaternion* q, const matrix_t& transform, int32_t mask, byte* computed);
		
		bool __fastcall hkSetupBones(void* ecx, void* edx, matrix_t* bone_to_world_out, int max_bones, int bone_mask, float curtime);
		using fnbones = bool(__fastcall*)(void*, void*, matrix_t*, int, int, float);

		void __fastcall hkUpdateAnims(void* ecx, void* edx);
		using fnanims = void(__fastcall*)(void*, void*);

		void __stdcall call(int sequence_number, float sample_frametime, bool active, bool& send_packet);
		void __fastcall hook(void* ecx, void* edx, int sequence_number, float sample_frametime, bool active);
		using fn = void(__stdcall*)(int, float, bool);

		void __fastcall hkSetCollisionBounds(void* ecx, void* edx, vec3_t mins, vec3_t maxs);
		using fncolbound = void(__fastcall*)(void*, vec3_t, vec3_t);

		bool __fastcall hkWriteUsercmdDeltaToBuffer(void* lpEcx, void* lpEdx, int nSlot, bf_write* lpBuffer, int nFrom, int nTo, bool bIsNew);
		using fnwrite = bool(__thiscall*) (void*, int, bf_write*, int, int, bool);

		void __fastcall hkRunCommand(void* ecx, void* edx, BaseEntity* player, UserCmd* m_pcmd, PlayerMoveHelper* move_helper);
		using fnruncmd = void(__thiscall*)(void*, BaseEntity*, UserCmd*, PlayerMoveHelper*);

		void __fastcall hkPhysicSim(BaseEntity* ecx, void* edx);
		using fnphysic = void(__thiscall*)(BaseEntity*);

	}

}


inline hooks::player::fn oCreateMove = nullptr;
inline hooks::player::fnbones o_setup_bones = nullptr;
inline hooks::player::fnanims o_update_anims = nullptr;
inline hooks::player::fncolbound o_setcol_bounds = nullptr;
inline hooks::player::fnwrite o_write_user = nullptr;
inline hooks::player::fnruncmd o_run_command = nullptr;
inline hooks::player::fnphysic o_physic = nullptr;