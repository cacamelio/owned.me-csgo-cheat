#pragma once

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/server/player_command.cpp#L315
class Prediction
{
public:
	Prediction()
	{
		iPredictionRandomSeed = *reinterpret_cast<int**>(pattern::Scan(XOR("client.dll"), XOR("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")) + 0x2); // @xref: "SelectWeightedSequence"
		pSetPredictionEntity = *reinterpret_cast<BaseEntity**>(pattern::Scan(XOR("client.dll"), XOR("89 35 ? ? ? ? F3 0F 10 48 20")) + 0x2);
	}

	// Get
	void Start(UserCmd* pCmd, BaseEntity* pLocal);
	void End(UserCmd* pCmd, BaseEntity* pLocal);

	/*
	 * required cuz prediction works on frames, not ticks
	 * corrects tickbase if your framerate goes below tickrate and m_nTickBase won't update every tick
	 */
	int GetTickbase(UserCmd* pCmd, BaseEntity* pLocal);

	// Values
	/* prediction seed */
	int* iPredictionRandomSeed = nullptr;
	/* current predictable entity */
	BaseEntity* pSetPredictionEntity = nullptr;
	/* encapsulated input parameters to player movement */
	PlayerMoveData moveData = { };

	/* backup */
	float flSpread = 0.f;
	float flInaccuracy = 0.f;
	float flWeaponRange = 0.f;
	float flOldCurrentTime = 0.f;
	float flOldFrameTime = 0.f;
	int iOldTickCount = 0;
	vec3_t vecOldVelocity = vec3_t(0, 0, 0);
	float flNewCurrentTime = 0.f;
	float flNewFrameTime = 0.f;
	int iNewTickCount = 0;
	vec3_t vecNewVelocity = vec3_t(0, 0, 0);
};

extern Prediction prediction;

namespace net_compression
{
	struct stored_data_t
	{
		int tick_base = -1;
		float vel_modifier = -1.f;
		vec3_t punch = {};
		vec3_t punch_vel = {};
		vec3_t view_offset = {};
	};

	void store();
	void set(int stage);
}