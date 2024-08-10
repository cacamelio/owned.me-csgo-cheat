#include "../features.hpp"

void C_LagCompensation::Instance(int Stage)
{
	if (Stage != FRAME_NET_UPDATE_END)
		return;

	for (int32_t iPlayerID = 1; iPlayerID <= interfaces::globals->max_clients; iPlayerID++)
	{
		BaseEntity* pPlayer = reinterpret_cast<BaseEntity*>(interfaces::entity_list->get_client_entity(iPlayerID));
		if (!pPlayer || !pPlayer->IsPlayer() || !pPlayer->IsAlive() || pPlayer->TeamNum() == g::pLocalPlayer->TeamNum())
		{
			g_AnimationSync->MarkAsDormant(iPlayerID);

			g::m_CachedPlayerRecords[iPlayerID].clear();
			continue;
		}

		if (pPlayer->Dormant())
		{
			g_AnimationSync->MarkAsDormant(iPlayerID);
			continue;
		}

		if (g_AnimationSync->HasLeftOutOfDormancy(iPlayerID))
			g::m_CachedPlayerRecords[iPlayerID].clear();

		if (pPlayer->OldSimulationTime() >= pPlayer->SimulationTime())
		{
			if (pPlayer->OldSimulationTime() > pPlayer->SimulationTime())
				g::m_CachedPlayerRecords[iPlayerID].clear();

			continue;
		}

		bool bHasPreviousRecord = false;
		if (g::m_CachedPlayerRecords[iPlayerID].empty())
			bHasPreviousRecord = false;
		else if (utilities::TIME_TO_TICKS(fabs(pPlayer->SimulationTime() - g::m_CachedPlayerRecords[iPlayerID].front().m_SimulationTime)) <= 17)
			bHasPreviousRecord = true;

		C_LagRecord LagRecord;
		this->FillRecord(pPlayer, LagRecord);

		this->UnsetBreakingLagCompensation(iPlayerID);
		if (bHasPreviousRecord)
		{
			if (LagRecord.m_AnimationLayers.at(ROTATE_SERVER).at(11).flCycle == this->GetPreviousRecord(iPlayerID).m_AnimationLayers.at(ROTATE_SERVER).at(11).flCycle)
				continue;

			g_AnimationSync->SetPreviousRecord(iPlayerID, this->GetPreviousRecord(iPlayerID));
			if ((LagRecord.m_Origin - this->GetPreviousRecord(iPlayerID).m_Origin).length_2d_sqr() > 4096.0f)
			{
				this->SetBreakingLagCompensation(iPlayerID);
				this->ClearRecords(iPlayerID);
			}
		}

		g::m_CachedPlayerRecords[iPlayerID].push_front(LagRecord);
		if (g::m_CachedPlayerRecords[iPlayerID].size() > 32)
			g::m_CachedPlayerRecords[iPlayerID].pop_back();
	}
}

void C_LagCompensation::FillRecord(BaseEntity* pPlayer, C_LagRecord& LagRecord)
{
	std::memcpy(LagRecord.m_AnimationLayers.at(ROTATE_SERVER).data(), pPlayer->AnimOverlays(), sizeof(AnimationLayer) * 13);

	LagRecord.m_pEntity = pPlayer;
	LagRecord.m_nEntIndex = pPlayer->EntIndex( );
	LagRecord.m_UpdateDelay = 1;
	LagRecord.m_Flags = pPlayer->Flags();
	LagRecord.m_EyeAngles = pPlayer->EyeAngles();
	LagRecord.m_LowerBodyYaw = pPlayer->LowerBodyYaw();
	LagRecord.m_Mins = pPlayer->GetCollideable()->OBBMins();
	LagRecord.m_Maxs = pPlayer->GetCollideable()->OBBMaxs();
	LagRecord.m_SimulationTime = pPlayer->SimulationTime();
	LagRecord.m_Origin = pPlayer->Origin();
	LagRecord.m_AbsOrigin = pPlayer->AbsOrigin();
	LagRecord.m_DuckAmount = pPlayer->DuckAmount();
	LagRecord.m_Velocity = pPlayer->Velocity();

	if (pPlayer->ActiveWeapon())
		if (pPlayer->ActiveWeapon()->LastShotTime() <= pPlayer->SimulationTime())
			if (pPlayer->ActiveWeapon()->LastShotTime() > pPlayer->OldSimulationTime())
				LagRecord.m_bIsShooting = true;
}

void C_LagCompensation::ResetData()
{
	for (int32_t iPlayerID = 0; iPlayerID < 65; iPlayerID++)
	{
		if (!g::m_CachedPlayerRecords[iPlayerID].empty())
			g::m_CachedPlayerRecords[iPlayerID].clear();

		m_BreakingLagcompensation[iPlayerID] = false;
	}
}

float_t C_LagCompensation::GetLerpTime()
{
	float_t flUpdateRate = std::clamp(convars::update_rate->GetFloat(), convars::min_update_rate->GetFloat(), convars::max_update_rate->GetFloat());
	float_t flLerpRatio = std::clamp(convars::interp_ratio->GetFloat(), convars::min_interp_ratio->GetFloat(), convars::max_interp_ratio->GetFloat());
	return std::clamp(flLerpRatio / flUpdateRate, convars::interp->GetFloat(), 1.0f);
}

bool C_LagCompensation::IsValidTime(float_t flTime)
{
	INetChannel_info* m_netInfo = interfaces::engine->get_net_channel_info();
	if (!m_netInfo)
		return false;

	static auto sv_maxunlag = interfaces::console->FindVar(XOR("sv_maxunlag")); 

	int nTickbase = g::pLocalPlayer->GetTickBase() + 1;
	float flLerpTime = g_LagCompensation->GetLerpTime();
	float flDeltaTime = fmin(m_netInfo->get_average_latency(FLOW_OUTGOING) + g_LagCompensation->GetLerpTime(), 0.2f) - utilities::TICKS_TO_TIME(nTickbase - utilities::TIME_TO_TICKS(flTime));

	if (fabs(flDeltaTime) > 0.2f)
		return false;

	int nDeadTime = utilities::TICKS_TO_TIME(interfaces::globals->tick_count) - 0.2f;
	if (utilities::TIME_TO_TICKS(flTime + flLerpTime) < nDeadTime)
		return false;

	return true;
}