#include "../features.hpp"

float Penetration::flScaleDamage(BaseEntity* pTarget, float flDamage, float flHeadshotMultipier, float flArmorRatio, int iHitgroup) {
	int   iArmorValue = 0;
	float flHeavyRatio, flBonusRatio, flRatio, flNewDamage;

	static auto bIsArmored = [](BaseEntity* pTarget, int iArmorValue, int iHitgroup) {
		// the player has no armor.
		if (iArmorValue <= 0)
			return false;

		// if the hitgroup is head and the player has a helment, return true.
		// otherwise only return true if the hitgroup is not generic / legs / gear.
		if (iHitgroup == hitgroup_head && pTarget->HasHelmet())
			return true;

		else if (iHitgroup >= hitgroup_chest && iHitgroup <= hitgroup_rightarm)
			return true;

		return false;
	};

	// check if the player has heavy armor, this is only really used in operation stuff.
	const bool  bHasHeavyArmor = pTarget->HasHeavyArmor();

	static auto mp_damage_scale_ct_head = interfaces::console->FindVar(XOR("mp_damage_scale_ct_head"));
	static auto mp_damage_scale_t_head = interfaces::console->FindVar(XOR("mp_damage_scale_t_head"));

	static auto mp_damage_scale_ct_body = interfaces::console->FindVar(XOR("mp_damage_scale_ct_body"));
	static auto mp_damage_scale_t_body = interfaces::console->FindVar(XOR("mp_damage_scale_t_body"));

	float flHeadDamageScale = pTarget->TeamNum() == 2 ? mp_damage_scale_ct_head->GetFloat() : mp_damage_scale_t_head->GetFloat();
	const float flBodyDamageScale = pTarget->TeamNum() == 2 ? mp_damage_scale_ct_body->GetFloat() : mp_damage_scale_t_body->GetFloat();

	// scale damage based on hitgroup.
	switch (iHitgroup)
	{
	case hitgroup_head:
		flDamage *= flHeadshotMultipier * flHeadDamageScale;
		break;
	case hitgroup_chest:
	case hitgroup_leftarm:
	case hitgroup_rightarm:
		flDamage *= flBodyDamageScale;
		break;
	case hitgroup_stomach:
		flDamage *= 1.25f * flBodyDamageScale;
		break;
	case hitgroup_leftleg:
	case hitgroup_rightleg:
		flDamage *= 0.75f * flBodyDamageScale;
		break;
	default:
		break;
	}

	// grab amount of player armor.
	iArmorValue = pTarget->Armor();

	// check if the ent is armored and scale damage based on armor.
	if (bIsArmored(pTarget, iArmorValue, iHitgroup)) {
		flHeavyRatio = 1.f;
		flBonusRatio = 0.5f;
		flRatio = flArmorRatio * 0.5f;

		// player has heavy armor.
		if (bHasHeavyArmor) {
			// calculate ratio values.
			flBonusRatio = 0.33f;
			flRatio = flArmorRatio * 0.25f;
			flHeavyRatio = 0.33f;

			// calculate new damage.
			flNewDamage = (flDamage * flRatio) * 0.85f;
		}

		// no heavy armor, do normal damage calculation.
		else
			flNewDamage = flDamage * flRatio;

		if (((flDamage - flNewDamage) * (flHeavyRatio * flBonusRatio)) > iArmorValue)
			flNewDamage = flDamage - (iArmorValue / flBonusRatio);

		flDamage = flNewDamage;
	}

	return std::floor(flDamage);
}

bool Penetration::bTraceToExit(const vec3_t& vecStart, const vec3_t& vecDirection, vec3_t& vecOut, trace_t* pEnterTrace, trace_t* pExitTrace) {
	static trace_filter_simple filter{};

	float  dist{};
	vec3_t new_end;
	int    contents, first_contents{};

	// max pen distance is 90 units.
	while (dist <= 90.f) {
		// step forward a bit.
		dist += 4.f;

		// set vecOut pos.
		vecOut = vecStart + (vecDirection * dist);

		contents = interfaces::trace_ray->get_point_contents_world( vecOut, MASK_SHOT_HULL | CONTENTS_HITBOX );
		if (!first_contents)
			first_contents = contents;

		if ((contents & MASK_SHOT_HULL) && (!(contents & CONTENTS_HITBOX) || (contents == first_contents)))
			continue;

		// move end pos a bit for tracing.
		new_end = vecOut - (vecDirection * 4.f);

		// do first trace.
		interfaces::trace_ray->trace_ray(ray_t(vecOut, new_end), MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, pExitTrace);

		// note - dex; this is some new stuff added sometime around late 2017 ( 10.31.2017 update? ).
		trace_filter filter;
        filter.skip = pExitTrace->entity;

		ClipTraceToPlayers(new_end, vecStart, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, pExitTrace, -60.f);

		// we hit an ent's hitbox, do another trace.
		if (pExitTrace->startSolid && (pExitTrace->surface.flags & SURF_HITBOX)) {
			filter.skip = pExitTrace->entity;

			interfaces::trace_ray->trace_ray(ray_t(vecOut, vecStart), MASK_SHOT_HULL, (i_trace_filter*)&filter, pExitTrace);

			if (pExitTrace->did_hit() && !pExitTrace->startSolid) {
				vecOut = pExitTrace->end;
				return true;
			}

			continue;
		}

		if (!pExitTrace->did_hit() || pExitTrace->startSolid) {
			if (pEnterTrace->entity->is_breakable_entity()) {
				*pExitTrace = *pEnterTrace;
				pExitTrace->end = vecStart + vecDirection;
				return true;
			}

			continue;
		}

		if ((pExitTrace->surface.flags & SURF_NODRAW)) {
			// note - dex; ok, when this happens the game seems to not ignore world?
			if (pExitTrace->entity->is_breakable_entity() && pEnterTrace->entity->is_breakable_entity()) {
				vecOut = pExitTrace->end;
				return true;
			}

			if (!(pEnterTrace->surface.flags & SURF_NODRAW))
				continue;
		}

		if (pExitTrace->plane.normal.dot(vecDirection) <= 1.f) {
			vecOut -= (vecDirection * (pExitTrace->flFraction * 4.f));
			return true;
		}
	}

	return false;
}

void Penetration::ClipTraceToPlayer(const vec3_t& vecStart, const vec3_t& vecEnd, uint32_t uiMask, trace_t* pTrace, BaseEntity* pTarget, const float flMinRange) {
	const ICollideable* pCollideable = pTarget->GetCollideable();

	if (pCollideable == nullptr)
		return;

	vec3_t     pos, to, dir, on_ray;
	float      len, range_along, range;
	ray_t        ray;
	trace_t new_trace;

	// reference: https://github.com/alliedmodders/hl2sdk/blob/3957adff10fe20d38a62fa8c018340bf2618742b/game/shared/util_shared.h#L381

	// set some local vars.
	pos = pTarget->Origin() + ((pCollideable->OBBMins() + pCollideable->OBBMaxs()) * 0.5f);
	to = pos - vecStart;
	dir = vecStart - vecEnd;
	len = dir.normalize();
	range_along = dir.dot(to);

	// off vecStart point.
	if (range_along < 0.f)
		range = -(to).length();

	// off end point.
	else if (range_along > len)
		range = -(pos - vecEnd).length();

	// within ray bounds.
	else {
		on_ray = vecStart + (dir * range_along);
		range = (pos - on_ray).length();
	}

	if ( /*min <= range &&*/ range <= 60.f) {
		// clip to player.
		interfaces::trace_ray->clip_ray_to_entity(ray_t(vecStart, vecEnd), uiMask, pTarget, &new_trace);

		if (pTrace->flFraction > new_trace.flFraction)
			*pTrace = new_trace;
	}
}

void Penetration::ClipTraceToPlayers(const vec3_t& vecAbsStart, const vec3_t& vecAbsEnd, const unsigned int fMask, trace_filter* pFilter, trace_t* pTrace, const float flMinRange)
{
	vec3_t     pos, to, dir, on_ray;
	float      len, range_along, range;
	ray_t        ray;
	trace_t new_trace;

	// reference: https://github.com/alliedmodders/hl2sdk/blob/3957adff10fe20d38a62fa8c018340bf2618742b/game/shared/util_shared.h#L381
	for (int i = 1; i <= interfaces::globals->max_clients; i++)
	{
		BaseEntity* pTarget = reinterpret_cast<BaseEntity*>(interfaces::entity_list->get_client_entity(i));

		if ( !pTarget || !pTarget->IsAlive() || pTarget->Dormant() || !pTarget->IsPlayer( ) || pTarget->TeamNum( ) == g::pLocalPlayer->TeamNum( ) )
			continue;

		if ( pFilter && !pFilter->ShouldHitEntity( pTarget, fMask ) )
			continue;

		const ICollideable* pCollideable = pTarget->GetCollideable();

		// set some local vars.
		pos = pTarget->Origin() + ((pCollideable->OBBMins() + pCollideable->OBBMaxs()) * 0.5f);
		to = pos - vecAbsStart;
		dir = vecAbsStart - vecAbsEnd;
		len = dir.normalize();
		range_along = dir.dot(to);

		// off vecAbsStart point.
		if (range_along < 0.f)
			range = -(to).length();

		// off end point.
		else if (range_along > len)
			range = -(pos - vecAbsEnd).length();

		// within ray bounds.
		else {
			on_ray = vecAbsStart + (dir * range_along);
			range = (pos - on_ray).length();
		}

		if ( range > 0.0f && range <= 60.f) {
			// clip to player.
			interfaces::trace_ray->clip_ray_to_entity(ray_t(vecAbsStart, vecAbsEnd), fMask | CONTENTS_HITBOX, pTarget, &new_trace);

			if ( new_trace.flFraction < pTrace->flFraction )
				*pTrace = new_trace;
		}
	}
}

bool Penetration::bRunPenetration(PenetrationInput_t* penInput, PenetrationOutput_t* penOutput) {
	int			  pen{ 4 }, enter_material, exit_material;
	float		  damage, penetration, penetration_mod, player_damage, remaining, trace_len{}, total_pen_mod, damage_mod, modifier, damage_lost;
	surfacedata_t* enter_surface, * exit_surface;
	bool		  nodraw, grate;
	vec3_t		  start, dir, end, pen_end;
	trace_t	  trace, exit_trace;
	BaseWeapon* weapon;
	CWeaponData* weapon_info;

	// if we are tracing from our local player perspective.
	if (penInput->pFrom == g::pLocalPlayer) {
		weapon = g::pLocalWeapon;
		weapon_info = g::pLocalWeaponData;
		if (penInput->vecFrom != vec3_t(0, 0, 0))
			start = g::pLocalPlayer->GetEyePosition(true);
		else
			start = penInput->vecFrom;
	}

	// not local player.
	else {
		weapon = penInput->pFrom->ActiveWeapon();
		if (!weapon)
			return false;

		// get weapon info.
		weapon_info = weapon->GetWpnData();
		if (!weapon_info)
			return false;

		// set trace start.
		start = penInput->pFrom->GetEyePosition(false);
	}

	// get some weapon data.
	damage = (float)weapon_info->weapon_damage;
	penetration = weapon_info->weapon_penetration;

	// used later in calculations.
	penetration_mod = std::max(0.f, (3.f / penetration) * 1.25f);

	// get direction to end point.
	dir = (penInput->vecPos - start).normalized();

	// setup trace filter for later.
	trace_filter filter;
	filter.skip = penInput->pFrom;

	while (damage > 0.f) {
		// calculating remaining len.
		remaining = weapon_info->weapon_range - trace_len;

		// end position of bullet
		end = start + dir * remaining;

		interfaces::trace_ray->trace_ray(ray_t(start, end), MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

		// we didn't hit anything, stop tracing shoot
		if (trace.flFraction == 1.0f)
			break;

		// check for player hitboxes extending outside their collision bounds
		if (penInput->pTarget != nullptr)
			ClipTraceToPlayer(start, end + (dir * 40.f), MASK_SHOT_HULL | CONTENTS_HITBOX, &trace, penInput->pTarget, -60.f);
		else
			ClipTraceToPlayers(start, end + (dir * 40.f), MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace, -60.f);

		// calculate the damage based on the distance the bullet traveled
		trace_len += trace.flFraction * remaining;
		damage *= std::powf(weapon_info->weapon_range_mod, trace_len / MAX_DAMAGE);

		// if a target was passed.
		if (penInput->pTarget != nullptr) {
			// validate that we hit the target we aimed for.
			if (trace.entity != nullptr) {
				if (/*trace.entity == penInput->pTarget &&*/ penInput->pFrom->Enemy(trace.entity) && ((trace.hitGroup >= hitgroup_head && trace.hitGroup <= hitgroup_rightleg) || trace.hitGroup == hitgroup_gear)) {
					int group = (weapon->ItemDefinitionIndex() == WEAPON_TASER) ? hitgroup_generic : trace.hitGroup;

					// scale damage based on the hitgroup we hit.
					player_damage = flScaleDamage(penInput->pTarget, damage, weapon_info->weapon_headshot_multipier, weapon_info->weapon_armor_ratio, group);

					// set result data for when we hit a player.
					penOutput->bPenetrated = pen != 4;
					penOutput->iHitgroup = group;
					penOutput->flDamage = player_damage;
					penOutput->pTarget = penInput->pTarget;

					// non-penetrate damage.
					if (pen == 4)
						return player_damage >= penInput->flDamage;

					// penetration damage.
					return player_damage >= penInput->flDamagePen;
				}
			}
		}

		// no target was passed, check for any player hit or just get final damage done.
		else {
			penOutput->bPenetrated = pen != 4;

			if (trace.entity != nullptr) {
				if (/*trace.entity == penInput->pTarget &&*/ penInput->pFrom->Enemy(trace.entity) && ((trace.hitGroup >= hitgroup_head && trace.hitGroup <= hitgroup_rightleg) || trace.hitGroup == hitgroup_gear)) {
					int group = (weapon->ItemDefinitionIndex() == WEAPON_TASER) ? hitgroup_generic : trace.hitGroup;

					// scale damage based on the hitgroup we hit.
					player_damage = flScaleDamage(penInput->pTarget, damage, weapon_info->weapon_headshot_multipier, weapon_info->weapon_armor_ratio, group);

					// set result data for when we hit a player.
					penOutput->bPenetrated = pen != 4;
					penOutput->iHitgroup = group;
					penOutput->flDamage = player_damage;
					penOutput->pTarget = penInput->pTarget;

					// non-penetrate damage.
					if (pen == 4)
						return player_damage >= penInput->flDamage;

					// penetration damage.
					return player_damage >= penInput->flDamagePen;
				}
			}

			// if we've reached here then we didn't hit a player yet, set damage and hitgroup.
			penOutput->flDamage = damage;
		}

		// don't run pen code if it's not wanted.
		if (!penInput->bCanPenetrate)
			return false;

		enter_surface = interfaces::surface_props->GetSurfaceData(trace.surface.surfaceProps);

		// this happens when we're too far away from a surface and can penetrate walls or the surface's pen modifier is too low.
		if ( enter_surface->game.flPenetrationModifier < 0.1f )
			pen = 0;

		// store data about surface flags / contents.
		nodraw = (trace.surface.flags & SURF_NODRAW);
		grate = (trace.contents & CONTENTS_GRATE);

		// get material at entry point.
		enter_material = enter_surface->game.hMaterial;

		// note - dex; some extra stuff the game does.
		if (!pen && !nodraw && !grate && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS)
			return false;

		// no more pen.
		if (penetration <= 0.f || pen <= 0)
			return false;

		// try to penetrate object.
		if (!bTraceToExit(trace.end, dir, pen_end, &trace, &exit_trace)) {
			if (!(interfaces::trace_ray->get_point_contents_world(pen_end, MASK_SHOT_HULL) & MASK_SHOT_HULL))
				return false;
		}

		// get surface / material at exit point.
		exit_surface = interfaces::surface_props->GetSurfaceData(exit_trace.surface.surfaceProps);
		exit_material = exit_surface->game.hMaterial;

		// todo - dex; check for CHAR_TEX_FLESH and ff_damage_bullet_penetration / ff_damage_reduction_bullets convars?
		//             also need to check !isbasecombatweapon too.
		if (enter_material == CHAR_TEX_GRATE || enter_material == CHAR_TEX_GLASS) {
			total_pen_mod = 3.f;
			damage_mod = 0.05f;
		}

		else if (nodraw || grate) {
			total_pen_mod = 1.f;
			damage_mod = 0.16f;
		}

		else {
			total_pen_mod = (enter_surface->game.flPenetrationModifier + exit_surface->game.flPenetrationModifier) * 0.5f;
			damage_mod = 0.16f;
		}

		// thin metals, wood and plastic get a penetration bonus.
		if (enter_material == exit_material) {
			if (exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD)
				total_pen_mod = 3.f;

			else if (exit_material == CHAR_TEX_PLASTIC)
				total_pen_mod = 2.f;
		}

		// set some local vars.
		trace_len = (exit_trace.end - trace.end).length_sqr();
		modifier = fmaxf(1.f / total_pen_mod, 0.f);

		// this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
		damage_lost = fmaxf(
			((modifier * trace_len) / 24.f)
			+ ((damage * damage_mod)
				+ (fmaxf(3.75 / penetration, 0.f) * 3.f * modifier)), 0.f);

		// subtract from damage.
		damage -= std::max(0.f, damage_lost);
		if (damage < 1.f)
			return false;

		// set new start pos for successive trace.
		start = exit_trace.end;

		// decrement pen.
		--pen;
	}

	return false;
}