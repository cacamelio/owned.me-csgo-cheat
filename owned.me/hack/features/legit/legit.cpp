#include "../features.hpp"

bool lines_goes_smoke(vec3_t start, vec3_t end) 
{
	static auto lines_goes_smokefn = (bool(*)(vec3_t start, vec3_t end))pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
	return lines_goes_smokefn(start, end);
}

void legitbot::work(UserCmd* cmd)
{
	if (!g::pLocalPlayer || !g::pLocalPlayer->IsAlive())
		return;

	setup_weapon();	

	if (trigger_settings.trigger_enable)
		run_trigger(cmd);

	if (legit_settings.legit_enable)
		run_aim(cmd);

	if (config.backtrack_enable)
		legit_records::run(cmd);
}

void legitbot::run_aim(UserCmd* cmd)
{
	if (!g::pLocalWeapon || !g::pLocalWeaponData)
		return;

	if (!g::pLocalWeapon->IsGun() || !g::pLocalWeapon->Clip1Count())
		return;

	if (legit_settings.kill_delay > 1 && killDelay >= interfaces::globals->tick_count)
		return;

	if (!g_keyhandler.CheckKey(legit_settings.legit_key, legit_settings.legit_key_style))
		return;

	float bestFov = legit_settings.fov;

	vec3_t bestTarget;
	vec3_t shootPos = g::pLocalPlayer->EyePos();

	std::vector<int> aimBones;

	aimBones.clear();

	if (legit_settings.hitboxes[0]) 
	{ //head
		aimBones.emplace_back(8);
	}

	if (legit_settings.hitboxes[1]) 
	{ // body
		aimBones.emplace_back(6); aimBones.emplace_back(5); aimBones.emplace_back(4); aimBones.emplace_back(3);
	}

	if (legit_settings.hitboxes[2]) 
	{  //hands
		aimBones.emplace_back(12); aimBones.emplace_back(40);
	}

	if (legit_settings.hitboxes[3])
	{  //legs
		aimBones.emplace_back(65); aimBones.emplace_back(66); aimBones.emplace_back(72); aimBones.emplace_back(73);
	}

	static auto gameType = interfaces::console->FindVar(XOR("game_type"))->GetInt();
	static auto weaponRecoil = interfaces::console->FindVar(XOR("weapon_recoil_scale"))->GetFloat();

	for (auto aimPlayer : entity_cache::Groups[CGroupType::PLAYERS_ALL])
	{
		if (g::pLocalPlayer->IsInAir() && !legit_settings.checks[0]) //air
			continue;

		if (g::pLocalPlayer->IsFlashed() && !legit_settings.checks[1]) // flash
			continue;

		if (aimPlayer == g::pLocalPlayer || !aimPlayer->IsAlive() || aimPlayer->Dormant() || aimPlayer->HasGunGameImmunity())
			continue;

		if (gameType == 6) 
		{
			if ((g::pLocalPlayer->SurvivalTeam() != -1) && (g::pLocalPlayer->SurvivalTeam() == g::pLocalPlayer->SurvivalTeam()))
				continue;
		}
		else if (aimPlayer->TeamNum() == g::pLocalPlayer->TeamNum() && !legit_settings.checks[2]) //team
			continue;

		for (const auto aimBone : aimBones)
		{
			auto bonePosition = aimPlayer->GetBonePos(aimBone);
			auto playerVisible = g::pLocalPlayer->CanSeePlayerPos(aimPlayer, bonePosition);
			
			Penetration::PenetrationInput_t penInput;

			penInput.flDamage = legit_settings.wall_dmg;
			penInput.flDamagePen = legit_settings.wall_dmg; //TODO: make penetration mindamage.
			penInput.bCanPenetrate = true; //TODO: make penetration checkbox.
			penInput.pTarget = aimPlayer;
			penInput.pFrom = g::pLocalPlayer;
			penInput.vecPos = bonePosition;

			Penetration::PenetrationOutput_t penOutput;

			auto playerSmoke = lines_goes_smoke(shootPos, bonePosition);

			if (!Penetration::bRunPenetration(&penInput, &penOutput))
				continue;

			if (!playerVisible && !legit_settings.checks[3] || //wall
				penOutput.flDamage < legit_settings.wall_dmg) //smoke
				continue;

			if (playerSmoke && !legit_settings.checks[4])
				continue;

			vec3_t aimAngle = math::calculate_angle(shootPos, bonePosition, cmd->viewangles).clamped();
			aimAngle -= (g::pLocalPlayer->AimPunchAngle() * (weaponRecoil * legit_settings.rcs));

			auto aimFov = std::hypotf(aimAngle.x, aimAngle.y);

			if (aimFov < bestFov)
			{
				bestTarget = bonePosition;
				bestFov = aimFov;
			}

		}

		if (bestTarget == vec3_t(0, 0, 0))
			return;

		vec3_t finalAngle = math::calculate_angle(shootPos, bestTarget, cmd->viewangles).clamped() /= legit_settings.smooth;
		finalAngle -= (g::pLocalPlayer->AimPunchAngle() * (weaponRecoil * legit_settings.rcs)) /= legit_settings.smooth;
		cmd->viewangles += finalAngle.clamped();

		if (!legit_settings.legit_silent)
			interfaces::engine->set_view_angles(cmd->viewangles);
	}
}

void legitbot::run_trigger(UserCmd* pCmd)
{
	auto server_time = g::pLocalPlayer->GetTickBase() * interfaces::globals->interval_per_tick;

	const auto time = server_time;

	if (g::pLocalPlayer->NextAttack() > server_time || g::pLocalPlayer->IsDefusing() || g::pLocalPlayer->WaitNextAttack())
		return;

	const auto activeWeapon = g::pLocalWeapon;
	if (!activeWeapon || !activeWeapon->Clip1Count() || activeWeapon->NextPrimaryAttack() > time || activeWeapon->IsKnife())
		return;

	if (!g_keyhandler.CheckKey(trigger_settings.trigger_key, trigger_settings.trigger_key_style))
		return;

	static auto lastTime = 0.0f;
	static auto lastContact = 0.0f;

	const auto now = interfaces::globals->realtime;

	if (now - lastContact < trigger_settings.burstTime)
	{
		pCmd->buttons |= IN_ATTACK;
		return;
	}
	lastContact = 0.0f;

	if (now - lastTime < trigger_settings.shotDelay / 1000.0f)
		return;

	if (!trigger_settings.ignoreFlash && g::pLocalPlayer->IsFlashed())
		return;

	if (trigger_settings.scopedOnly && activeWeapon->IsZoomable(false) && !g::pLocalPlayer->IsScoped())
		return;

	const auto weaponData = g::pLocalWeaponData;
	if (!weaponData)
		return;

	const auto startPos = g::pLocalPlayer->EyePos();
	const auto endPos = startPos + vec3_t::fromAngle(pCmd->viewangles) * weaponData->weapon_range;

	if (!trigger_settings.ignoreSmoke && lines_goes_smoke(startPos, endPos))
		return;

	trace_t trace;

	Penetration::PenetrationInput_t penInput;

	penInput.flDamage = trigger_settings.minDamage;
	penInput.flDamagePen = trigger_settings.minDamageAutoWall; //TODO: make penetration mindamage.
	penInput.bCanPenetrate = !trigger_settings.visibleOnly; //TODO: make penetration checkbox.
	penInput.pFrom = g::pLocalPlayer;
	penInput.pTarget = nullptr;
	penInput.vecPos = endPos;

	Penetration::PenetrationOutput_t penOutput;

	if (!Penetration::bRunPenetration(&penInput, &penOutput))
		return;

	lastTime = now;
	
	if (!trigger_settings.hitgroup.empty())
	{
		if (penOutput.iHitgroup == hitgroup_head && !trigger_settings.hitgroup[0])
			return;
		if (penOutput.iHitgroup == hitgroup_chest && !trigger_settings.hitgroup[1])
			return;
		if (penOutput.iHitgroup == hitgroup_stomach && !trigger_settings.hitgroup[2])
			return;
		if (penOutput.iHitgroup == hitgroup_rightarm && !trigger_settings.hitgroup[3])
			return;
		if (penOutput.iHitgroup == hitgroup_leftarm && !trigger_settings.hitgroup[4])
			return;
		if (penOutput.iHitgroup == hitgroup_leftleg && !trigger_settings.hitgroup[5])
			return;
		if (penOutput.iHitgroup == hitgroup_rightleg && !trigger_settings.hitgroup[6])
			return;
	}

	if (!penOutput.pTarget || !penOutput.pTarget->IsPlayer())
		return;

	const auto distance = fabs(vec3_t(penOutput.pTarget->Origin() - penInput.pFrom->Origin()).length_2d());
	if (trigger_settings.distance && trigger_settings.distance < distance)
		return;

	const int targetHealth = penOutput.pTarget->Health() + static_cast<int>(penOutput.pTarget->Health() * 0.1f);
	auto minDamage = penOutput.bPenetrated ?
		std::min(trigger_settings.minDamageAutoWall, targetHealth) :
		std::min(trigger_settings.minDamage, targetHealth);

	if (penOutput.flDamage >= minDamage)
	{
		pCmd->buttons |= IN_ATTACK;
		lastTime = 0.0f;
		lastContact = now;
	}
}

void legitbot::setup_weapon()
{
	if (!g::pLocalWeapon)
		return;

	int currentWeapon = -1;

	switch (g::pLocalWeapon->ItemDefinitionIndex())
	{
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_FAMAS:
		case WEAPON_SG556:
		case WEAPON_GALILAR:
		case WEAPON_AUG:
			currentWeapon = weapon_type_legit::rifles; break;
		case WEAPON_MAG7:
		case WEAPON_NOVA:
		case WEAPON_XM1014:
		case WEAPON_SAWEDOFF:
			currentWeapon = weapon_type_legit::shotguns; break;
		case WEAPON_MP7:
		case WEAPON_MP9:
		case WEAPON_P90:
		case WEAPON_M249:
		case WEAPON_NEGEV:
		case WEAPON_UMP45:
			currentWeapon = weapon_type_legit::smgs; break;
		case WEAPON_SCAR20:
		case WEAPON_G3SG1:
			currentWeapon = weapon_type_legit::autos; break;
		case WEAPON_GLOCK:
		case WEAPON_HKP2000:
		case WEAPON_USP_SILENCER:
		case WEAPON_CZ75A:
		case WEAPON_TEC9:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_P250:
			currentWeapon = weapon_type_legit::pistols; break;
		case WEAPON_SSG08:
			currentWeapon = weapon_type_legit::scout; break;
		case WEAPON_AWP:
			currentWeapon = weapon_type_legit::awp; break;
		case WEAPON_DEAGLE:
			currentWeapon = weapon_type_legit::deagle; break;
		case WEAPON_REVOLVER:
			currentWeapon = weapon_type_legit::revolver; break;
		default: currentWeapon = -1;
	}

	if (currentWeapon <= -1)
	{
		trigger_settings = triggerbot_t();
		legit_settings = legitbot_t();
		return;
	}

	trigger_settings = config.triggersettings[currentWeapon];
	legit_settings = config.legitsettings[currentWeapon];
}

void legit_records::update(int stage)
{
	if (!config.backtrack_enable || !g::pLocalPlayer || !g::pLocalPlayer->IsAlive()) 
	{
		for (auto& record : records)
			record.clear();

		return;
	}

	if (stage == FRAME_RENDER_START)
	{
		for (int i = 1; i <= interfaces::globals->max_clients; i++) 
		{
			auto entity = reinterpret_cast<BaseEntity*>(interfaces::entity_list->get_client_entity(i));
			if (!entity || entity == g::pLocalPlayer || entity->Dormant() || !entity->IsAlive() || entity->TeamNum() == g::pLocalPlayer->TeamNum()) 
			{
				records[i].clear();
				continue;
			}

			if (!records[i].empty() && (records[i].front().simTime == entity->SimulationTime()))
				continue;

			auto nci = interfaces::engine->get_net_channel_info();

			if (!nci)
				continue;

			storedRecords record{ };
			record.hitbox = entity->GetHitboxPos(hitbox_head);
			record.origin = entity->AbsOrigin();
			record.simTime = entity->SimulationTime();

			entity->SetupBones(record.matrix, 128, 0x7FF00, interfaces::globals->cur_time);

			records[i].push_front(record);

			while (config.pingspike ? (records[i].size() > 3 && records[i].size() > static_cast<size_t>(time_to_ticks(0.2f) + nci->get_latency(FLOW_OUTGOING))) : (records[i].size() > 3 && records[i].size() > static_cast<size_t>(time_to_ticks(0.2f))))
				records[i].pop_back();

			if (auto invalid = std::find_if(std::cbegin(records[i]), std::cend(records[i]), [](const storedRecords& rec) { return !valid(rec.simTime); }); invalid != std::cend(records[i]))
				records[i].erase(invalid, std::cend(records[i]));
		}
	}
}

void legit_records::run(UserCmd* cmd) 
{
	if (!config.backtrack_enable)
		return;

	if (!(cmd->buttons & IN_ATTACK))
		return;

	if (!g::pLocalPlayer)
		return;

	auto local_eye_pos = g::pLocalPlayer->EyePos();

	auto best_fov = 255.f;
	BaseEntity* best_target{ };
	int best_target_index{ };
	vec3_t best_target_origin{ };
	int best_record{ };

	for (int i = 1; i <= interfaces::globals->max_clients; i++) 
	{
		auto entity = reinterpret_cast<BaseEntity*>(interfaces::entity_list->get_client_entity(i));
		if (!entity || entity == g::pLocalPlayer || entity->Dormant() || !entity->IsAlive()
			|| entity->TeamNum() == g::pLocalPlayer->TeamNum())
			continue;

		auto origin = entity->AbsOrigin();
		auto angle = math::calculate_angle(local_eye_pos, origin, cmd->viewangles);
		auto fov = std::hypotf(angle.x, angle.y);

		if (fov < best_fov)
		{
			best_fov = fov;
			best_target = entity;
			best_target_index = i;
			best_target_origin = origin;
		}
	}

	if (best_target) 
	{
		if (records[best_target_index].size() <= 3)
			return;

		best_fov = 255.f;

		for (size_t i = 0; i < records[best_target_index].size(); i++) 
		{
			auto& record = records[best_target_index][i];
			if (!valid(record.simTime))
				continue;

			auto angle = math::calculate_angle(local_eye_pos, record.origin, cmd->viewangles);
			auto fov = std::hypotf(angle.x, angle.y);

			if (fov < best_fov) 
			{
				best_fov = fov;
				best_record = i;
			}
		}
	}

	if (best_record) 
	{
		auto record = records[best_target_index][best_record];
		best_target->SetAbsOrigin(record.origin);
		cmd->tick_count = time_to_ticks(record.simTime + lerp_time());
	}
}