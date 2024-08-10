#include "../features.hpp"

void aimbot::run()
{
	// clear old targets.
	g_aim_data->targets.clear();
	if ( !g::pLocalWeapon )
		return;

	setup_weapons();
	if (!rage_settings.ragebot_enable || antiaim::freezeCheck)
	{
		should_quick_stop = false;
		return;
	}

	if (!g::pLocalPlayer->IsAlive() )
	{
		should_quick_stop = false;
		return;
	}

	if (!can_shoot())
	{
		if (!rage_settings.autostop_type[1]) {
			should_quick_stop = false;
		}
		return;
	}

	select_targets();

	scan_targets();

	// fixes the spread.
	// needed for rapid-fire weapons.
	static auto m_WeaponRecoilScale = interfaces::console->FindVar(XOR("weapon_recoil_scale"));
	if (m_WeaponRecoilScale->GetFloat() > 0.0f)
		g::pCmd->viewangles -= m_WeaponRecoilScale->GetFloat() * g::pLocalPlayer->AimPunchAngle();
}

C_LagRecord* aimbot::find_newest_record(/*std::deque<C_LagRecord>& log*/ BaseEntity* target_entity)
{
	auto& log = g_LagCompensation->GetPlayerRecords(target_entity->EntIndex());
	for (auto it = log.begin(); it != log.end(); it = next(it)) {
		if (g_LagCompensation->IsValidTime(it->m_SimulationTime)) {
			return &*it;
		}
	}
	return nullptr;
}

C_LagRecord* aimbot::find_oldest_record(/*std::deque<C_LagRecord>& log*/ BaseEntity* target_entity)
{
	C_LagRecord* Result = nullptr;

	auto& log = g_LagCompensation->GetPlayerRecords(target_entity->EntIndex());
	for (auto it = log.begin(); it != log.end(); it = next(it)) {
		if (g_LagCompensation->IsValidTime(it->m_SimulationTime)) {
			Result = &*it;
		}
	}
	return Result;
}

void aim_data_t::setup_hitboxes(C_LagRecord* record) {
	if (!record)
		return;

	// reset hitboxes.
	enabled_hitboxes.clear();

	auto player = interfaces::entity_list->get<BaseEntity>(record->m_nEntIndex);
	if (!player || !player->IsAlive())
		return;

	auto player_weapon = player->ActiveWeapon();
	if ( !player_weapon )
		return;

	CWeaponData* player_weapon_data = nullptr;
	if (player_weapon != nullptr)
		player_weapon_data = player_weapon->GetWpnData();
	const auto max_speed = player_weapon_data != nullptr ? (player->IsScoped() ? player_weapon_data->weapon_max_speed_alt : player_weapon_data->weapon_max_speed ) : 260.f;
	const auto is_zeus = g::pLocalWeapon->ItemDefinitionIndex() == WEAPON_TASER;
	const auto is_knife = !is_zeus && g::pLocalWeaponData->weapon_type == WEAPONTYPE_KNIFE;
	const auto is_baim_key = g_aimbot->rage_settings.baim_key && g_keyhandler.CheckKey(g_aimbot->rage_settings.baim_key, g_aimbot->rage_settings.baim_key_style);
	bool only_special_items = false;
	bool only = false;

	if (is_zeus) {
		enabled_hitboxes.push_back({ hitbox_stomach, prefer });
		only_special_items = true;
	}
	else if (is_knife) {
		enabled_hitboxes.push_back({ hitbox_upper_chest, prefer });
		only_special_items = true;
	}

	if (only_special_items)
		return;

	// only body, in air.
	if (is_baim_key) {
		enabled_hitboxes.push_back({ hitbox_chest, prefer });
		enabled_hitboxes.push_back({ hitbox_stomach, prefer });
		only = true;
	}

	if (only)
		return;

	// prefer body, lethal.
	if (g_aimbot->rage_settings.body_prefers[0])
		enabled_hitboxes.push_back({ hitbox_stomach, lethal });

	// prefer body, lethal x2.
	if (g_aimbot->rage_settings.body_prefers[1])
		enabled_hitboxes.push_back({ hitbox_stomach, lethal2 });

	// prefer body, standing.
	if (g_aimbot->rage_settings.body_prefers[2] && !(record->m_Velocity.length_2d() > 3.25f))
		enabled_hitboxes.push_back({ hitbox_stomach, prefer });

	// prefer body, moving.
	if (g_aimbot->rage_settings.body_prefers[3] && record->m_Velocity.length_2d() > max_speed)
		enabled_hitboxes.push_back({ hitbox_stomach, prefer });

	// prefer body, in air.
	if (g_aimbot->rage_settings.body_prefers[4] && !(record->m_Flags & FL_ONGROUND))
		enabled_hitboxes.push_back({ hitbox_stomach, prefer });

	// prefer head, onshot.
	if (g_aimbot->rage_settings.body_prefers[0] && record->m_bIsShooting)
		enabled_hitboxes.push_back({ hitbox_head, prefer });

	// prefer head, standing.
	if (g_aimbot->rage_settings.body_prefers[2] && !(record->m_Velocity.length_2d() > 3.25f))
		enabled_hitboxes.push_back({ hitbox_head, prefer });

	// prefer head, moving.
	if (g_aimbot->rage_settings.body_prefers[3] && record->m_Velocity.length_2d() > max_speed)
		enabled_hitboxes.push_back({ hitbox_head, prefer });

	// prefer head, in air.
	if (g_aimbot->rage_settings.body_prefers[4] && !(record->m_Flags & FL_ONGROUND))
		enabled_hitboxes.push_back({ hitbox_head, prefer });

	if (g_aimbot->rage_settings.hitboxes[0])
		enabled_hitboxes.push_back({ hitbox_head, g_aimbot->rage_settings.hitbox_prefer == 1 ? prefer : normal });

	if (g_aimbot->rage_settings.hitboxes[1])
		enabled_hitboxes.push_back({ hitbox_neck, normal });

	if (g_aimbot->rage_settings.hitboxes[2])
		enabled_hitboxes.push_back({ hitbox_upper_chest, normal });

	if (g_aimbot->rage_settings.hitboxes[3]) 
		enabled_hitboxes.push_back({ hitbox_chest, normal });

	if (g_aimbot->rage_settings.hitboxes[4])
		enabled_hitboxes.push_back({ hitbox_lower_chest, normal });

	if (g_aimbot->rage_settings.hitboxes[5]) 
		enabled_hitboxes.push_back({ hitbox_pelvis, normal });

	if (g_aimbot->rage_settings.hitboxes[6])
		enabled_hitboxes.push_back({ hitbox_stomach, g_aimbot->rage_settings.hitbox_prefer == 2 ? prefer : normal });
	
	if (g_aimbot->rage_settings.hitboxes[7]) {
		enabled_hitboxes.push_back({ hitbox_left_hand, normal });
		enabled_hitboxes.push_back({ hitbox_right_hand, normal });
	}

	if (g_aimbot->rage_settings.hitboxes[8]) {
		enabled_hitboxes.push_back({ hitbox_left_calf, normal });
		enabled_hitboxes.push_back({ hitbox_right_calf, normal });
	}

	if (g_aimbot->rage_settings.hitboxes[9]) {
		enabled_hitboxes.push_back({ hitbox_left_foot, normal });
		enabled_hitboxes.push_back({ hitbox_right_foot, normal });
	}
}


void aimbot::select_targets()
{
	for (auto i = 1; i < interfaces::globals->max_clients; i++)
	{
		auto player = reinterpret_cast<BaseEntity*>(interfaces::entity_list->get_client_entity(i));
		if (!player || !player->IsAlive() || player->Dormant() || !player->Enemy(g::pLocalPlayer) || player == g::pLocalPlayer || player->HasGunGameImmunity())
			continue;

		g_aim_data->targets.emplace_back(player, i);
	}
}

void aimbot::scan_targets()
{
	vec3_t best_point(vec3_t(0.f, 0.f, 0.f));
	vec3_t shot_rec_point(vec3_t(0.f, 0.f, 0.f));
	vec3_t new_rec_point(vec3_t(0.f, 0.f, 0.f));
	vec3_t old_rec_point(vec3_t(0.f, 0.f, 0.f));
	auto best_damage = 0.f;
	auto shot_rec_damage = 0.f;
	auto new_rec_damage = 0.f;
	auto old_rec_damage = 0.f;
	int best_hitbox = -1;
	int shot_rec_hitbox = -1;
	int new_rec_hitbox = -1;
	int old_rec_hitbox = -1;
	BaseEntity* best_player = nullptr;
	C_LagRecord* best_record = nullptr;
	C_LagRecord* shot_record = nullptr;

	auto record_is_ready_to_scan = [](C_LagRecord* tested_record, BaseEntity* tested_entity) -> bool {	
		if (tested_record == nullptr || tested_entity == nullptr || !g_LagCompensation->IsValidTime( tested_record->m_SimulationTime ) )
			return false;

		return true;
	};

	if (g_aim_data->targets.size() == NULL) {
		should_quick_stop = false;
		return;
	}

	// iterate all targets.
	for (auto& target : g_aim_data->targets)
	{
		auto& log = g_LagCompensation->GetPlayerRecords(target.player->EntIndex());
		if (log.empty())
			continue;

		auto old_record = find_oldest_record(target.player);
		auto new_record = find_newest_record(target.player);
		std::deque<C_LagRecord> shot_records;

		for (auto& record : log) {
			if (!record_is_ready_to_scan(&record, target.player))
				continue;

			if (record.m_bIsShooting)
				shot_records.push_back(record);
		}

		for (auto& record : shot_records)
		{
			g_aim_data->setup_hitboxes(&record);
			if (g_aim_data->enabled_hitboxes.empty())
				continue;

			float damage = 0.f;
			int hitbox = -1;
			const vec3_t point = g_aim_data->get_best_damage(record, damage, hitbox);

			//if (!g_aim_data->select_main_target(&record, damage))
			//	continue;

			if (damage > best_damage)
			{
				shot_rec_point = point;
				shot_rec_hitbox = hitbox;
				shot_record = &record;
				shot_rec_damage = damage;
				best_player = target.player;
			}
		}

		if (record_is_ready_to_scan(old_record, target.player) && !g_LagCompensation->IsBreakingLagCompensation(target.index) && old_record != new_record) {			
			
			g_aim_data->setup_hitboxes(old_record);
			if (g_aim_data->enabled_hitboxes.empty())
				continue;

			float damage = 0.f;
			int hitbox = -1;
			const vec3_t point = g_aim_data->get_best_damage(*old_record, damage, hitbox);

			//if (!g_aim_data->select_main_target(old_record, damage))
			//	continue;

			old_rec_point = point;
			old_rec_hitbox = hitbox;
			old_rec_damage = damage;

			if (damage > best_damage)
			{
  				best_point = point;
				best_damage = damage;
				best_record = old_record;
				best_player = target.player;
			}
		}

		if (record_is_ready_to_scan(new_record, target.player))
		{
			g_aim_data->setup_hitboxes(new_record);
			if (g_aim_data->enabled_hitboxes.empty())
				continue;

			float damage = 0.f;
			int hitbox = -1;
			const vec3_t point = g_aim_data->get_best_damage(*new_record, damage, hitbox);

			//if (!g_aim_data->select_main_target(new_record, damage))
			//	continue;

			new_rec_point = point;
			new_rec_hitbox = hitbox;
			new_rec_damage = damage;

			if (damage > best_damage)
			{
				best_point = point;
				best_damage = damage;
				best_record = new_record;
				best_player = target.player;
			}
		}

		if (best_player == target.player)
		{
			if (shot_rec_point != vec3_t(0.f, 0.f, 0.f) 
				&& shot_rec_damage > new_rec_damage 
				&& shot_rec_damage > old_rec_damage)
			{
				best_damage = shot_rec_damage;
				best_hitbox = shot_rec_hitbox;
				best_point = shot_rec_point;
				best_record = shot_record;
			}
			else
			{
				if (old_rec_point != vec3_t(0.f, 0.f, 0.f) 
					&& old_rec_damage > new_rec_damage)
				{
					best_damage = old_rec_damage;
					best_hitbox = old_rec_hitbox;
					best_point = old_rec_point;
					best_record = old_record;
				}
				else
				{
					best_damage = new_rec_damage;
					best_hitbox = new_rec_hitbox;
					best_point = new_rec_point;
					best_record = new_record;
				}
			}
		}
	}

	if (best_record == nullptr) {
		should_quick_stop = false;
		return;
	}

	const auto aim_angles = math::calculate_angle( g::pLocalPlayer->GetEyePosition(true), best_point);
	const auto is_zeus = g::pLocalWeapon->ItemDefinitionIndex() == WEAPON_TASER;
	const auto is_knife = !is_zeus && g::pLocalWeaponData->weapon_type == WEAPONTYPE_KNIFE;

	C_LagRecord backup;
	g_LagCompensation->FillRecord( best_player, backup );

	best_record->Apply(best_player);
	auto hitchance = g_hit_chance->can_hit(*best_record, g::pLocalWeapon, aim_angles, best_hitbox);
	backup.Apply(best_player);

	if (is_zeus && !hitchance)
		return;
	
	if (!hitchance && !is_knife && !is_zeus) {
		short wpnid = g::pLocalWeapon->ItemDefinitionIndex();
		if ((wpnid == WEAPON_SSG08 || wpnid == WEAPON_SCAR20 || wpnid == WEAPON_G3SG1 || wpnid == WEAPON_AWP) && rage_settings.autoscope_enable) {
			if (!g::pLocalPlayer->IsScoped()) {
				g::pCmd->buttons |= IN_ATTACK2;
				return;
			}
		}
		should_quick_stop = true;
		return;
	}

	if (rage_settings.autoshoot_enable)
	{
		if (is_knife)
		{
			if (g::pLocalPlayer->GetEyePosition(false).distance_to(best_point) <= 48.f)
				g::pCmd->buttons |= IN_ATTACK2;
			else
				g::pCmd->buttons |= IN_ATTACK;
		}
		else if (!is_knife && g::pCmd->buttons & IN_ATTACK2) // scope fix
		{
			g::pCmd->buttons &= ~IN_ATTACK;
			return;
		}
		else if (g::pLocalWeapon->ItemDefinitionIndex() == WEAPON_REVOLVER && !revolver_fire)
		{
			float ready_time = g::pLocalWeapon->PostpineFireReadyTime() + utilities::TICKS_TO_TIME(config.limit_fakelag - interfaces::clientstate->choked_commands);
			if (ready_time == FLT_MAX || utilities::TIME_TO_TICKS(ready_time) > 0) return;
		}
		
		if (!is_knife && !(g::pCmd->buttons & IN_ATTACK2))
		{
			if (rage_settings.autostop_type[1])
				should_quick_stop = true;

			g::pCmd->buttons |= IN_ATTACK;
		}

		g::pCmd->tick_count = utilities::TIME_TO_TICKS(best_record->m_SimulationTime + g_LagCompensation->GetLerpTime());
		g::pCmd->viewangles = aim_angles;

		if (!is_knife)
		{
			// here draw shoot matrix etc.
			if(!g::bFakeDuck)
				*g::bSendPacket = true;

			//g_LagCompensation->DrawHitboxMatrix(*best_record, Color(255, 10, 10, 255), 8);

			//player_info_t info;
			//if (interfaces::engine->get_player_info(best_player->Index(), &info))
			//{
			//	std::stringstream ss;
			//	ss << "fired shot at " << info.name << " tick: " << g::pLocalPlayer->GetTickBase() % 150 << " bt: " << utilities::TIME_TO_TICKS(fabs(best_player->SimulationTime() - best_record->m_SimulationTime));
			//	eventlog.add(ss.str().c_str(), Color(240, 240, 240, 255));
			//}
		}
	}
	aimbot_called = interfaces::globals->cur_time;
}

bool aimbot::can_shoot(const float time, bool check)
{
	auto weapon = g::pLocalWeapon;
	if (!weapon)
		return false;

	const auto is_zeus = weapon->ItemDefinitionIndex() == WEAPON_TASER;
	const auto is_knife = !is_zeus && g::pLocalWeaponData->weapon_type == WEAPONTYPE_KNIFE;

	if (!check)
	{
		if (!config.knifebot && is_knife)
			return false;

		if (!config.zeusbot && is_zeus)
			return false;
	}

	const auto weapontype = weapon->ItemDefinitionIndex();
	if (weapontype == WEAPONTYPE_C4 || weapon->IsGrenade())
		return false;

	if (weapon->Clip1Count() < 1 && !is_knife)
		return false;

	if (weapon->NextPrimaryAttack() > time)
		return false;

	if (g::pLocalPlayer->NextAttack() > time)
		return false;

	if (check && weapon->ItemDefinitionIndex() == WEAPON_REVOLVER && config.auto_revolver)
	{
		float ready_time = weapon->PostpineFireReadyTime() + utilities::TICKS_TO_TIME(config.limit_fakelag - interfaces::clientstate->choked_commands);
		if (ready_time == FLT_MAX) return false;

		float time_to_shoot = ready_time - time;
		return utilities::TIME_TO_TICKS(time_to_shoot) < 1;
	}
	return true;
}

void aimbot::calc_time()
{
	if (!config.auto_revolver)
		return;

	if (!g::pLocalWeapon)
		return;

	if (g::pLocalWeapon->ItemDefinitionIndex() != WEAPON_REVOLVER)
		return;

	float last_cock_time = 0.f;

	if (!(g::pCmd->buttons & IN_ATTACK) && g::pLocalWeapon->Clip1Count())
	{
		float curtime = utilities::TICKS_TO_TIME(g::pLocalPlayer->GetTickBase());

		g::pCmd->buttons &= ~IN_ATTACK2;
		if (can_shoot())
		{
			if (curtime < last_cock_time)
				g::pCmd->buttons |= IN_ATTACK;
			else if (curtime < g::pLocalWeapon->NextSecondaryAttack())
				g::pCmd->buttons |= IN_ATTACK2;
			else
				last_cock_time = curtime + 0.234375f;
		}
		else
		{
			last_cock_time = curtime + 0.234375f;
			g::pCmd->buttons &= ~IN_ATTACK;
		}
	}
}

void aimbot::quick_stop()
{
	if (!rage_settings.autostop_enable)
		return;

	if (!g::pLocalPlayer || !g::pLocalPlayer->IsAlive()) {
		should_quick_stop = false;
		return;
	}

	static bool should_stop_on_peek = false;
	const bool conditions = rage_settings.autostop_type[0] && g::bPeekState;
	if (should_stop_on_peek != conditions)
	{
		if (g::bPeekState)
		{
			/*std::stringstream ss;
			ss << "stop on peek worked";
			eventlog.add(ss.str().c_str(), Color(240, 240, 240, 255));*/
			should_quick_stop = true;
		}
		should_stop_on_peek = !should_stop_on_peek;
	}

	if (!should_quick_stop)
		return;

	static const auto nospread = interfaces::console->FindVar(XOR("weapon_accuracy_nospread"));

	const auto weapon = g::pLocalWeapon;

	if (!weapon)
		return;

	if (nospread->GetInt() || !(g::pLocalPlayer->Flags() & FL_ONGROUND) ||
		(weapon && weapon->ItemDefinitionIndex() == WEAPON_TASER))
		return;

	const auto wpn_info = g::pLocalWeaponData;

	if (!wpn_info)
		return;

	static auto accel = interfaces::console->FindVar(XOR("sv_accelerate"));
	float max_speed_with_weapon = 0.33000001 * (g::pLocalPlayer->IsScoped()
		&&
			(weapon->ItemDefinitionIndex() == WEAPON_SCAR20 ||
				weapon->ItemDefinitionIndex() == WEAPON_G3SG1 ||
				weapon->ItemDefinitionIndex() == WEAPON_SSG08 ||
				weapon->ItemDefinitionIndex() == WEAPON_AWP)
		? wpn_info->weapon_max_speed_alt : wpn_info->weapon_max_speed_alt);

	if (prediction.vecNewVelocity.length_2d() < max_speed_with_weapon) {

		float calculated_velocity = 
			std::sqrtf((g::pCmd->forwardmove * g::pCmd->forwardmove) +
				(g::pCmd->sidemove * g::pCmd->sidemove));

		float forward_velocity = g::pCmd->forwardmove / calculated_velocity;
		float side_velocity = g::pCmd->sidemove / calculated_velocity;

		auto Velocity = g::pLocalPlayer->Velocity().length_2d();

		if (max_speed_with_weapon + 1.0 <= Velocity) {
			g::pCmd->forwardmove = 0;
			g::pCmd->sidemove = 0;
		}
		else {
			g::pCmd->forwardmove = forward_velocity * max_speed_with_weapon;
			g::pCmd->sidemove = side_velocity * max_speed_with_weapon;
		}
	}
	else
	{
		vec3_t angle;
		math::vector_angles(g::pLocalPlayer->Velocity(), angle);

		float speed = g::pLocalPlayer->Velocity().length();

		angle.y = g::pCmd->viewangles.y - angle.y;

		vec3_t direction;
		math::angle_vectors(angle, direction);

		vec3_t stop = direction * -speed;

		g::pCmd->forwardmove = stop.x;
		g::pCmd->sidemove = stop.y;
	}
}

bool aimbot::predict_stop()
{
	if (!g::pLocalPlayer || !g::pLocalPlayer->IsAlive()) 
		return false;
	
	if ( !g::pLocalWeapon || !g::pLocalWeaponData || !g::pLocalWeapon->IsGun( ) )
		return false;

	if (g::pLocalPlayer->Velocity().length_2d() < 5.f)
		return false;

	vec3_t predicted_eye_pos = (g::pLocalPlayer->Origin() + g::pLocalPlayer->Velocity() * interfaces::globals->interval_per_tick) * 3.f;
	for (int i = 1; i <= interfaces::globals->max_clients; i++)
	{
		auto records = &g_LagCompensation->GetPlayerRecords(i);
		if (records->empty())
			continue;

		auto record = &records->front();
		if (!record)
			continue;

		if (!record->m_pEntity || !record->m_pEntity->IsAlive() || !record->m_pEntity->IsPlayer( ) || record->m_pEntity->Dormant( ))
			continue;

		g_aim_data->setup_hitboxes(record);
		if (!g_aim_data->enabled_hitboxes.empty())
		{
			// we can hit p!
			auto damage = 0.f;
			auto hitbox = -1;
			const vec3_t predicted_point = g_aim_data->get_best_damage(*record, damage, hitbox, &predicted_eye_pos);
			if (predicted_point != vec3_t(0, 0, 0) && damage > 5.f && hitbox != -1)
			{
				/* set actual peek state */
				g::bPeekState = true;
			}
			else
			{
				/* set peek state to false due to lack of point/damage/hitbox */
				g::bPeekState = false;
			}
		}
	}
	return g::bPeekState;
}

float aim_data_t::get_point_scale(C_LagRecord log, int ihitbox, const vec3_t* pos)
{
	BaseEntity* player = interfaces::entity_list->get < BaseEntity >(log.m_nEntIndex);

	const auto model = player->GetModel();
	if (!model)
		return 0.f;

	const auto studio_hdr = interfaces::model_info->get_studio_model(model);
	if (!studio_hdr)
		return 0.f;

	auto hitbox = studio_hdr->pHitbox(ihitbox, 0);
	if (!hitbox)
		return 0.f;

	const auto point_scale_cfg = g_aimbot->rage_settings.mp_scale * 0.01f;
	auto point_scale = point_scale_cfg;
	
	auto& mat = log.m_Matricies[log.m_RotationMode][hitbox->bone];
	const auto mod = hitbox->radius != -1.f ? hitbox->radius : 0.f;
	
	vec3_t max;
	vec3_t min;
	math::transform_vector(hitbox->maxs + mod, mat, max);
	math::transform_vector(hitbox->mins - mod, mat, min);
	
	auto center = (min + max) * 0.5f;
	
	// calculate dynamic scale
	if (g_aimbot->rage_settings.adaptive_scale) {
		math::vector_i_transform(center, mat, center);

		auto spread = prediction.flSpread + prediction.flInaccuracy;
		auto distance = center.distance_to(*pos);

		distance /= sin(DEG2RAD(90.0f - RAD2DEG(spread)));
		spread = sin(spread);

		// get radius and set spread.
		auto radius = std::max(mod - distance * spread, 0.0f);
		point_scale = std::clamp(radius / mod, 0.0f, 1.0f);
	}
	return point_scale;
}

int aim_data_t::get_minimum_damage(BaseEntity* pPlayer, bool IsAutoWall)
{
	int minimum_damage = IsAutoWall ? g_aimbot->rage_settings.min_dmg_awall : g_aimbot->rage_settings.min_dmg;
	if (g_keyhandler.CheckKey(g_aimbot->rage_settings.min_dmg_key, g_aimbot->rage_settings.min_dmg_style) && g_aimbot->rage_settings.min_dmg_key)
		minimum_damage = g_aimbot->rage_settings.min_dmg_override;

	minimum_damage = std::min(minimum_damage, pPlayer->Health());
	return minimum_damage;
}

bool CollidePoint(const vec3_t& vecStart, const vec3_t& vecEnd, studio_box_t* pHitbox, matrix_t* aMatrix)
{
	static void* lpClipRayToHitbox = pattern::Scan("client.dll", "55 8B EC 83 E4 F8 F3 0F 10 42");
	if (!pHitbox || !aMatrix)
		return false;
	
	ray_t Ray;
	Ray.initialize(vecStart, vecEnd);

	trace_t Trace;
	Trace.fractionLeftSolid = 1.0f;
	Trace.startSolid = false;
	
	typedef int(__fastcall* ClipRayToHitbox_t)(const ray_t&, studio_box_t*, matrix_t&, trace_t&);
	return ((ClipRayToHitbox_t)(lpClipRayToHitbox))(Ray, pHitbox, aMatrix[ pHitbox->bone ], Trace) >= 0;
}

bool IsSafePoint(BaseEntity* player, C_LagRecord& log, vec3_t vecStart, vec3_t vecPoint, int hitbox)
{
	studio_hdr_t* pStudioHdr = (studio_hdr_t*)(interfaces::model_info->get_studio_model(player->GetModel()));
	if ( !pStudioHdr )
		return false;

	studio_hitbox_set_t* pHitset = pStudioHdr->hitbox_set(player->HitboxSet());
	if (!pHitset)
		return false;

	studio_box_t* pHitbox = pHitset->hitbox(hitbox);
	if (!pHitbox)
		return false;

	int nPointsCollided = 0;
	if ( CollidePoint(vecStart, vecPoint, pHitbox, log.m_Matricies[ 1 ].data()))
		nPointsCollided++;
	if ( CollidePoint(vecStart, vecPoint, pHitbox, log.m_Matricies[ 2 ].data()))
		nPointsCollided++;
	if ( CollidePoint(vecStart, vecPoint, pHitbox, log.m_Matricies[ 3 ].data()))
		nPointsCollided++;

	return nPointsCollided >= 2;
}

vec3_t aim_data_t::get_best_damage(C_LagRecord& log, float& damage, int& hitbox, vec3_t* pos)
{
	vec3_t* shoot_position{ pos ? pos : &g::pLocalPlayer->GetEyePosition(false)};
	vec3_t final_position = { };

	if (!shoot_position)
		return vec3_t(0.f, 0.f, 0.f);

	BaseEntity* player = interfaces::entity_list->get < BaseEntity > ( log.m_nEntIndex );
	bool done = false;

	C_LagRecord backup;
	g_LagCompensation->FillRecord( player, backup );
	log.Apply(player);

	std::vector<vec3_t> points;
	hitscan_data_t         scan_data;

	const auto is_zeus = g::pLocalWeapon->ItemDefinitionIndex() == WEAPON_TASER;
	const auto is_knife = !is_zeus && g::pLocalWeaponData->weapon_type == WEAPONTYPE_KNIFE;

	// iterate hitboxes.
	for (const auto& it : enabled_hitboxes) {
		done = false;

		setup_hitbox_points(log, points, it.m_index, shoot_position);
		if(points.empty())
			continue;

		// iterate points on hitbox.
		for (const auto& point : points) {

			Penetration::PenetrationInput_t penInput;

			if (g_aimbot->rage_settings.force_safepoints) {
				if (!IsSafePoint(player, log, *shoot_position, point, it.m_index))
					continue;
			}

			penInput.flDamage = is_zeus ? player->Health() + 1.f : get_minimum_damage(player, false);
			penInput.flDamagePen = is_zeus ? player->Health() + 1.f : get_minimum_damage(player, true);
			penInput.bCanPenetrate = true;
			penInput.pTarget = player;
			penInput.pFrom = g::pLocalPlayer;
			penInput.vecPos = point;
			penInput.vecFrom = *shoot_position;

			Penetration::PenetrationOutput_t penOutput;

			float best_damage_per_hitbox = 0, distance_to_center = 0;
			auto distance = point.dist_to(points.front());

			// we can hit p!
			if (Penetration::bRunPenetration(&penInput, &penOutput)) //TODO: Add some optimization. 
			{
				const auto dist = shoot_position->distance_to(point);
				if (is_knife && dist > 64.f) {
					continue;
				}

				// nope we did not hit head..
				if (it.m_index == hitbox_head && penOutput.iHitgroup != hitgroup_head) {
					continue;
				}

				if (is_knife)
					penOutput.flDamage = 100.f / dist;

				// prefered hitbox, just stop now.
				if (it.m_mode == prefer)
					done = true;

				// this hitbox requires lethality to get selected, if that is the case.
				// we are done, stop now.
				else if (it.m_mode == lethal && penOutput.flDamage >= player->Health())
					done = true;

				// 2 shots will be sufficient to kill.
				else if (it.m_mode == lethal2 && (penOutput.flDamage * 2.f) >= player->Health())
					done = true;

				if (best_damage_per_hitbox > 0 && (penOutput.flDamage >= best_damage_per_hitbox || (penOutput.flDamage >= (best_damage_per_hitbox - 15)) && distance < distance_to_center))
				{
					scan_data.m_damage = penOutput.flDamage;
					scan_data.m_pos = point;
					scan_data.m_hitbox = it.m_index;
					best_damage_per_hitbox = penOutput.flDamage;
					distance_to_center = distance;
				}
				else {
					if (((penOutput.flDamage > scan_data.m_damage || (penOutput.flDamage >= (scan_data.m_damage - 15)) && distance < distance_to_center) || done))
					{
						// save new best data.
						scan_data.m_damage = penOutput.flDamage;
						scan_data.m_pos = point;
						scan_data.m_hitbox = it.m_index;
						best_damage_per_hitbox = penOutput.flDamage;
						distance_to_center = distance;
					}
				}
			}
		}

		// ghetto break out of outer loop.
		if (done)
			break;
	}

	// we found something that we can damage.
	// set out vars.
	if (scan_data.m_damage > 0.f) {
		backup.Apply(player);
		hitbox = scan_data.m_hitbox;
		damage = scan_data.m_damage;
		final_position = scan_data.m_pos;
	}
	else {
		hitbox = -1;
		damage = 0.f;
		final_position = vec3_t(0, 0, 0);
	}
	
	return final_position;
}

std::vector <vec3_t> aim_data_t::setup_hitbox_points(C_LagRecord& record, std::vector< vec3_t >& points, int index, vec3_t* pos) {
	// reset points.
	points.clear();

	BaseEntity* player = interfaces::entity_list->get < BaseEntity >(record.m_nEntIndex);

	const model_t* model = player->GetModel();
	if (!model)
		return { };

	auto hdr = interfaces::model_info->get_studio_model(model);
	if (!hdr)
		return { };

	auto set = hdr->hitbox_set(player->HitboxSet());
	if (!set)
		return { };

	auto bbox = set->hitbox(index);
	if (!bbox)
		return { };

	// get hitbox scales.
	const float scale = get_point_scale(record, index, pos);

	// these indexes represent boxes.
	if (bbox->radius <= 0.f) {
		// references: 
		// https://developer.valvesoftware.com/wiki/Rotation_Tutorial
		// CBaseAnimating::GetHitboxBonePosition
		// CBaseAnimating::DrawServerHitboxes

		// convert rotation angle to a matrix.
		matrix_t rot_matrix;
		g::AngleMatrix(bbox->rotation, rot_matrix);

		// apply the rotation to the entity input space (local).
		matrix_t matrix;
		math::concat_transforms(record.m_Matricies[record.m_RotationMode][bbox->bone], rot_matrix, matrix);

		// extract origin from matrix.
		vec3_t origin = matrix.get_origin();

		// compute raw center point.
		vec3_t center = (bbox->mins + bbox->maxs) / 2.f;

		// the feet hiboxes have a side, heel and the toe.
		if (index == hitbox_right_foot || index == hitbox_left_foot) {
			points.push_back({ center.x, center.y, center.z });
		}

		// nothing to do here we are done.
		if (points.empty())
			return { };

		// rotate our bbox points by their correct angle
		// and convert our points to world space.
		for (auto& p : points) {
			// VectorRotate.
			// rotate point by angle stored in matrix.
			p = { p.dot(matrix[0]), p.dot(matrix[1]), p.dot(matrix[2]) };

			// transform point to world space.
			p += origin;
		}
	}

	// these hitboxes are capsules.
	else {
		// factor in the pointscale.
		float point_scale = bbox->radius * scale;

		// compute raw center point.
		vec3_t center = (bbox->mins + bbox->maxs) / 2.f;

		// head has 5 points.
		if (index == hitbox_head) {
			// rotation matrix 45 degrees.
			// https://math.stackexchange.com/questions/383321/rotating-x-y-points-45-degrees
			// std::cos( deg_to_rad( 45.f ) )
			constexpr float rotation = 0.70710678f;

			points.push_back({ bbox->maxs.x + (rotation * point_scale), bbox->maxs.y + (-rotation * point_scale), bbox->maxs.z });
			points.push_back({ bbox->maxs.x, bbox->maxs.y, bbox->maxs.z - point_scale });

			// get animstate ptr.
			auto state = record.m_pEntity->AnimState();

			// add this point only under really specific circumstances.
			// if we are standing still and have the lowest possible pitch pose.
			if (state && record.m_Velocity.length() <= 0.1f && record.m_EyeAngles.x <= state->m_flAimPitchMin) {
				// bottom point.
				points.push_back({ bbox->maxs.x - point_scale, bbox->maxs.y, bbox->maxs.z });
			}
		}

		// body has 5 points.
		else if (index == hitbox_stomach) {
			// center.
			points.push_back(center);

			// back.
			points.push_back({ center.x, bbox->maxs.y - point_scale, center.z });
		}

		else if (index == hitbox_pelvis || index == hitbox_upper_chest) {
			// back.
			points.push_back({ center.x, bbox->maxs.y - point_scale, center.z });
		}

		// other stomach/chest hitboxes have 2 points.
		else if (index == hitbox_lower_chest || index == hitbox_chest) {
			// add center.
			points.push_back(center);

			// add extra point on back.
			//if (g_menu.main.aimbot.multipoint.get(1))
				points.push_back({ center.x, bbox->maxs.y - point_scale, center.z });
		}

		else if (index == hitbox_right_calf || index == hitbox_left_calf) {
			// add center.
			points.push_back(center);
		}

		else if (index == hitbox_right_thigh || index == hitbox_left_thigh) {
			// add center.
			points.push_back(center);
		}

		// arms get only one point.
		else if (index == hitbox_right_upper_arm || index == hitbox_left_upper_arm) {
			// elbow.
			points.push_back(center);
		}

		// nothing left to do here.
		if (points.empty())
			return { };

		// transform capsule points.
		for (auto& p : points)
			math::transform_vector(p, record.m_Matricies[record.m_RotationMode][bbox->bone], p);
	}

	return points;
}

bool aim_data_t::select_main_target(C_LagRecord* record, float damage) {
	if (damage > best_damage)
	{
		best_damage = damage;
		return true;
	}

	return false;
}

void hit_chance::build_seed_table()
{
	constexpr float pi_2 = 2.0f * (float)M_PI;
	for (size_t i = 0; i < 256; ++i)
	{
		math::random_seed(i);

		const float rand_a = math::random_float(0.0f, 1.0f);
		const float rand_pi_a = math::random_float(0.0f, pi_2);
		const float rand_b = math::random_float(0.0f, 1.0f);
		const float rand_pi_b = math::random_float(0.0f, pi_2);

		hit_chance_records[i] =
		{
			{  rand_a, rand_b						     },
			{  std::cos(rand_pi_a), std::sin(rand_pi_a)	 },
			{  std::cos(rand_pi_b), std::sin(rand_pi_b)	 }
		};
	}
}

bool hit_chance::intersects_bb_hitbox(vec3_t start, vec3_t delta, vec3_t min, vec3_t max)
{
	float d1, d2, f;
	auto start_solid = true;
	auto t1 = -1.0, t2 = 1.0;

	const float _start[3] = { start.x, start.y, start.z };
	const float _delta[3] = { delta.x, delta.y, delta.z };
	const float mins[3] = { min.x, min.y, min.z };
	const float maxs[3] = { max.x, max.y, max.z };

	for (auto i = 0; i < 6; ++i)
	{
		if (i >= 3)
		{
			const auto j = (i - 3);

			d1 = _start[j] - maxs[j];
			d2 = d1 + _delta[j];
		}
		else
		{
			d1 = -_start[i] + mins[i];
			d2 = d1 - _delta[i];
		}

		if (d1 > 0 && d2 > 0)
		{
			start_solid = false;
			return false;
		}

		if (d1 <= 0 && d2 <= 0)
			continue;

		if (d1 > 0)
			start_solid = false;

		if (d1 > d2)
		{
			f = d1;
			if (f < 0)
				f = 0;

			f /= d1 - d2;
			if (f > t1)
				t1 = f;
		}
		else
		{
			f = d1 / (d1 - d2);
			if (f < t2)
				t2 = f;
		}
	}

	return start_solid || (t1 < t2&& t1 >= 0.0f);
}

bool __vectorcall hit_chance::intersects_hitbox(vec3_t eye_pos, vec3_t end_pos, vec3_t min, vec3_t max, float radius)
{
	auto dist = math::dist_segment_to_segment(eye_pos, end_pos, min, max);

	return (dist < radius);
}

std::vector<hit_chance::hitbox_data_t> hit_chance::get_hitbox_data(C_LagRecord& log, int hitbox)
{
	std::vector<hitbox_data_t> hitbox_data;
	BaseEntity* target = interfaces::entity_list->get < BaseEntity >(log.m_nEntIndex);

	const auto model = target->GetClientRenderable()->GetModel();

	if (!model)
		return {};

	const auto hdr = interfaces::model_info->get_studio_model(model);

	if (!hdr)
		return {};

	const auto set = hdr->hitbox_set(target->HitboxSet());

	if (!set)
		return {};

	std::array<matrix_t, 256> bone_matrix = log.m_Matricies[log.m_RotationMode];

	vec3_t min, max;

	if (hitbox == -1)
	{
		for (int i = 0; i < set->hitbox_count; ++i)
		{
			const auto box = set->hitbox(i);

			if (!box)
				continue;

			float radius = box->radius;
			const auto is_capsule = radius != -1.f;

			if (is_capsule)
			{
				math::transform_vector(box->mins, bone_matrix[box->bone], min);
				math::transform_vector(box->maxs, bone_matrix[box->bone], max);
			}
			else
			{
				math::transform_vector(math::vector_rotate(box->mins, box->rotation), bone_matrix[box->bone], min);
				math::transform_vector(math::vector_rotate(box->maxs, box->rotation), bone_matrix[box->bone], max);
				radius = min.distance_to(max);
			}

			hitbox_data.emplace_back(hitbox_data_t{ min, max, radius, box, box->bone, box->rotation });
		}
	}
	else
	{
		const auto box = set->hitbox(hitbox);

		if (!box)
			return {};

		float radius = box->radius;
		const auto is_capsule = radius != -1.f;

		if (is_capsule)
		{
			math::transform_vector(box->mins, bone_matrix[box->bone], min);
			math::transform_vector(box->maxs, bone_matrix[box->bone], max);
		}
		else
		{
			math::transform_vector(math::vector_rotate(box->mins, box->rotation), bone_matrix[box->bone], min);
			math::transform_vector(math::vector_rotate(box->maxs, box->rotation), bone_matrix[box->bone], max);
			radius = min.distance_to(max);
		}

		hitbox_data.emplace_back(hitbox_data_t{ min, max, radius, box, box->bone, box->rotation });
	}

	return hitbox_data;
}

vec3_t hit_chance::get_spread_direction(BaseWeapon* weapon, vec3_t angles, int seed)
{
	if (!weapon)
		return vec3_t();

	const int   rnsd = (seed & 0xFF);
	const auto* data = &hit_chance_records[rnsd];

	if (!data)
		return vec3_t();

	float rand_a = data->random[0];
	float rand_b = data->random[1];

	if (weapon->ItemDefinitionIndex() == WEAPON_NEGEV)
	{
		auto weapon_info = weapon ? interfaces::weapon_system->get_weapon_data(weapon->ItemDefinitionIndex()) : nullptr;

		if (weapon_info && weapon_info->weapon_recoil_seed < 3)
		{
			rand_a = 1.0f - std::pow(rand_a, static_cast<float>(3 - weapon_info->weapon_recoil_seed + 1));
			rand_b = 1.0f - std::pow(rand_b, static_cast<float>(3 - weapon_info->weapon_recoil_seed + 1));
		}
	}

	const float rand_inaccuracy = rand_a * prediction.flInaccuracy;
	const float rand_spread = rand_b * prediction.flSpread;

	const float spread_x = data->inaccuracy[0] * rand_inaccuracy + data->spread[0] * rand_spread;
	const float spread_y = data->inaccuracy[1] * rand_inaccuracy + data->spread[1] * rand_spread;

	vec3_t forward, right, up;
	math::angle_vectors(angles, &forward, &right, &up);

	return forward + right * spread_x + up * spread_y;
}

bool hit_chance::can_intersect_hitbox(const vec3_t start, const vec3_t end, vec3_t spread_dir, C_LagRecord& log, int hitbox)
{
	const auto hitbox_data = get_hitbox_data(log, hitbox);

	if (hitbox_data.empty())
		return false;

	auto intersected = false;
	vec3_t delta;
	vec3_t start_scaled;

	for (const auto& it : hitbox_data)
	{
		const auto is_capsule = it.m_radius != -1.f;
		if (!is_capsule)
		{
			math::vector_i_transform(start, log.m_Matricies[log.m_RotationMode][it.m_bone], start_scaled);
			math::vector_i_rotate(spread_dir * 8192.f, log.m_Matricies[log.m_RotationMode][it.m_bone], delta);
			if (intersects_bb_hitbox(start_scaled, delta, it.m_min, it.m_max))
			{
				intersected = true;
				break; //Note - cannot hit more than one hitbox.
			}
		}
		else if (intersects_hitbox(start, end, it.m_min, it.m_max, it.m_radius))
		{
			intersected = true;
			break;//Note - cannot hit more than one hitbox.
		}
		else
		{
			intersected = false;
			break;
		}
	}

	return intersected;
}

bool hit_chance::can_hit(C_LagRecord& log, BaseWeapon* weapon, vec3_t angles, int hitbox)
{
	BaseEntity* target = interfaces::entity_list->get < BaseEntity >(log.m_nEntIndex);

	if (!target || !weapon)
		return false;

	auto weapon_info = weapon ? interfaces::weapon_system->get_weapon_data(weapon->ItemDefinitionIndex()) : nullptr;;

	if (!weapon_info)
		return false;

	build_seed_table();

	if (weapon->ItemDefinitionIndex() == WEAPON_SCAR20 || weapon->ItemDefinitionIndex() == WEAPON_G3SG1)
	{
		if ((prediction.flInaccuracy <= 0.0380f) && weapon->ZoomLevel() == 0)
		{
			return true;
		}
	}

	if ((weapon->ItemDefinitionIndex() == WEAPON_SSG08 || weapon->ItemDefinitionIndex() == WEAPON_REVOLVER) && !(g::pLocalPlayer->Flags() & FL_ONGROUND))
	{
		if ((prediction.flInaccuracy < 0.009f))
		{
			return true;
		}
	}

	const auto hitchance_cfg = weapon->ItemDefinitionIndex() == WEAPON_TASER ? config.zeus_hc : g_aimbot->rage_settings.hitchance_amount;
	const int hits_needed = (hitchance_cfg * 256) / 100;
	int hits = 0;

	for (int i = 0; i < 256; ++i) {
		vec3_t spread_dir = get_spread_direction(weapon, angles, i);
		vec3_t end_pos = g::pLocalPlayer->GetEyePosition(true) + (spread_dir * 8192.f);

		if (can_intersect_hitbox(g::pLocalPlayer->GetEyePosition(true), end_pos, spread_dir, log, hitbox))
			hits++;

		if (hits >= hits_needed)
			return true;
	}

	return false;
}

void aimbot::setup_weapons()
{
	if (!g::pLocalWeapon)
		return;

	int currentWeapon = -1;
	const auto is_zeus = g::pLocalWeapon->ItemDefinitionIndex() == WEAPON_TASER;
	const auto is_knife = !is_zeus && g::pLocalWeaponData->weapon_type == WEAPONTYPE_KNIFE;

	switch (g::pLocalWeapon->ItemDefinitionIndex())
	{
	case WEAPON_AK47:
	case WEAPON_M4A1:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_FAMAS:
	case WEAPON_SG556:
	case WEAPON_GALILAR:
	case WEAPON_AUG:
		currentWeapon = weapon_type_rage::wep_rifle; break;
	case WEAPON_MAG7:
	case WEAPON_NOVA:
	case WEAPON_XM1014:
	case WEAPON_SAWEDOFF:
		currentWeapon = weapon_type_rage::wep_shotgun; break;
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_P90:
	case WEAPON_M249:
	case WEAPON_NEGEV:
	case WEAPON_UMP45:
		currentWeapon = weapon_type_rage::wep_smg; break;
	case WEAPON_SCAR20:
	case WEAPON_G3SG1:
		currentWeapon = weapon_type_rage::wep_auto; break;
	case WEAPON_GLOCK:
	case WEAPON_HKP2000:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_TEC9:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_P250:
		currentWeapon = weapon_type_rage::wep_pistol; break;
	case WEAPON_SSG08:
		currentWeapon = weapon_type_rage::wep_scout; break;
	case WEAPON_AWP:
		currentWeapon = weapon_type_rage::wep_awp; break;
	case WEAPON_DEAGLE:
		currentWeapon = weapon_type_rage::wep_deagle; break;
	case WEAPON_REVOLVER:
		currentWeapon = weapon_type_rage::wep_revolver; break;
	default: currentWeapon = -1;
	}

	if (is_zeus || is_knife)
	{
		rage_settings.ragebot_enable = is_zeus ? config.zeusbot : config.knifebot;
		rage_settings.autoshoot_enable = is_zeus ? config.zeusbot : config.knifebot;
		return;
	}
	if (currentWeapon <= -1)
	{
		rage_settings = ragebot_t();
		return;
	}
	rage_settings = config.ragesettings[currentWeapon];
	rage_settings.ragebot_enable = true;
}