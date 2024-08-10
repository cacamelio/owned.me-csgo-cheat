#include "../features.hpp"

Prediction prediction;

void Prediction::Start(UserCmd* pCmd, BaseEntity* pLocal)
{
	// @xref: "Prediction::ProcessMovement"

	if (pLocal == nullptr || !pLocal->IsAlive() || interfaces::move_helper == nullptr)
		return;

	// start command
	*pLocal->CurrentCommand() = pCmd;
	pLocal->LastCommand() = *pCmd;

	// random_seed isn't generated in ClientMode::CreateMove yet, we must generate it ourselves
	if (!iPredictionRandomSeed)
		iPredictionRandomSeed = *reinterpret_cast<int**>(pattern::Scan(XOR("client.dll"), sig_pred_random_seed) + 2);

	*iPredictionRandomSeed = pCmd->randomseed & 0x7FFFFFFF;
	// set ourselves as a predictable entity
	pSetPredictionEntity = pLocal;

	// backup globals
	flOldCurrentTime = interfaces::globals->cur_time;
	flOldFrameTime = interfaces::globals->frame_count;
	iOldTickCount = interfaces::globals->tick_count;
	vecOldVelocity = pSetPredictionEntity->Velocity();

	// backup tickbase
	const int iOldTickBase = pLocal->GetTickBase();

	// backup prediction states
	const bool bOldIsFirstPrediction = interfaces::prediction->m_first_time_predicted;
	const bool bOldInPrediction = interfaces::prediction->m_in_prediction;

	// set corrected values
	interfaces::globals->cur_time = utilities::TICKS_TO_TIME(GetTickbase(pCmd, pLocal));
	interfaces::globals->frame_time = interfaces::prediction->m_engine_paused ? 0.f : interfaces::globals->interval_per_tick;
	interfaces::globals->tick_count = GetTickbase(pCmd, pLocal);

	interfaces::prediction->m_first_time_predicted = false;
	interfaces::prediction->m_in_prediction = true;

	/* skipped weapon select and vehicle predicts */
	//if (pCmd->impulse)
	//	*pLocal->GetImpulse() = pCmd->impulse;

	// synchronize m_afButtonForced & m_afButtonDisabled
	pCmd->buttons |= pLocal->ButtonForced();
	pCmd->buttons &= ~(pLocal->ButtonDisabled());

	// update button state
	const int iButtons = pCmd->buttons;
	const int nButtonsChanged = iButtons ^ *reinterpret_cast<int*>(uintptr_t(pLocal) + 0x31E8);

	// synchronize m_afButtonLast
	*reinterpret_cast<int*>(uintptr_t(pLocal) + 0x31DC) = (uintptr_t(pLocal) + 0x31E8);

	// synchronize m_nButtons
	*reinterpret_cast<int*>(uintptr_t(pLocal) + 0x31E8) = iButtons;

	// synchronize m_afButtonPressed
	*reinterpret_cast<int*>(uintptr_t(pLocal) + 0x31E0) = iButtons & nButtonsChanged;

	// synchronize m_afButtonReleased
	*reinterpret_cast<int*>(uintptr_t(pLocal) + 0x31E4) = nButtonsChanged & ~iButtons;

	// check if the player is standing on a moving entity and adjusts velocity and basevelocity appropriately
	interfaces::prediction->CheckMovingGround(pLocal, interfaces::globals->frame_time);

	// copy angles from command to player
	interfaces::prediction->SetLocalViewAngles(pCmd->viewangles);

	// run prethink
	if (pLocal->PhysicsRunThink(0))
		pLocal->PreThink();

	// run think
	int* iNextThinkTick = &pLocal->GetNextThinkTick();
	if (*iNextThinkTick > 0 && *iNextThinkTick <= GetTickbase(pCmd, pLocal))
	{
		*iNextThinkTick = -1;
		pLocal->Think();
	}

	// set host player
	interfaces::move_helper->SetHost(pLocal);

	// setup move
	interfaces::prediction->SetupMove(pLocal, pCmd, interfaces::move_helper, &moveData);
	interfaces::game_movement->ProcessMovement(pLocal, &moveData);

	// finish move
	interfaces::prediction->FinishMove(pLocal, pCmd, &moveData);
	interfaces::move_helper->ProcessImpacts();

	// run post think
	pLocal->PostThink();

	// restore tickbase
	pLocal->GetTickBase() = iOldTickBase;

	// restore globals
	flNewCurrentTime = interfaces::globals->cur_time;
	flNewFrameTime = interfaces::globals->frame_count;
	iNewTickCount = interfaces::globals->tick_count;
	vecNewVelocity = pSetPredictionEntity->Velocity();

	// restore prediction states
	interfaces::prediction->m_in_prediction = bOldInPrediction;
	interfaces::prediction->m_first_time_predicted = bOldIsFirstPrediction;

	auto wep = g::pLocalWeapon;

	if (wep) {
		wep->UpdateAccuracyPenalty();
		flInaccuracy = wep->GetInaccuracy();
		flSpread = wep->GetSpread();
	}
	else {
		flInaccuracy = flSpread = 0.f;
	}

	g_localanims->setupShotPos();
}

void Prediction::End(UserCmd* pCmd, BaseEntity* pLocal)
{
	if (pLocal == nullptr || !pLocal->IsAlive() || interfaces::move_helper == nullptr)
		return;

	// reset host player
	interfaces::move_helper->SetHost(nullptr);

	// restore globals
	interfaces::globals->cur_time = flOldCurrentTime;
	interfaces::globals->frame_time = flOldFrameTime;
	interfaces::globals->tick_count = iOldTickCount;

	// finish command
	*pLocal->CurrentCommand() = nullptr;

	// reset prediction seed
	*iPredictionRandomSeed = -1;

	// reset prediction entity
	pSetPredictionEntity = nullptr;

	// reset move
	interfaces::game_movement->reset();
}

int Prediction::GetTickbase(UserCmd* pCmd, BaseEntity* pLocal)
{
	static int iTick = 0;
	static UserCmd* pLastCmd = nullptr;

	if (pCmd != nullptr)
	{
		// if command was not predicted - increment tickbase
		if (pLastCmd == nullptr || pLastCmd->predicted)
			iTick = g::tick_base;
		else
			iTick++;

		pLastCmd = pCmd;
	}

	return iTick;
}

// fix issues with predicted netvars (like aimpunch, etc)
namespace net_compression
{
	std::array<stored_data_t, 150> data = {};

	void reset()
	{
		data.fill(stored_data_t());
	}

	void store()
	{
		if (!g::pLocalPlayer || !g::pLocalPlayer->IsAlive())
		{
			reset();
			return;
		}

		int tick_base = g::pLocalPlayer->GetTickBase();

		stored_data_t& d = data[tick_base % 150];
		d.tick_base = tick_base;
		d.punch = g::pLocalPlayer->AimPunchAngle();
		d.punch_vel = g::pLocalPlayer->AimPunchAngleVel();
		d.view_offset = g::pLocalPlayer->ViewOffset();
		d.vel_modifier = g::pLocalPlayer->VelocityModifier();
	}

	void set(int stage)
	{
		if (stage != FRAME_NET_UPDATE_END)
			return;

		if (!g::pLocalPlayer || !g::pLocalPlayer->IsAlive())
		{
			reset();
			return;
		}

		int tick_base = g::pLocalPlayer->GetTickBase();

		stored_data_t& d = data[tick_base % 150];
		if (d.tick_base != g::pLocalPlayer->GetTickBase())
			return;

		auto punch_delta = g::pLocalPlayer->AimPunchAngle() - d.punch;
		auto punch_vel_delta = g::pLocalPlayer->AimPunchAngleVel() - d.punch_vel;
		auto view_delta = g::pLocalPlayer->ViewOffset() - d.view_offset;

		if (std::abs(punch_delta.x) < 0.03125f &&
			std::abs(punch_delta.y) < 0.03125f &&
			std::abs(punch_delta.z) < 0.03125f)
			g::pLocalPlayer->AimPunchAngle() = d.punch;

		if (std::abs(punch_vel_delta.x) < 0.03125f &&
			std::abs(punch_vel_delta.y) < 0.03125f &&
			std::abs(punch_vel_delta.z) < 0.03125f)
			g::pLocalPlayer->AimPunchAngleVel() = d.punch_vel;

		if (std::abs(view_delta.x) < 0.03125f &&
			std::abs(view_delta.y) < 0.03125f &&
			std::abs(view_delta.z) < 0.03125f)
			g::pLocalPlayer->ViewOffset() = d.view_offset;

		if (std::abs(g::pLocalPlayer->VelocityModifier() - d.vel_modifier) <= 0.00625f)
			g::pLocalPlayer->VelocityModifier() = d.vel_modifier;
	}
}