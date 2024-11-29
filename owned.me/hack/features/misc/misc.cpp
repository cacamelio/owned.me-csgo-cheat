#include "../features.hpp"

#define D3DX_PI    ((FLOAT)  3.141592654f)

template <class T>
static T* find_hud(const char* name)
{
	static DWORD* pThis = nullptr;

	if (!pThis)
	{
		static auto pThisSignature = pattern::Scan(XOR("client.dll"), XOR("B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 5D 08"));

		if (pThisSignature)
			pThis = *reinterpret_cast<DWORD**>(pThisSignature + 0x1);
	}

	if (!pThis)
		return 0;

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(pattern::Scan(XOR("client.dll"), XOR("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
	return (T*)find_hud_element(pThis, name);
}

//TO DO - fix bugwith clear killfeed after death
void misc::killfeed()
{
	if (!interfaces::engine->is_connected() || !interfaces::engine->is_in_game())
		return;

	static DWORD* death_notice = nullptr;

	if (g::pLocalPlayer->IsAlive())
	{
		if (!death_notice)
			death_notice = find_hud<DWORD>(("CCSGO_HudDeathNotice"));

		if (death_notice)
		{
			auto local_death_notice = (float*)((uintptr_t)death_notice + 0x50);

			if (local_death_notice)
				*local_death_notice = config.enable_killfeed ? FLT_MAX : 1.5f;

			if (g::bClearKillFeed)
			{
				g::bClearKillFeed = false;

				using Fn = void(__thiscall*)(uintptr_t);
				static auto clear_notice = (Fn)pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 EC 0C 53 56 8B 71 58"));

				clear_notice((uintptr_t)death_notice - 0x14);
			}
		}

	}
	else
		death_notice = 0;
}

void misc::RecoilCrosshair()
{
	static auto cl_crosshair_recoil = interfaces::console->FindVar("cl_crosshair_recoil");
	cl_crosshair_recoil->SetValue(config.recoilcross ? 1 : 0);
}

void misc::RankRevealer()
{
	if (config.rankreveal)
	{
		if (GetAsyncKeyState(VK_TAB))
			interfaces::client->dispatch_user_message(cs_um_serverrankrevealall, 0, 0, nullptr);
	}
}

void misc::ViewModelOffset()
{
	if (!config.custom_viewmodel)
	{
		interfaces::console->FindVar("viewmodel_offset_x")->SetValue(1);
		interfaces::console->FindVar("viewmodel_offset_y")->SetValue(1);
		interfaces::console->FindVar("viewmodel_offset_z")->SetValue(-1);
	}
	else
	{
		interfaces::console->FindVar("viewmodel_offset_x")->SetValue(config.viewmodel_offset_x);
		interfaces::console->FindVar("viewmodel_offset_y")->SetValue(config.viewmodel_offset_y);
		interfaces::console->FindVar("viewmodel_offset_z")->SetValue(config.viewmodel_offset_z);
	}
}

void misc::DisableInterpolation()
{
	for (auto entity : entity_cache::Groups[CGroupType::PLAYERS_ENEMIES])
	{
		if (!entity || !entity->IsAlive())
			continue;

		if (entity->Dormant())
			continue;

		auto varmap = reinterpret_cast<uintptr_t>(entity) + 36;
		auto varmap_size = *reinterpret_cast<int*>(varmap + 20);
		for (auto index = 0; index < varmap_size; index++)
			*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(varmap) + index * 12) = 0;
	}
}

void trace_hull(const vec3_t& start, const vec3_t& end, const unsigned int mask, const vec3_t& extens, trace_t* trace) {

	trace_world_and_props_only_t filter;
	auto ray = ray_t(start, end);
	ray.m_extents = extens;
	ray.m_is_ray = false;

	interfaces::trace_ray->trace_ray(ray, mask, &filter, trace);
}

void misc::ThirdPerson()
{
	if (!config.thirdperson)
		return;
	auto localplayer = interfaces::entity_list->get<BaseEntity>(interfaces::engine->get_local_player());
	if (!localplayer)
		return;

	static auto b_once = false;
	bool is_enabled = g_keyhandler.CheckKey(config.thirdperson_key, config.thirdperson_key_style);
	if (is_enabled)
		tp_distance = std::min(std::max(tp_distance, 30.0f) + 6.0f, config.thirdperson_distance);
	else
		tp_distance = std::max(tp_distance - 6.0f, 30.0f);

	if (tp_distance <= (is_enabled ? 50.0f : 30.0f))
	{
		interfaces::input->m_bCameraInThirdPerson = false;
		return;
	}

	vec3_t eye_pos = localplayer->AbsOrigin() + (g::bFakeDuck ? vec3_t(0, 0, interfaces::game_movement->get_player_view_offset(false).z) : localplayer->ViewOffset());
	//TODO: fix crash AbsOrigin();

	vec3_t view_angles;
	interfaces::engine->get_view_angles(view_angles);

	interfaces::input->m_bCameraInThirdPerson = true;
	interfaces::input->m_vecCameraOffset = vec3_t(view_angles.x, view_angles.y, tp_distance);

	trace_t trace;

	float extent = 12.0f + ((90 + config.view_fov) / 4.8f - 18.f);

	vec3_t forward = vec3_t(0, 0, 0);

	math::angle_vectors(vec3_t(view_angles.x, view_angles.y, 0.f), forward);

	trace_hull(eye_pos, eye_pos - forward * interfaces::input->m_vecCameraOffset.z, MASK_NPCWORLDSTATIC, vec3_t(extent, extent, extent), &trace);

	interfaces::input->m_vecCameraOffset.z *= trace.flFraction;

	if (interfaces::input->m_vecCameraOffset.z < std::min(30.f, config.thirdperson_distance))
		interfaces::input->m_bCameraInThirdPerson = false;

	if (localplayer->IsAlive())
	{
		b_once = false;
		return;
	}

	if (b_once)
		localplayer->ObserverMode() = 5;

	if (localplayer->ObserverMode() == 4)
		b_once = true;
}

void misc::FakeDuck() {

	if (!(g::pLocalPlayer->Flags() & FL_ONGROUND))
	{
		g::bFakeDuck = false;
		return;
	}

	if (!config.fakeduck)
	{
		g::bFakeDuck = false;
		return;
	}

	if (!g_keyhandler.CheckKey(config.fakeduck_key, config.fakeduck_key_style))
	{
		g::bFakeDuck = false;
		return;
	}

	if (!g::bFakeDuck && interfaces::clientstate->choked_commands != 7)
		return;

	g::pCmd->buttons |= IN_BULLRUSH;

	if (interfaces::clientstate->choked_commands >= 7)
		g::pCmd->buttons |= IN_DUCK;
	else
		g::pCmd->buttons &= ~IN_DUCK;

	*g::bSendPacket = (interfaces::clientstate->choked_commands >= 14);

	g::bFakeDuck = true;
}