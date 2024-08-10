#pragma once

// used: surfacedata
#include "../../../client/interfaces/i_physics_surface_props.hpp"

namespace Penetration
{
	struct PenetrationInput_t {
		BaseEntity* pFrom;
		BaseEntity* pTarget;
		vec3_t  vecFrom;
		vec3_t  vecPos;
		float	flDamage;
		float   flDamagePen;
		bool	bCanPenetrate;
	};

	struct PenetrationOutput_t {
		BaseEntity* pTarget;
		float   flDamage;
		int     iHitgroup;
		bool    bPenetrated;

		__forceinline PenetrationOutput_t() : pTarget{ nullptr }, flDamage{ 0.f }, iHitgroup{ -1 }, bPenetrated{ false } { }
	};

	//// Get
	///* returns damage at point and simulated bullet data (if given) */
	//static float GetDamage(BaseEntity* pLocal, const vec3_t& vecFrom, const vec3_t& vecPoint, FireBulletData_t* pDataOut = nullptr);
	///* calculates damage factor */
	//static void ScaleDamage(const int iHitGroup, BaseEntity* pEntity, const float flWeaponArmorRatio, const float flWeaponHeadShotMultiplier, float& flDamage);
	///* simulates fire bullet to penetrate up to 4 walls, return true when hitting player */
	//static bool SimulateFireBullet(BaseEntity* pLocal, BaseWeapon* pWeapon, FireBulletData_t& data);

	float flScaleDamage(BaseEntity* pTarget, float flDamage, float flHeadshotMultipier, float flArmorRatio, int iHitgroup);
	bool  bRunPenetration(PenetrationInput_t* penInput, PenetrationOutput_t* penOutup);

	// Main
	//static bool TraceToExit(trace_t& enterTrace, trace_t& exitTrace, const vec3_t& vecPosition, const vec3_t& vecDirection, BaseEntity* pClipPlayer);
	//static bool HandleBulletPenetration(BaseEntity* pLocal, const CWeaponData* pWeaponData, const surfacedata_t* pEnterSurfaceData, FireBulletData_t& data);

	bool  bTraceToExit(const vec3_t& vecStart, const vec3_t& vecDirection, vec3_t& vecOut, trace_t* pEnterTrace, trace_t* pExitTrace);
	void  ClipTraceToPlayer(const vec3_t& vecStart, const vec3_t& vecEnd, uint32_t uiMask, trace_t* pTrace, BaseEntity* pTarget, const float flMinRange = 0.0f);
	void  ClipTraceToPlayers(const vec3_t& vecAbsStart, const vec3_t& vecAbsEnd, const unsigned int fMask, trace_filter* pFilter, trace_t* pTrace, const float flMinRange = 0.0f);
}

//extern Penetration penetration;