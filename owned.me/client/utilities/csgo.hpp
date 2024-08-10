#pragma warning( disable : 2459 )
#pragma warning( disable : 2479 )
#pragma warning( disable : 249 )
#pragma warning( disable : 2468,23 )
#pragma once

#define NOMINMAX

#include <windows.h>
#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <array>
#include <fstream>
#include <istream>
#include <unordered_map>
#include <intrin.h>
#include <filesystem>
#include <minwindef.h>
#include <Psapi.h>
#include <cassert>

#include "../utilities/singleton.hpp"
#include "../utilities/fnv.hpp"
#include "../utilities/utilities.hpp"
#include "../../client/minhook/minhook.h"
#include "../interfaces/interfaces.hpp"
#include "../../sdk/sdk.hpp"
#include "../../hack/hooks/hooks.hpp"
#include "../../client/math/math.hpp"
#include "../../client/utilities/console/console.hpp"
#include "../utilities/csgo.hpp"
#include "xor.h"
#include "../json/json.hpp"
#include "../../client/utilities/renderer/render.hpp"
#include "../../client/utilities/tinyformat.h"
#include "../../client/utilities/module.h"
#include "../../menu/cfg.hpp"
#include "../../client/utilities/log.h"
#include "../../hack/hooks/events.hpp"

#define sig_client_state XOR("A1 ? ? ? ? 8B 80 ? ? ? ? C3")
#define sig_directx XOR("A1 ? ? ? ? 50 8B 08 FF 51 0C")
#define sig_input XOR("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")
#define sig_glow_manager XOR("0F 11 05 ? ? ? ? 83 C8 01")
#define sig_pmovehelper XOR("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01")
#define sig_weapon_data XOR("8B 35 ? ? ? ? FF 10 0F B7 C0")
#define sig_set_angles XOR("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1")
#define sig_pred_random_seed XOR("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")
#define c(n, s) auto (n) = reinterpret_cast<char*>(alloca(((s).first.size() + 1) * sizeof(char))); \
	for (size_t i = 0; i < (s).first.size(); i++) \
        (n)[i] = (s).first[i] ^ (s).second; \
    (n)[(s).first.size()] = '\0'

enum ROTATE_MODE
{
	ROTATE_SERVER,
	ROTATE_LEFT,
	ROTATE_CENTER,
	ROTATE_RIGHT,
	ROTATE_LOW_LEFT,
	ROTATE_LOW_RIGHT
};


class C_LagRecord
{
public:
	void Apply( BaseEntity* player )
	{
		player->SimulationTime( ) = m_SimulationTime;
		player->Mins( ) = m_Mins;
		player->Maxs( ) = m_Maxs;
		player->Origin( ) = m_Origin;
		player->SetAbsOrigin( m_Origin );
		player->SetAbsAngles( m_AbsAngles );

		std::memcpy( player->AnimOverlays( ), m_AnimationLayers.data( ), m_AnimationLayers.size( ) * sizeof( AnimationLayer ) );
		player->SetPoseParameters( m_PoseParameters.data( ) );
	}
	
	BaseEntity* m_pEntity;

	int m_nEntIndex = 0;
	float_t m_SimulationTime = 0.0f;
	float_t m_LowerBodyYaw = 0.0f;
	float_t m_DuckAmount = 0.0f;
	float_t m_BruteYaw = 0.0f;

	int32_t m_UpdateDelay = 0;
	int32_t m_RotationMode = ROTATE_SERVER;
	int32_t m_Flags = 0;
	int32_t m_AdjustTick = 0;

	vec3_t m_EyeAngles = vec3_t(0, 0, 0);
	vec3_t m_AbsAngles = vec3_t(0, 0, 0);
	vec3_t m_Velocity = vec3_t(0, 0, 0);
	vec3_t m_Origin = vec3_t(0, 0, 0);
	vec3_t m_AbsOrigin = vec3_t(0, 0, 0);
	vec3_t m_Mins = vec3_t(0, 0, 0);
	vec3_t m_Maxs = vec3_t(0, 0, 0);

	std::array < std::array < AnimationLayer, 13 >, 6 > m_AnimationLayers = { };
	std::array < float_t, 24 > m_PoseParameters = { };
	std::array < std::array < matrix_t, MAXSTUDIOBONES >, 4 > m_Matricies = { };

	bool m_bIsShooting = false;
	bool m_bAnimResolved = false;
	bool m_bJumped = false;
};

namespace g
{
	using AngleMatrix_t = void(__fastcall*)(const vec3_t&, matrix_t&);
	inline AngleMatrix_t AngleMatrix;
	inline HMODULE hDLL;
	inline vec2_t vScreenSize;
	inline BaseEntity* pLocalPlayer;
	inline BaseWeapon* pLocalWeapon;
	inline CWeaponData* pLocalWeaponData;
	inline UserCmd* pCmd;
	inline bool bTakingInput;
	inline bool bPeekState;
	inline int iScaling;
	inline int iWidth;
	inline int iHeight;
	inline int iButtons;
	inline int iOldButtons;
	inline int iFlags;
	inline bool bPressingMove;
	inline int iStage;
	inline bool bAlive;
	inline bool* bSendPacket = nullptr;
	inline std::string pUser;
	inline float StoredAlpha;
	inline HWND hWindow;
	inline WNDPROC OldWndProc;
	inline bool bUpdateSound;
	inline float fLatency;
	inline bool bUpdateMatrix;
	inline float nextLbyUpdate = 0.0f;
	inline bool bUpdateAnims;
	inline bool bUpdateBones;
	inline bool bUpdateCollision;
	inline vec3_t vFakeAng;
	inline vec3_t vRealAng;
	inline vec3_t m_angExtendedAngles;
	inline bool bExtendingAngles;
	inline bool bFakeDuck;
	inline vec3_t vStrafeAngles;

	inline int iTickRate;

	inline int tick_base = 0;

	inline float flLastVelocityModifier;

	inline bool bInCreatemove;

	inline std::array < std::deque < C_LagRecord >, 65 > m_CachedPlayerRecords;

	inline Address UTIL_TraceLine;
	inline Address CTraceFilterSimple_vmt;
	inline Address CTraceFilterSkipTwoEntities_vmt;
	inline Address aHasC4;
	inline Address SetAbsOrigin;
	inline Address LockStudioHdr;
	inline Address SmokeCount;
	inline size_t studioHdr;
	inline size_t BoneAccessor;
	inline size_t PlayerAnimState;

	inline bool bClearKillFeed;
}

namespace client 
{
	void initialize();
	void uninitialize();
	void OnMapload();
	void MouseDelta();
	void Watermark();

	inline PE::Module m_kernel32_dll;
	inline PE::Module m_user32_dll;
	inline PE::Module m_shell32_dll;
	inline PE::Module m_shlwapi_dll;
	inline PE::Module m_client_dll;
	inline PE::Module m_engine_dll;
	inline PE::Module m_vstdlib_dll;
	inline PE::Module m_tier0_dll;
	inline PE::Module m_serverbrowser_dll;
	inline PE::Module m_shaderapidx9_dll;
}

