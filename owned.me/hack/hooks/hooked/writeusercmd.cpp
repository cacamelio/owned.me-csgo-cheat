#include "../../features/features.hpp"

struct revolver_data_t
{
	std::array<int, 150> tickbase;
	std::array<bool, 150> in_attacks;
	std::array<bool, 150> can_shoot;

	void get(BaseEntity* player, UserCmd* cmd);
};

void revolver_data_t::get(BaseEntity* player, UserCmd* cmd)
{
	int cmd_number = cmd->command_number % 150;
	tickbase[cmd_number] = player->GetTickBase();
	in_attacks[cmd_number] = cmd->buttons & 0x801;
	can_shoot[cmd_number] = aimbot::can_shoot();
}

revolver_data_t revolver_data = {};

void update_revolver_time(int cmd_number)
{
	if (!config.auto_revolver)
		return;

	if (g::pLocalWeapon->ItemDefinitionIndex() != WEAPON_REVOLVER)
		return;

	float post_pone_time = FLT_MAX;

	int tickrate = (1.0 / interfaces::globals->interval_per_tick);
	if (tickrate >> 1 > 1)
	{
		int cmd_num = cmd_number - 1;

		int new_cmd = 0;
		for (int i = 1; i < tickrate >> 1; ++i)
		{
			new_cmd = cmd_num;
			if (!revolver_data.in_attacks[cmd_num % 150] || !revolver_data.can_shoot[cmd_num % 150])
				break;
			--cmd_num;
		}

		if (new_cmd)
		{
			float revolver_cock_time = 1.f - (-0.03348f / interfaces::globals->interval_per_tick);
			if (cmd_number - new_cmd >= revolver_cock_time)
			{
				int new_tick = (new_cmd + static_cast<int>(revolver_cock_time)) % 150;
				post_pone_time = utilities::TICKS_TO_TIME(revolver_data.tickbase[new_tick]) + 0.2f;
			}
		}
	}

	if (std::isfinite(post_pone_time))
		g::pLocalWeapon->PostpineFireReadyTime() = post_pone_time;
}

using RunCommand_t = void(__thiscall*)(void*, BaseEntity*, UserCmd*, PlayerMoveHelper*);

void __fastcall hooks::player::hkRunCommand(void* ecx, void* edx, BaseEntity* player, UserCmd* m_pcmd, PlayerMoveHelper* move_helper)
{
	g::pLocalPlayer = (BaseEntity*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());

	if (!g::pLocalPlayer || player != g::pLocalPlayer || !m_pcmd || !g::pLocalPlayer->IsAlive() || !player->ActiveWeapon())
		return o_run_command(ecx, player, m_pcmd, move_helper);

	if (m_pcmd->tick_count >= interfaces::globals->tick_count + 8)
	{
		// don't predict netvars on recharge
		m_pcmd->predicted = true;
		player->SetAbsOrigin(player->Origin());

		// still process local tickbase
		if (interfaces::globals->frame_time > 0.0f && !interfaces::prediction->m_engine_paused)
			++player->GetTickBase();

		return;
	}

	// get unpredicted data for autorevolver
	revolver_data.get(player, m_pcmd);

	// update post pone fire time
	update_revolver_time(m_pcmd->command_number);

	return o_run_command(ecx, player, m_pcmd, move_helper);

	// store predicted netvars
	net_compression::store();
}

void __fastcall hooks::player::hkPhysicSim(BaseEntity* ecx, void* edx) {
	auto player = reinterpret_cast<BaseEntity*>(ecx);
	auto& simulation_tick = *reinterpret_cast<int*>((uintptr_t)player + 0x2AC);
	auto cctx = reinterpret_cast<c_cmd_ctx*>((uintptr_t)player + 0x34FC);

	if (!interfaces::engine->is_in_game() || !g::pLocalPlayer || !g::pLocalPlayer->IsAlive()
		|| interfaces::globals->tick_count == simulation_tick
		|| player != g::pLocalPlayer
		|| interfaces::engine->is_playing_demo() || interfaces::engine->is_hltv()
		|| player->Flags() & 0x40)
	{
		o_physic(ecx);
		return;
	}

	tick_base.fix(cctx->cmd.command_number, player->GetTickBase());

	o_physic(ecx);
}

void WriteUsercmd(bf_write* buf, UserCmd* incmd, UserCmd* outcmd)
{
	using WriteUserCmd_t = void(__fastcall*)(void*, UserCmd*, UserCmd*);
	static auto Fn = pattern::Scan("client.dll", "55 8B EC 83 E4 F8 51 53 56 8B D9");

	__asm
	{
		mov     ecx, buf
		mov     edx, incmd
		push    outcmd
		call    Fn
		add     esp, 4
	}
}

bool __fastcall hooks::player::hkWriteUsercmdDeltaToBuffer(void* lpEcx, void* lpEdx, int nSlot, bf_write* lpBuffer, int nFrom, int nTo, bool bIsNew)
{
	if (!g::pLocalPlayer || !g::pCmd || !g::pLocalPlayer->IsAlive() || exploits::shift_amount <= 0)
		return o_write_user(lpEcx, nSlot, lpBuffer, nFrom, nTo, bIsNew);

	if (nFrom != -1)
		return true;

	int _from = -1;

	uintptr_t frame_ptr{};
	__asm mov frame_ptr, ebp;

	int* backup_commands = (int*)(frame_ptr + 0xFD8);
	int* new_commands = (int*)(frame_ptr + 0xFDC);
	int32_t newcmds = *new_commands;

	const auto shift_amt = exploits::shift_amount;

	exploits::shift_amount = 0;
	*backup_commands = 0;

	int choked_modifier = newcmds + shift_amt;

	if (choked_modifier > 62)
		choked_modifier = 62;

	*new_commands = choked_modifier;

	const int next_cmdnr = interfaces::clientstate->choked_commands + interfaces::clientstate->m_last_outgoing_command + 1;
	int _to = next_cmdnr - newcmds + 1;
	if (_to <= next_cmdnr)
	{
		while (o_write_user(lpEcx, nSlot, lpBuffer, _from, _to, true))
		{
			_from = _to++;
			if (_to > next_cmdnr)
				goto LABEL_11;
		}
		return false;
	}
LABEL_11:

	auto* ucmd = interfaces::input->get_pUser_cmd(_from);
	if (!ucmd)
		return true;

	UserCmd to_cmd{};
	UserCmd from_cmd{};

	from_cmd = *ucmd;
	to_cmd = from_cmd;

	++to_cmd.command_number;
	to_cmd.tick_count += g::iTickRate + g::iTickRate * 2;

	for (int i = 0; i < shift_amt ; i++ )
	{
		WriteUsercmd(lpBuffer, &to_cmd, &from_cmd);

		from_cmd = to_cmd;
		++to_cmd.command_number;
		++to_cmd.tick_count;
	}

	exploits::simulation_ticks = choked_modifier - newcmds + 1;

	return true;
}