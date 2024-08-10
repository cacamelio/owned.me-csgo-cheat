#pragma once

enum ESequenceActivity : int
{
	ACT_CSGO_NULL = 957,
	ACT_CSGO_DEFUSE,
	ACT_CSGO_DEFUSE_WITH_KIT,
	ACT_CSGO_FLASHBANG_REACTION,
	ACT_CSGO_FIRE_PRIMARY,
	ACT_CSGO_FIRE_PRIMARY_OPT_1,
	ACT_CSGO_FIRE_PRIMARY_OPT_2,
	ACT_CSGO_FIRE_SECONDARY,
	ACT_CSGO_FIRE_SECONDARY_OPT_1,
	ACT_CSGO_FIRE_SECONDARY_OPT_2,
	ACT_CSGO_RELOAD,
	ACT_CSGO_RELOAD_START,
	ACT_CSGO_RELOAD_LOOP,
	ACT_CSGO_RELOAD_END,
	ACT_CSGO_OPERATE,
	ACT_CSGO_DEPLOY,
	ACT_CSGO_CATCH,
	ACT_CSGO_SILENCER_DETACH,
	ACT_CSGO_SILENCER_ATTACH,
	ACT_CSGO_TWITCH,
	ACT_CSGO_TWITCH_BUYZONE,
	ACT_CSGO_PLANT_BOMB,
	ACT_CSGO_IDLE_TURN_BALANCEADJUST,
	ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING,
	ACT_CSGO_ALIVE_LOOP,
	ACT_CSGO_FLINCH,
	ACT_CSGO_FLINCH_HEAD,
	ACT_CSGO_FLINCH_MOLOTOV,
	ACT_CSGO_JUMP,
	ACT_CSGO_FALL,
	ACT_CSGO_CLIMB_LADDER,
	ACT_CSGO_LAND_LIGHT,
	ACT_CSGO_LAND_HEAVY,
	ACT_CSGO_EXIT_LADDER_TOP,
	ACT_CSGO_EXIT_LADDER_BOTTOM
};

enum EPoseParameters {
	STRAFE_YAW,
	STAND,
	LEAN_YAW,
	SPEED,
	LADDER_YAW,
	LADDER_SPEED,
	JUMP_FALL,
	MOVE_YAW,
	MOVE_BLEND_CROUCH,
	MOVE_BLEND_WALK,
	MOVE_BLEND_RUN,
	BODY_YAW,
	BODY_PITCH,
	AIM_BLEND_STAND_IDLE,
	AIM_BLEND_STAND_WALK,
	AIM_BLEND_STAND_RUN,
	AIM_BLEND_COURCH_IDLE,
	AIM_BLEND_CROUCH_WALK,
	DEATH_YAW
};

class matrix3x4a_t;
class c_base_animating;
class c_bone_accessor
{
public:
	const c_base_animating* pAnimating;		//0x00
	matrix3x4a_t* matBones;		//0x04
	int						nReadableBones;	//0x08
	int						nWritableBones;	//0x0C
}; // Size: 0x10

class AnimationLayer
{
public:
	float			flAnimationTime;		//0x00
	float			flFadeOut;				//0x04
	void*			pStudioHdr;				//0x08
	int				nDispatchedSrc;			//0x0C
	int				nDispatchedDst;			//0x10
	int				iOrder;					//0x14
	std::uintptr_t  nSequence;				//0x18
	float			flPrevCycle;			//0x1C
	float			flWeight;				//0x20
	float			flWeightDeltaRate;		//0x24
	float			flPlaybackRate;			//0x28
	float			flCycle;				//0x2C
	void*			pOwner;					//0x30
	int				nInvalidatePhysicsBits;	//0x34
}; // Size: 0x38

struct proceduralFoot
{
	vec3_t m_vecPosAnim;
	vec3_t m_vecPosAnimLast;
	vec3_t m_vecPosPlant;
	vec3_t m_vecPlantVel;
	float m_flLockAmount;
	float m_flLastPlantTime;
};//Size: 0x38(56)

class CSGOPlayerAnimState {
public:
	void Create(BaseEntity* pEntity)
	{
		using CreateAnimationStateFn = void(__thiscall*)(void*, BaseEntity*);
		static auto oCreateAnimationState = (CreateAnimationStateFn)(pattern::Scan(XOR("client.dll"), XOR("55 8B EC 56 8B F1 B9 ? ? ? ? C7"))); // @xref: "ggprogressive_player_levelup"

		if (oCreateAnimationState == nullptr)
			return;

		oCreateAnimationState(this, pEntity);
	}

	int32_t SelectSequenceFromActivityModifier(int32_t iActivity)
	{
		bool bIsPlayerDucked = m_flAnimDuckAmount > 0.55f;
		bool bIsPlayerRunning = m_flSpeedAsPortionOfWalkTopSpeed > 0.25f;

		int32_t iLayerSequence = -1;
		switch (iActivity)
		{
		case ACT_CSGO_JUMP:
		{
			iLayerSequence = 15 + static_cast <int32_t>(bIsPlayerRunning);
			if (bIsPlayerDucked)
				iLayerSequence = 17 + static_cast <int32_t>(bIsPlayerRunning);
		}
		break;

		case ACT_CSGO_ALIVE_LOOP:
		{
			iLayerSequence = 8;
			if (m_pWeaponLast != m_pWeapon)
				iLayerSequence = 9;
		}
		break;

		case ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING:
		{
			iLayerSequence = 6;
		}
		break;

		case ACT_CSGO_FALL:
		{
			iLayerSequence = 14;
		}
		break;

		case ACT_CSGO_IDLE_TURN_BALANCEADJUST:
		{
			iLayerSequence = 4;
		}
		break;

		case ACT_CSGO_LAND_LIGHT:
		{
			iLayerSequence = 20;
			if (bIsPlayerRunning)
				iLayerSequence = 22;

			if (bIsPlayerDucked)
			{
				iLayerSequence = 21;
				if (bIsPlayerRunning)
					iLayerSequence = 19;
			}
		}
		break;

		case ACT_CSGO_LAND_HEAVY:
		{
			iLayerSequence = 23;
			if (bIsPlayerDucked)
				iLayerSequence = 24;
		}
		break;

		case ACT_CSGO_CLIMB_LADDER:
		{
			iLayerSequence = 13;
		}
		break;
		default: break;
		}

		return iLayerSequence;
	}

	void SetLayerSequence(AnimationLayer* pAnimationLayer, int32_t iActivity);

	void Update(vec3_t angView)
	{
		using UpdateAnimationStateFn = void(_vectorcall*)(void*, void*, float, float, float, void*);
		static auto oUpdateAnimationState = (UpdateAnimationStateFn)(pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"))); // @xref: "%s_aim"

		if (oUpdateAnimationState == nullptr)
			return;

		oUpdateAnimationState(this, nullptr, 0.0f, angView[1], angView[0], nullptr);
	}

	void Reset()
	{
		using ResetAnimationStateFn = void(__thiscall*)(void*);
		static auto oResetAnimationState = (ResetAnimationStateFn)(pattern::Scan(XOR("client.dll"), XOR("56 6A 01 68 ? ? ? ? 8B F1"))); // @xref: "player_spawn"

		if (oResetAnimationState == nullptr)
			return;

		oResetAnimationState(this);
	}

	std::byte pad[4];
	//PAD(4);
	bool					m_bFirstRunSinceInit; //4 = 0x4
	std::byte				pad1[91]; //PAD(91); //5
	void* m_pBaseEntity; //96 = 0x60
	void* m_pWeapon; //100 = 0x64
	void* m_pWeaponLast; //104 = 0x68

	float					m_flLastUpdateTime; //108 = 0x6C LastClientSideAnimationUpdateTime
	int						m_nLastUpdateFrame; //112 = 0x70 LastClientSideAnimationUpdateFramecount
	float					m_flLastUpdateIncrement; //116 = 0x74

	float					m_flEyeYaw; //120 = 0x78
	float					m_flEyePitch; //124 = 0x7C
	float					m_flFootYaw; //128 = 0x80 //GoalFeetYaw;
	float					m_flFootYawLast; //132 = 0x84 //CurrentFeetYaw
	float					m_flMoveYaw; //136 = 0x88
	float					m_flMoveYawIdeal; //140 = 0x8C
	float					m_flMoveYawCurrentToIdeal; //144 = 0x90
	float					m_flTimeToAlignLowerBody; //148 = 0x94

	float					m_flPrimaryCycle; //152 = 0x98
	float					m_flMoveWeight; //156 = 09C
	float					m_flMoveWeightSmoothed; //T 160
	float					m_flAnimDuckAmount; //T 164 duckAmount
	float					m_flDuckAdditional; //168 = 0xA8 landingDuckAmount
	float					m_flRecrouchWeight; //T 172

	vec3_t					m_vecPositionCurrent; //T 176(X),180(Y),184(Z) Origin
	vec3_t					m_vecPositionLast; //T 188(X),192(Y),196(Z) LastOrigin

	vec3_t					m_vecVelocity; //T 200(X),204(Y),208(Z)
	vec3_t					m_vecVelocityNormalized; //T 212(X),216(Y),220(Z)
	vec3_t					m_vecVelocityNormalizedNonZero; //T 224(X),228(Y),232(Z)
	float					m_flVelocityLengthXY; //236 = 0xEC
	float					m_flVelocityLengthZ; // T 240

	float					m_flSpeedAsPortionOfRunTopSpeed; //T 244
	float					m_flSpeedAsPortionOfWalkTopSpeed; //T 248 footSpeed
	float					m_flSpeedAsPortionOfCrouchTopSpeed; //T 252 footSpeed2

	float					m_flDurationMoving; //T 256 TimeSinceStartedMoving
	float					m_flDurationStill; //T 260 TimeSinceStoppedMoving

	bool					m_bOnGround; //264 = 0x108 OnGround
	bool					m_bLanding; //265 = 0x109 InHitGroundAnimation
	std::byte pad2[2]; //PAD(2);
	float					m_flJumpToFall; //268
	float					m_flDurationInAir; //272 = 0x110
	float					m_flLeftGroundHeight; //276 = 0x114
	float					m_flLandAnimMultiplier; //280 = 0x118 HeadHeightOrOffsetFromHittingGroundAnimation

	float					m_flWalkToRunTransition; //284 = 0x11C stopToFullRunningFraction

	bool					m_bLandedOnGroundThisFrame; //288 = 0x120
	bool					m_bLeftTheGroundThisFrame; //289 = 0x121
	std::byte pad3[2]; //PAD(2);
	float					m_flInAirSmoothValue; //292 = 0x124

	bool					m_bOnLadder; //296 = 0x128
	std::byte pad4[3]; //PAD(3);
	float					m_flLadderWeight; //300 = 0x12C
	float					m_flLadderSpeed; //304 = 0x130

	bool					m_bWalkToRunTransitionState; //308 = 0x134

	bool					m_bDefuseStarted; //T 309
	bool					m_bPlantAnimStarted;//T 310
	bool					m_bTwitchAnimStarted;//T 311
	bool					m_bAdjustStarted;//T 312
	std::byte pad5[3]; //PAD(3);
	CUtlVector<uint16_t>	m_ActivityModifiers;//T 316 (size 20)

	float					m_flNextTwitchTime;//T 336

	float					m_flTimeOfLastKnownInjury;//T 340

	float					m_flLastVelocityTestTime; //344 = 0x158
	vec3_t					m_vecVelocityLast; //T 348(X), 352(Y), 356(Z)
	vec3_t					m_vecTargetAcceleration; //T 360(X), 364(Y), 368(Z)
	vec3_t					m_vecAcceleration; //T 372(X), 376(Y), 380(Z)
	float					m_flAccelerationWeight; //T 384

	float					m_flAimMatrixTransition;//T 388
	float					m_flAimMatrixTransitionDelay; //T 392

	bool					m_bFlashed;// T 396
	std::byte pad6[3];//PAD(3);
	float					m_flStrafeChangeWeight; //400 = 0x190
	float					m_flStrafeChangeTargetWeight; // T 404
	float					m_flStrafeChangeCycle; //408 = 0x198
	int						m_nStrafeSequence; //412 = 0x19C
	bool					m_bStrafeChanging; //416 = 0x1A0
	std::byte pad7[3]; //PAD(3);
	float					m_flDurationStrafing; //420 = 0x1A4

	float					m_flFootLerp; // T 424

	bool					m_bFeetCrossed; // T 428

	bool					m_bPlayerIsAccelerating; //429 = 0x1AD
	std::byte pad8[2]; //PAD(2);
	std::byte pad9[240]; //PAD(240); //animstatePoseParamCache m_tPoseParamMappings[20];//T 432

	float					m_flDurationMoveWeightIsTooHigh; //672 = 0x2A0
	float					m_flStaticApproachSpeed; //676 =  0x2A4

	int						m_nPreviousMoveState; //680 = 0x2A8
	float					m_flStutterStep; //684 = 0x2AC

	float					m_flActionWeightBiasRemainder; //688 = 0x2B0 //Literally useless??

	proceduralFoot	m_footLeft; //692 = 0x2B4
	proceduralFoot	m_footRight;//T 748

	float					m_flCameraSmoothHeight; //804 = 0x324
	bool					m_bSmoothHeightValid; //808 = 0x328
	std::byte pad10[3]; //PAD(3);
	float					m_flLastTimeVelocityOverTen; //T 812 = 0x329
	std::byte pad11[4]; // Thanks UKnown Chit!!
	float					m_flAimYawMin; //T 816
	float					m_flAimYawMax; //T 820
	float					m_flAimPitchMin; //T 824
	float					m_flAimPitchMax; //T 828

	int						m_nAnimstateModelVersion; //832 = 0x340

	float& yaw_desync_adjustment()
	{
		return *(float*)((uintptr_t)this + 0x334);
	}
};
