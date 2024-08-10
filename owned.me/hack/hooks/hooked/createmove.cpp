#include "../../features/features.hpp"

int last_choked_packets = -1;

bool __fastcall hooks::hkCreateMove(void* thisptr, int edx, float flInputSampleTime, UserCmd* pCmd)
{
	static auto oCreateMove = detour::create_move.GetOriginal<decltype(&hkCreateMove)>();
	oCreateMove(thisptr, edx, flInputSampleTime, pCmd);

	bool ret = detour::create_move.GetOriginal<decltype(&hkCreateMove)>();

	if (!pCmd || !pCmd->command_number)
		return ret;

	if (ret)
		interfaces::engine->set_view_angles(pCmd->viewangles);

	g::pLocalPlayer = interfaces::entity_list->get<BaseEntity>(interfaces::engine->get_local_player());

	if (g::pLocalPlayer)
	{
		g::pLocalWeapon = g::pLocalPlayer->ActiveWeapon();
		if (g::pLocalWeapon)
			g::pLocalWeaponData = interfaces::weapon_system->get_weapon_data(g::pLocalWeapon->ItemDefinitionIndex());
	}

	g::iTickRate = 1.f / interfaces::globals->interval_per_tick;
	g::vStrafeAngles = pCmd->viewangles;
	client::MouseDelta();
	g::pCmd = pCmd;
	g::iButtons = pCmd->buttons;
	g::iFlags = g::pLocalPlayer->Flags();
	g::bAlive = g::pLocalPlayer && g::pLocalPlayer->IsAlive();

	if (exploits::need_to_recharge && last_choked_packets == 0)
	{
		pCmd->tick_count = INT_MAX;

		pCmd->forwardmove = 0.0f;
		pCmd->sidemove = 0.0f;
		pCmd->upmove = 0.0f;

		pCmd->buttons &= ~(IN_ATTACK | IN_ATTACK2);

		if (++exploits::charge_ticks >= 17)
		{
			exploits::need_to_recharge = false;
			exploits::recharge_completed = true;
			tick_base.store(g::pLocalPlayer->GetTickBase(), g::pCmd->command_number, exploits::charge_ticks, false);
			*g::bSendPacket = true;
		}
		else
			*g::bSendPacket = false;

		exploits::shift_amount = 0;
		last_choked_packets = 0;
		return false;
	}
	else
		last_choked_packets = interfaces::clientstate->choked_commands;

	if (interfaces::engine->get_net_channel_info())
		g::fLatency = interfaces::engine->get_net_channel_info()->get_latency(FLOW_OUTGOING);
	else
		g::fLatency = 0.f;

	if (g_menu.IsMenuOpened())
		pCmd->buttons &= ~IN_ATTACK;

	exploits::prepare();

	if (exploit[hs].works && g::pLocalWeapon->IsGun())
		g::tick_base = g::pLocalPlayer->GetTickBase() - 9;
	else
		g::tick_base = g::pLocalPlayer->GetTickBase();

	/*run before pred*/
	movement::run();
	misc::ViewModelOffset();
	misc::RankRevealer();
	misc::RecoilCrosshair();
	misc::FakeDuck();
	g_aimbot->predict_stop();
	g_aimbot->quick_stop();

	if (g::pLocalPlayer && g::pLocalPlayer->IsAlive() && config.extended_ang_enable)
	{
		if (!(pCmd->buttons & IN_ATTACK))
		{
			g::bExtendingAngles = true;
			g::m_angExtendedAngles = pCmd->viewangles;
			g::m_angExtendedAngles.z = config.extended_ang;
		}
	}
	else if (!config.extended_ang_enable)
	{
		g::m_angExtendedAngles = pCmd->viewangles;
		g::m_angExtendedAngles.z = 0;
	}

	fakelag::lag = interfaces::clientstate->choked_commands;

	prediction.Start(pCmd, g::pLocalPlayer);
	{
		legitbot::work(pCmd);
		fakelag::createMove(pCmd);
		antiaim::desyncAngle = 0.f;
		antiaim::createMove(pCmd);
		g_aimbot->calc_time();

		g_aimbot->run();
		grenades::NadePred();
		if (*g::bSendPacket)
		{
			//get current origin 
			vec3_t curr = g::pLocalPlayer->Origin();
			//get pervoius origin
			vec3_t perv = fakelag::m_net_pos.empty() ? curr : fakelag::m_net_pos.front().m_pos;
			//check if we broke lagcomp
			fakelag::break_lc = (curr - perv).length_sqr() > 4096.f;
			//save sent origin and time
			fakelag::m_net_pos.emplace_front(interfaces::globals->cur_time, curr);
		}

		exploits::run();
	}
	prediction.End(pCmd, g::pLocalPlayer);

	math::normalize_angles(pCmd->viewangles);
	movement::FixMove(pCmd, g::vStrafeAngles);

	if (*g::bSendPacket) {
		g::vRealAng = pCmd->viewangles;
	}
	else {
		g::vFakeAng = pCmd->viewangles;
	}

	g_localanims->work();

	return false;
}

void __stdcall hooks::player::call(int sequence_number, float sample_frametime, bool active, bool& send_packet)
{
	g::bSendPacket = &send_packet;
	oCreateMove(sequence_number, sample_frametime, active);
}

__declspec(naked) void __fastcall hooks::player::hook(void* ecx, void* edx, int sequence_number, float sample_frametime, bool active) {
	__asm {
		push ebp
		mov  ebp, esp
		push ebx
		push esp
		push dword ptr[active]
		push dword ptr[sample_frametime]
		push dword ptr[sequence_number]
		call hooks::player::call
		pop  ebx
		pop  ebp
		retn 0Ch
	}
}