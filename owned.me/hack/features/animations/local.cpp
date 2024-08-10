#include "../features.hpp"

void CSGOPlayerAnimState::SetLayerSequence(AnimationLayer* pAnimationLayer, int32_t iActivity) 
{
	{
		int32_t iSequence = this->SelectSequenceFromActivityModifier(iActivity);
		if (iSequence < 2)
			return;

		pAnimationLayer->flCycle = 0.0f;
		pAnimationLayer->flWeight = 0.0f;
		pAnimationLayer->nSequence = iSequence;
		pAnimationLayer->flPlaybackRate = reinterpret_cast< BaseEntity* >(m_pBaseEntity)->GetLayerSequenceCycleRate(pAnimationLayer, iSequence);
	}
}

void BuildMatrix(BaseEntity* pPlayer, matrix_t* aMatrix, bool bSafeMatrix)
{
	std::array < AnimationLayer, 13 > aAnimationLayers;

	float_t flCurTime = interfaces::globals->cur_time;
	int32_t iFrameCount = interfaces::globals->frame_count;

	interfaces::globals->cur_time = utilities::TICKS_TO_TIME(pPlayer->GetTickBase() + 1);
	interfaces::globals->frame_count = INT_MAX;

	int32_t nClientEffects = pPlayer->m_nClientEffects();
	int32_t nLastSkipFramecount = pPlayer->m_nLastSkipFramecount();
	int32_t nOcclusionMask = pPlayer->m_nOcclusionMask();
	int32_t nOcclusionFrame = pPlayer->m_nOcclusionFrame();
	int32_t iEffects = pPlayer->Effects();
	bool bJiggleBones = pPlayer->m_bJiggleBones();
	vec3_t vecAbsOrigin = pPlayer->AbsOrigin();

	int32_t iMask = BONE_USED_BY_ANYTHING;

	std::memcpy(aAnimationLayers.data(), pPlayer->AnimOverlays(), sizeof(AnimationLayer) * 13);

	pPlayer->InvalidateBoneCache();
	pPlayer->BoneAccessor().m_ReadableBones = NULL;
	pPlayer->BoneAccessor().m_WritableBones = NULL;

	pPlayer->m_nOcclusionFrame() = 0;
	pPlayer->m_nOcclusionMask() = 0;
	pPlayer->m_nLastSkipFramecount() = 0;

	pPlayer->Effects() |= EF_NOINTERP;
	pPlayer->m_nClientEffects() |= 2;
	pPlayer->m_bJiggleBones() = false;

	pPlayer->AnimOverlays()[12].flWeight = 0.0f;

	g::bUpdateBones = true;
	pPlayer->SetupBones(aMatrix, MAXSTUDIOBONES, iMask, utilities::TICKS_TO_TIME(pPlayer->GetTickBase() + 1));
	g::bUpdateBones = false;

	pPlayer->m_nClientEffects() = nClientEffects;
	pPlayer->m_bJiggleBones() = bJiggleBones;
	pPlayer->Effects() = iEffects;
	pPlayer->m_nLastSkipFramecount() = nLastSkipFramecount;
	pPlayer->m_nOcclusionFrame() = nOcclusionFrame;
	pPlayer->m_nOcclusionMask() = nOcclusionMask;

	std::memcpy(pPlayer->AnimOverlays(), aAnimationLayers.data(), sizeof(AnimationLayer) * 13);

	interfaces::globals->cur_time = flCurTime;
	interfaces::globals->frame_count = iFrameCount;
}

void localAnims::work()
{
	float_t flCurtime = interfaces::globals->cur_time;
	float_t flFrameTime = interfaces::globals->frame_time;

	if (g::pLocalPlayer->SpawnTime() != localData.spawnTime)
	{
		localData.iFlags[0] = localData.iFlags[1] = g::pLocalPlayer->Flags();
		localData.moveType[0] = localData.moveType[1] = g::pLocalPlayer->MoveType();
		localData.spawnTime = g::pLocalPlayer->SpawnTime();

		std::memcpy(&localData.fakeAnimState, g::pLocalPlayer->AnimState(), sizeof(CSGOPlayerAnimState));
		std::memcpy(localData.fakeLayers.data(), g::pLocalPlayer->AnimOverlays(), sizeof(AnimationLayer) * 13);
		std::memcpy(localData.fakePoseParams.data(), g::pLocalPlayer->PoseParameters().data(), sizeof(float_t) * 24);
	}

	if (!*g::bSendPacket)
	{
		if (g::pCmd->buttons & IN_ATTACK)
		{
			bool bIsRevolver = false;
			if (g::pLocalWeapon)
				bIsRevolver = g::pLocalWeapon->ItemDefinitionIndex() == WEAPON_REVOLVER;

			if (g::pLocalPlayer->CanFire(1, bIsRevolver))
			{
				if (!*g::bSendPacket)
				{
					localData.didShotinCycle = true;
					localData.angShotChoked = g::pCmd->viewangles;
				}
			}
		}
	}

	int32_t iFlags = g::pLocalPlayer->Flags();
	float_t flLowerBodyYaw = g::pLocalPlayer->LowerBodyYaw();
	float_t flDuckSpeed = g::pLocalPlayer->DuckSpeed();
	float_t flDuckAmount = g::pLocalPlayer->DuckAmount();
	vec3_t angVisualAngles = g::pLocalPlayer->m_angVisualAngles();

	interfaces::globals->cur_time = utilities::TICKS_TO_TIME(g::pLocalPlayer->GetTickBase());
	interfaces::globals->frame_time = interfaces::globals->interval_per_tick;

	g::pLocalPlayer->AbsVelocity() = g::pLocalPlayer->Velocity();
	g::pLocalPlayer->m_angVisualAngles() = g::pCmd->viewangles;

	static auto weaponRecoil = interfaces::console->FindVar(XOR("weapon_recoil_scale"));

	g::pLocalPlayer->ThirdpersonRecoil() = g::pLocalPlayer->AimPunchAngle().x * weaponRecoil->GetFloat();
	if ( interfaces::globals->realtime - localData.shotTime <= 0.25f)
		if (*g::bSendPacket)
			g::pLocalPlayer->m_angVisualAngles() = localData.angForcedAngles;

	if (localData.didShotinCycle)
		if (*g::bSendPacket)
			g::pLocalPlayer->m_angVisualAngles() = localData.angShotChoked;

	g::pLocalPlayer->m_angVisualAngles().z = 0.0f;

	g::pLocalPlayer->AnimState()->m_nLastUpdateFrame = -1;

	for (int iLayer = 0; iLayer < 13; iLayer++)
		g::pLocalPlayer->AnimOverlays()[iLayer].pOwner = g::pLocalPlayer;
	doAnimEvent(0);

	bool bClientSideAnimation = g::pLocalPlayer->ClientSideAnimation();
	g::pLocalPlayer->ClientSideAnimation() = true;

	g::bUpdateAnims = true;
	g::pLocalPlayer->UpdateClientSideAnimation();
	g::bUpdateAnims = false;

	g::pLocalPlayer->ClientSideAnimation() = bClientSideAnimation;

	std::memcpy(localData.poseParams.data(), g::pLocalPlayer->PoseParameters().data(), sizeof(float_t) * 24);
	std::memcpy(localData.animLayers.data(), g::pLocalPlayer->AnimOverlays(), sizeof(AnimationLayer) * 13);

	g::pLocalPlayer->Flags() = iFlags;
	g::pLocalPlayer->DuckAmount() = flDuckAmount;
	g::pLocalPlayer->DuckSpeed() = flDuckSpeed;
	g::pLocalPlayer->LowerBodyYaw() = flLowerBodyYaw;
	g::pLocalPlayer->m_angVisualAngles() = angVisualAngles;

	if (*g::bSendPacket)
	{
		CSGOPlayerAnimState AnimationState;
		std::memcpy(&AnimationState, g::pLocalPlayer->AnimState(), sizeof(CSGOPlayerAnimState));

		vec3_t angBonesAngles = g::pCmd->viewangles;
		if (g::bExtendingAngles)
			angBonesAngles = g::m_angExtendedAngles;

		g::pLocalPlayer->EyeAngles() = angBonesAngles;
		g::pLocalPlayer->m_angVisualAngles() = angBonesAngles;

		BuildMatrix(g::pLocalPlayer, localData.mainBones.data(), false);

		std::memcpy(g::pLocalPlayer->AnimOverlays(), getFakeAnimLayers().data(), sizeof(AnimationLayer) * 13);
		std::memcpy(g::pLocalPlayer->AnimState(), &localData.fakeAnimState, sizeof(CSGOPlayerAnimState));
		std::memcpy(g::pLocalPlayer->PoseParameters().data(), localData.fakePoseParams.data(), sizeof(float_t) * 24);

		int32_t iSimulationTicks = interfaces::clientstate->choked_commands + 1;
		for (int32_t iSimulationTick = 1; iSimulationTick <= iSimulationTicks; iSimulationTick++)
		{
			int32_t iTickCount = g::pLocalPlayer->GetTickBase() - (iSimulationTicks - iSimulationTick);
			interfaces::globals->cur_time = utilities::TICKS_TO_TIME(iTickCount);
			interfaces::globals->frame_time = interfaces::globals->interval_per_tick;

			g::pLocalPlayer->AbsVelocity() = g::pLocalPlayer->Velocity();
			g::pLocalPlayer->ThirdpersonRecoil() = g::pLocalPlayer->AimPunchAngle().x * weaponRecoil->GetFloat();

			g::pLocalPlayer->m_angVisualAngles() = g::pCmd->viewangles;
			if ((iSimulationTicks - iSimulationTick) < 1)
			{
				if ( interfaces::globals->realtime - localData.shotTime <= 0.25f)
					g::pLocalPlayer->m_angVisualAngles() = localData.angForcedAngles;

				if (localData.didShotinCycle) 
					g::pLocalPlayer->m_angVisualAngles() = localData.angShotChoked;
				

				g::pLocalPlayer->m_angVisualAngles().z = 0.0f;
			}

			g::pLocalPlayer->AnimState()->m_nLastUpdateFrame = -1488;

			for (int iLayer = 0; iLayer < 13; iLayer++)
				g::pLocalPlayer->AnimOverlays()[iLayer].pOwner = g::pLocalPlayer;
			doAnimEvent(1);

			bool bClientSideAnimation = g::pLocalPlayer->ClientSideAnimation();
			g::pLocalPlayer->ClientSideAnimation() = true;

			g::bUpdateAnims = true;
			g::pLocalPlayer->UpdateClientSideAnimation();
			g::bUpdateAnims = false;

			g::pLocalPlayer->ClientSideAnimation() = bClientSideAnimation;
		}

		g::pLocalPlayer->EyeAngles() = g::pCmd->viewangles;
		g::pLocalPlayer->m_angVisualAngles() = g::pCmd->viewangles;

		BuildMatrix(g::pLocalPlayer, localData.desyncBones.data(), false);

		std::memcpy(localData.lagBones.data(), localData.desyncBones.data(), sizeof(matrix_t) * MAXSTUDIOBONES);
		std::memcpy(&localData.fakeAnimState, g::pLocalPlayer->AnimState(), sizeof(CSGOPlayerAnimState));
		std::memcpy(localData.fakeLayers.data(), g::pLocalPlayer->AnimOverlays(), sizeof(AnimationLayer) * 13);
		std::memcpy(localData.fakePoseParams.data(), g::pLocalPlayer->PoseParameters().data(), sizeof(float_t) * 24);
		std::memcpy(g::pLocalPlayer->AnimOverlays(), getAnimLayers().data(), sizeof(AnimationLayer) * 13);
		std::memcpy(g::pLocalPlayer->AnimState(), &AnimationState, sizeof(CSGOPlayerAnimState));
		std::memcpy(g::pLocalPlayer->PoseParameters().data(), localData.poseParams.data(), sizeof(float_t) * 24);

		for (int i = 0; i < MAXSTUDIOBONES; i++)
			localData.vecBoneOrig[i] = g::pLocalPlayer->AbsOrigin() - localData.mainBones[i].get_origin();

		for (int i = 0; i < MAXSTUDIOBONES; i++)
			localData.fakeBoneOrig[i] = g::pLocalPlayer->AbsOrigin() - localData.desyncBones[i].get_origin();

		localData.didShotinCycle = false;
		localData.angShotChoked = vec3_t(0, 0, 0);
	}

	g::pLocalPlayer->Flags() = iFlags;
	g::pLocalPlayer->DuckAmount() = flDuckAmount;
	g::pLocalPlayer->DuckSpeed() = flDuckSpeed;
	g::pLocalPlayer->LowerBodyYaw() = flLowerBodyYaw;
	g::pLocalPlayer->m_angVisualAngles() = angVisualAngles;

	interfaces::globals->cur_time = flCurtime;
	interfaces::globals->frame_time = flFrameTime;
}

void localAnims::setupShotPos()
{
	std::memcpy(g::pLocalPlayer->AnimOverlays(), localAnims::getAnimLayers().data(), sizeof(AnimationLayer) * 13);
	std::memcpy(g::pLocalPlayer->PoseParameters().data(), localData.poseParams.data(), sizeof(float_t) * 24);

	float flOldBodyPitch = g::pLocalPlayer->PoseParameters()[BODY_PITCH];
	vec3_t vecOldOrigin = g::pLocalPlayer->AbsOrigin();

	g::pLocalPlayer->SetAbsAngles(vec3_t(0.0f, g::pLocalPlayer->AnimState()->m_flFootYaw, 0.0f));
	g::pLocalPlayer->SetAbsOrigin(g::pLocalPlayer->Origin());

	matrix_t aMatrix[MAXSTUDIOBONES];

	g::pLocalPlayer->PoseParameters()[BODY_PITCH] = (g::pLocalPlayer->EyeAngles().x + 89.0f) / 178.0f;
	BuildMatrix(g::pLocalPlayer, aMatrix, true);
	g::pLocalPlayer->PoseParameters()[BODY_PITCH] = flOldBodyPitch;

	g::pLocalPlayer->SetAbsOrigin(vecOldOrigin);
	std::memcpy(g::pLocalPlayer->BoneCache().Base(), aMatrix, sizeof(matrix_t) * g::pLocalPlayer->BoneCache().Count());

	localData.vecShootPos = g::pLocalPlayer->GetShootPosition();
}

void localAnims::doAnimEvent(int type)
{
	if (antiaim::freezeCheck || (g::pLocalPlayer->Flags() & FL_FROZEN))
	{
		localData.moveType[type] = MOVETYPE_NONE;
		localData.iFlags[type] = FL_ONGROUND;
	}

	AnimationLayer* pLandOrClimbLayer = &g::pLocalPlayer->AnimOverlays()[5];
	if (!pLandOrClimbLayer)
		return;

	AnimationLayer* pJumpOrFallLayer = &g::pLocalPlayer->AnimOverlays()[4];
	if (!pJumpOrFallLayer)
		return;

	if (localData.moveType[type] != MOVETYPE_LADDER && g::pLocalPlayer->MoveType() == MOVETYPE_LADDER)
		g::pLocalPlayer->AnimState()->SetLayerSequence(pLandOrClimbLayer, ACT_CSGO_CLIMB_LADDER);
	else if (localData.moveType[type] == MOVETYPE_LADDER && g::pLocalPlayer->MoveType() != MOVETYPE_LADDER)
		g::pLocalPlayer->AnimState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_FALL);
	else
	{
		if (g::pLocalPlayer->Flags() & FL_ONGROUND)
		{
			if (!(localData.iFlags[type] & FL_ONGROUND))
				g::pLocalPlayer->AnimState()->SetLayerSequence(pLandOrClimbLayer, g::pLocalPlayer->AnimState()->m_flDurationInAir > 1.0f && type == 0 ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT);
		}
		else if (localData.iFlags[type] & FL_ONGROUND)
		{
			if (g::pLocalPlayer->Velocity().z > 0.0f)
				g::pLocalPlayer->AnimState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_JUMP);
			else
				g::pLocalPlayer->AnimState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_FALL);
		}
	}

	localData.moveType[type] = g::pLocalPlayer->MoveType();
	localData.iFlags[type] = g::pLocalPlayer->Flags();
}

bool localAnims::getCachedMatrix(matrix_t* matrix)
{
	std::memcpy(matrix, localData.mainBones.data(), sizeof(matrix_t) * g::pLocalPlayer->BoneCache().Count());
	return true;
}

void localAnims::onUpdateClientAnims()
{
	for (int i = 0; i < MAXSTUDIOBONES; i++)
		localData.mainBones[i].set_origin(g::pLocalPlayer->AbsOrigin() - localData.vecBoneOrig[i]);

	for (int i = 0; i < MAXSTUDIOBONES; i++)
		localData.desyncBones[i].set_origin(g::pLocalPlayer->AbsOrigin() - localData.fakeBoneOrig[i]);

	std::memcpy(g::pLocalPlayer->BoneCache().Base(), localData.mainBones.data(), sizeof(matrix_t) * g::pLocalPlayer->BoneCache().Count());
	std::memcpy(g::pLocalPlayer->BoneArrayForWrite(), localData.mainBones.data(), sizeof(matrix_t) * g::pLocalPlayer->BoneCache().Count());

	return g::pLocalPlayer->SetupBones_AttachemtnHelper();
}

void localAnims::resetData()
{
	localData.desyncBones = {};
	localData.mainBones = {};

	localData.angShotChoked = vec3_t(0, 0, 0);
	localData.vecBoneOrig.fill(vec3_t(0, 0, 0));
	localData.fakeBoneOrig.fill(vec3_t(0, 0, 0));

	localData.didShotinCycle = false;

	localData.animLayers.fill(AnimationLayer());
	localData.fakeLayers.fill(AnimationLayer());

	localData.poseParams.fill(0.0f);
	localData.fakePoseParams.fill(0.0f);

	localData.shotTime = 0.0f;
	localData.angForcedAngles = vec3_t(0, 0, 0);

	localData.loweBodyYaw = 0.0f;
	localData.nextLBYupdate = 0.0f;
	localData.spawnTime = 0.0f;

	localData.iFlags[0] = localData.iFlags[1] = 0;
	localData.moveType[0] = localData.moveType[1] = 0;
}

std::array<matrix_t, MAXSTUDIOBONES> localAnims::getDesyncMatrix()
{
	return localData.desyncBones;
}

std::array<matrix_t, MAXSTUDIOBONES> localAnims::getLagMatrix()
{
	return localData.lagBones;
}

std::array<AnimationLayer, 13> localAnims::getAnimLayers()
{
	std::array<AnimationLayer, 13> output;

	std::memcpy(output.data(), g::pLocalPlayer->AnimOverlays(), sizeof(AnimationLayer) * 13);
	std::memcpy(&output.at(4), &localData.animLayers.at(4), sizeof(AnimationLayer));
	std::memcpy(&output.at(5), &localData.animLayers.at(5), sizeof(AnimationLayer));
	std::memcpy(&output.at(11), &localData.animLayers.at(11), sizeof(AnimationLayer));
	std::memcpy(&output.at(12), &localData.animLayers.at(12), sizeof(AnimationLayer));

	return output;
}

std::array<AnimationLayer, 13> localAnims::getFakeAnimLayers()
{
	std::array< AnimationLayer, 13 > output;

	std::memcpy(output.data(), g::pLocalPlayer->AnimOverlays(), sizeof(AnimationLayer) * 13);
	std::memcpy(&output.at(4), &localData.fakeLayers.at(4), sizeof(AnimationLayer));
	std::memcpy(&output.at(5), &localData.fakeLayers.at(5), sizeof(AnimationLayer));
	std::memcpy(&output.at(11), &localData.fakeLayers.at(11), sizeof(AnimationLayer));
	std::memcpy(&output.at(12), &localData.fakeLayers.at(12), sizeof(AnimationLayer));

	return output;
}