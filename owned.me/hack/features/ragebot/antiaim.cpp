#include "../features.hpp"

void antiaim::createMove(UserCmd* cmd)
{
	if (condition(cmd))
		return;

	auto side_move = [cmd]() -> void
	{
		static auto next_sidemove = 0;

		if (next_sidemove > g::pCmd->tick_count || next_sidemove < 0)
			return;

		auto pressed_move_key = (cmd->buttons & IN_MOVELEFT
			|| cmd->buttons & IN_MOVERIGHT
			|| cmd->buttons & IN_BACK
			|| cmd->buttons & IN_FORWARD);

		const float& sideAmount = cmd->buttons & IN_DUCK || g::bFakeDuck ? 3.f : 1.01f;

		static auto switch_sidemove = false;

		if (!pressed_move_key)
		{
			g::pCmd->sidemove = switch_sidemove ? -sideAmount : sideAmount;

			if (interfaces::clientstate->choked_commands == 1)
				next_sidemove = g::pCmd->tick_count + 2;
			else 
			{
				next_sidemove = g::pCmd->tick_count + 4;

				if (interfaces::clientstate->choked_commands < config.limit_fakelag - 1)
					next_sidemove--;
			}

			switch_sidemove = !switch_sidemove;
		}
	};
	
	if ((config.desync_type) && !(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_ATTACK2)) 
	{
		side_move();
	}

	setSides();

	static bool jitter_ = false;
	static float current_yaw = 0.f;
	const bool desync_side = g_keyhandler.CheckKey(config.desync_key, config.desync_key_style) && config.desync_key;
	static bool flip_ = false;
	float current_desync = flip_ ? config.desync_range : -config.inverted_desync_range;

	cmd->viewangles.x = getPitch(cmd);

	if (config.base_angle == 1)
		cmd->viewangles.y = (atTargets() + getYaw(cmd)) - 180.f;
	else
		cmd->viewangles.y += getYaw(cmd);

	if(current_yaw != cmd->viewangles.y)
		current_yaw = cmd->viewangles.y;
	
	if (!*g::bSendPacket)
	{
		switch (config.desync_type) {
		case 1:
			cmd->viewangles.y += current_desync;
			break;
		case 2:
			cmd->viewangles.y += jitter_ ? -current_desync : current_desync;
			break;
		}
	}
	if(flip_ != desync_side)
		flip_ = desync_side;
	
	jitter_ = !jitter_;
}

float antiaim::getPitch(UserCmd* cmd)
{
	auto pitch = 0.f;

	switch (config.pitch_type)
	{
	case 1:
		pitch = 89.f;
		break;
	case 2:
		pitch = -89.f;
		break;
	}

	return pitch;
}

float antiaim::getYaw(UserCmd* cmd)
{
	static auto invert_jitter = false;
	auto yaw_additive = 0.f;
	auto yaw = 0.f;
	float max = 0.f;
	float min = 0.f;
	float speed = 0.f;
	switch (config.yaw_type) 
	{
	case 0:
		yaw += 180.f;
		break;
	case 1:
		yaw += 180.f + (invert_jitter ? config.jitter_range : -config.jitter_range);
		break;
	}

	switch (manualSide)
	{
		case SIDE_LEFT:
			yaw -= 90.0f;
		break;
		
		case SIDE_RIGHT:
			yaw += 90.0f;
		break;
	}

	invert_jitter = !invert_jitter;
	return yaw;
}

bool antiaim::condition(UserCmd* cmd, bool check)
{
	if (!cmd)
		return true;

	if (!g::pLocalPlayer || !interfaces::engine->is_in_game())
		return true;

	if (!config.enable_antiaim)
		return true;

	if (!g::pLocalPlayer->IsAlive())
		return true;

	if (g::pLocalPlayer->HasGunGameImmunity() || g::pLocalPlayer->Flags() & FL_FROZEN)
		return true;

	if (g::pLocalPlayer->MoveType() == MOVETYPE_NOCLIP || g::pLocalPlayer->MoveType() == MOVETYPE_LADDER)
		return true;

	if (!g::pLocalWeapon)
		return true;

	if (cmd->buttons & IN_ATTACK && g::pLocalWeapon->ItemDefinitionIndex() != WEAPON_REVOLVER && g::pLocalWeapon->IsGun())
		return true;

	auto revolver_shoot = g::pLocalWeapon->ItemDefinitionIndex() == WEAPON_REVOLVER && !g_aimbot->revolver_fire && (g::pCmd->buttons & IN_ATTACK || g::pCmd->buttons & IN_ATTACK2);

	if (revolver_shoot)
		return true;

	if ((cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2) && g::pLocalWeapon->IsKnife())
		return true;

	if (check && freezeCheck)
		return true;

	if (check && cmd->buttons & IN_USE)
		return true;

	if (g::pLocalWeapon->IsGrenade() && g::pLocalWeapon->ThrowTime() >= 0)
		return true;

	return false;
}

float antiaim::atTargets()
{
	BaseEntity* target = nullptr;
	auto best_fov = FLT_MAX;

	for (auto i = 1; i < interfaces::globals->max_clients; i++)
	{
		auto e = reinterpret_cast<BaseEntity*>(interfaces::entity_list->get_client_entity(i));

		if (!e || e == g::pLocalPlayer || !e->IsPlayer() || !e->IsAlive() || e->Dormant() || e->TeamNum() == g::pLocalPlayer->TeamNum())
			continue;

		auto weapon = e->ActiveWeapon();

		if (!weapon)
			continue;

		if (!weapon->IsGun())
			continue;

		vec3_t angles;
		interfaces::engine->get_view_angles(angles);

		auto fov = math::get_fov(angles, math::calculate_angle(g::pLocalPlayer->EyePos(), e->AbsOrigin()));

		if (fov < best_fov)
		{
			best_fov = fov;
			target = e;
		}
	}

	auto angle = 180.0f;

	if (!target)
		return g::pCmd->viewangles.y + angle;

	return math::calculate_angle(g::pLocalPlayer->EyePos(), target->AbsOrigin()).y + angle;
}

void antiaim::setSides()
{
	static bool clicked = false;

	bool bBackClicked = g_keyhandler.CheckKey(config.backside, 1);
	if (bBackClicked && !clicked)
	{
		clicked = true;
		manualSide = manualSide == SIDE_BACK ? SIDE_NONE: SIDE_BACK;
	}

	bool bLeftClicked = g_keyhandler.CheckKey(config.leftside, 1);
	if (bLeftClicked && !clicked)
	{
		clicked = true;
		manualSide = manualSide == SIDE_LEFT ? SIDE_NONE : SIDE_LEFT;
	}

	bool bRightClicked = g_keyhandler.CheckKey(config.rightside, 1);
	if (bRightClicked && !clicked)
	{
		clicked = true;
		manualSide = manualSide == SIDE_RIGHT ? SIDE_NONE : SIDE_RIGHT;
	}

	if (!bRightClicked && !bLeftClicked && !bBackClicked)
		clicked = false;
}

// TO DO - Refactoring
void fakelag::createMove(UserCmd* cmd)
{
	if (antiaim::condition(cmd, false))
		return;

	if (g::bFakeDuck)
		return;

	if (!config.enable_fakelag && config.desync_type)
	{
		*g::bSendPacket = false;

		if (lag >= 2)
			*g::bSendPacket = true;
	}

	if (config.enable_fakelag)
	{
		auto max_lag = 14;

		if (exploit[ dt ].enabled || exploit[ hs ].enabled)
			max_lag = 2;

		int limit = std::min(config.limit_fakelag, max_lag);

		int variance = std::clamp(config.fakelag_variance, 1, 100);

		// get current origin.
		vec3_t cur = g::pLocalPlayer->Origin();

		// get prevoius origin.
		vec3_t prev = m_net_pos.empty() ? g::pLocalPlayer->Origin() : m_net_pos.front().m_pos;

		// delta between the current origin and the last sent origin.
		float delta = (cur - prev).length_sqr();

		//max
		if (config.fakelag_type == 0)
			*g::bSendPacket = false;

		//break 
		else if (config.fakelag_type == 1 && delta <= 4096.f)
			*g::bSendPacket = false;

		//fluctuate
		else if (config.fakelag_type == 2)
		{
			if (g::pCmd->command_number % variance >= limit)
				limit = 2;
			*g::bSendPacket = false;
		}

		if (interfaces::engine->voice_record())
		{
			if (config.desync_type)
				limit = 2;
			else {
				*g::bSendPacket = true;
			}
		}

		if (g::pLocalWeapon->IsGrenade() && g::pLocalWeapon->ThrowTime() > 0.0f)
		{
			if (config.desync_type)
				limit = 2;
			else {
				*g::bSendPacket = true;
			}
		}

		if (exploits::enabled())
		{
			if (config.desync_type)
				limit = 2;
			else {
				*g::bSendPacket = true;
			}
		}

		if (g::pLocalWeapon->ItemDefinitionIndex() == WEAPON_C4 || g::pLocalPlayer->IsDefusing())
		{
			if (g::pCmd->buttons & IN_ATTACK) {
				*g::bSendPacket = true;
			}
		}

		if (g::bFakeDuck || exploit[dt].recharging) 
		{
			limit = 13;
			*g::bSendPacket = false;
		}

		if (lag >= limit) 
		{
			*g::bSendPacket = true;
		}

		if (lag >= max_lag) 
		{
			*g::bSendPacket = true;
		}
	}
}
