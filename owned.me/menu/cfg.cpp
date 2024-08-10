#include "cfg.hpp"
#include "../client/json/json.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "menu.h"
#include "../hack/features/features.hpp"

ConfigSystem config;

void ConfigSystem::SetupValues()
{
	skins::get().SetupValues();

	config.pushitem(&config.backtrack_enable, "legitbot", "backtrack_enable", config.backtrack_enable);

	for (int i = 0; i < 9; i++)
	{
		config.pushitem(&config.triggersettings[i].burstTime, "triggerbot", "burstTime" + std::to_string(i), config.triggersettings[i].burstTime);
		config.pushitem(&config.triggersettings[i].trigger_enable, "triggerbot", "trigger_enable" + std::to_string(i), config.triggersettings[i].trigger_enable);
		config.pushitem(&config.triggersettings[i].visibleOnly, "triggerbot", "visibleOnly" + std::to_string(i), config.triggersettings[i].visibleOnly);
		config.pushitem(&config.triggersettings[i].scopedOnly, "triggerbot", "scopedOnly" + std::to_string(i), config.triggersettings[i].scopedOnly);
		config.pushitem(&config.triggersettings[i].ignoreFlash, "triggerbot", "ignoreFlash" + std::to_string(i), config.triggersettings[i].ignoreFlash);
		config.pushitem(&config.triggersettings[i].ignoreSmoke, "triggerbot", "ignoreSmoke" + std::to_string(i), config.triggersettings[i].ignoreSmoke);
		config.pushitem(&config.triggersettings[i].trigger_key, "triggerbot", "trigger_key" + std::to_string(i), config.triggersettings[i].trigger_key);
		config.pushitem(&config.triggersettings[i].trigger_key_style, "triggerbot", "trigger_key_style" + std::to_string(i), config.triggersettings[i].trigger_key_style);
		config.pushitem(&config.triggersettings[i].shotDelay, "triggerbot", "shotDelay" + std::to_string(i), config.triggersettings[i].shotDelay);
		config.pushitem(&config.triggersettings[i].distance, "triggerbot", "distance" + std::to_string(i), config.triggersettings[i].distance);
		config.pushitem(&config.triggersettings[i].minDamage, "triggerbot", "minDamage" + std::to_string(i), config.triggersettings[i].minDamage);
		config.pushitem(&config.triggersettings[i].minDamageAutoWall, "triggerbot", "minDamageAutoWall" + std::to_string(i), config.triggersettings[i].minDamageAutoWall);

		for (int j = 0; j < 7; j++)
		{
			config.pushitem(&config.triggersettings[i].hitgroup[j], "triggerbot", "hitgroup" + std::to_string(i) + std::to_string(j), config.triggersettings[i].hitgroup[j]);
		}

		//legitbot

		config.pushitem(&config.legitsettings[i].legit_enable, "legitbot", "legit_enable" + std::to_string(i), config.legitsettings[i].legit_enable);
		config.pushitem(&config.legitsettings[i].legit_silent, "legitbot", "legit_silent" + std::to_string(i), config.legitsettings[i].legit_silent);
		config.pushitem(&config.legitsettings[i].legit_key, "legitbot", "legit_key" + std::to_string(i), config.legitsettings[i].legit_key);
		config.pushitem(&config.legitsettings[i].legit_key_style, "legitbot", "legit_key_style" + std::to_string(i), config.legitsettings[i].legit_key_style);
		config.pushitem(&config.legitsettings[i].kill_delay, "legitbot", "legit_key_style" + std::to_string(i), config.legitsettings[i].kill_delay);
		config.pushitem(&config.legitsettings[i].fov, "legitbot", "fov" + std::to_string(i), config.legitsettings[i].fov);
		config.pushitem(&config.legitsettings[i].rcs, "legitbot", "rcs" + std::to_string(i), config.legitsettings[i].rcs);
		config.pushitem(&config.legitsettings[i].smooth, "legitbot", "smooth" + std::to_string(i), 1.0f);
		config.pushitem(&config.legitsettings[i].wall_dmg, "legitbot", "wall_dmg" + std::to_string(i), config.legitsettings[i].wall_dmg);

		for (int s = 0; s < 6; s++)
		{
			config.pushitem(&config.legitsettings[i].hitboxes[s], "legitbot", "hitboxes" + std::to_string(i) + std::to_string(s), config.legitsettings[i].hitboxes[s]);
			config.pushitem(&config.legitsettings[i].checks[s], "legitbot", "checks" + std::to_string(i) + std::to_string(s), config.legitsettings[i].checks[s]);
		}
		//ragebot
		config.pushitem(&config.ragesettings[i].ragebot_enable, "ragebot", "ragebot_enable" + std::to_string(i), config.ragesettings[i].ragebot_enable);
		config.pushitem(&config.ragesettings[i].autoshoot_enable, "ragebot", "autoshoot_enable" + std::to_string(i), config.ragesettings[i].autoshoot_enable);
		config.pushitem(&config.ragesettings[i].autostop_enable, "ragebot", "autostop_enable" + std::to_string(i), config.ragesettings[i].autostop_enable);
		config.pushitem(&config.ragesettings[i].autoscope_enable, "ragebot", "autoscope_enable" + std::to_string(i), config.ragesettings[i].autoscope_enable);
		config.pushitem(&config.ragesettings[i].adaptive_scale, "ragebot", "adaptive_scale" + std::to_string(i), config.ragesettings[i].adaptive_scale);
		config.pushitem(&config.ragesettings[i].hitchance_amount, "ragebot", "hitchance_amount" + std::to_string(i), config.ragesettings[i].hitchance_amount);
		config.pushitem(&config.ragesettings[i].min_dmg, "ragebot", "min_dmg" + std::to_string(i), config.ragesettings[i].min_dmg);
		config.pushitem(&config.ragesettings[i].min_dmg_awall, "ragebot", "min_dmg_awall" + std::to_string(i), config.ragesettings[i].min_dmg_awall);
		config.pushitem(&config.ragesettings[i].min_dmg_override, "ragebot", "min_dmg_override" + std::to_string(i), config.ragesettings[i].min_dmg_override);
		config.pushitem(&config.ragesettings[i].mp_scale, "ragebot", "mp_scale" + std::to_string(i), config.ragesettings[i].mp_scale);
		config.pushitem(&config.ragesettings[i].baim_key, "ragebot", "baim_key" + std::to_string(i), config.ragesettings[i].baim_key);
		config.pushitem(&config.ragesettings[i].baim_key_style, "ragebot", "baim_key_style" + std::to_string(i), config.ragesettings[i].baim_key_style);
		config.pushitem(&config.ragesettings[i].min_dmg_key, "ragebot", "min_dmg_key" + std::to_string(i), config.ragesettings[i].min_dmg_key);
		config.pushitem(&config.ragesettings[i].min_dmg_style, "ragebot", "min_dmg_style" + std::to_string(i), config.ragesettings[i].min_dmg_style);
		config.pushitem(&config.ragesettings[i].hitbox_prefer, "ragebot", "hitbox_prefer" + std::to_string(i), config.ragesettings[i].hitbox_prefer);
		config.pushitem(&config.ragesettings[i].autostop_type[0], "ragebot", "autostop_type_0" + std::to_string(i), config.ragesettings[i].autostop_type[0]);
		config.pushitem(&config.ragesettings[i].autostop_type[1], "ragebot", "autostop_type_1" + std::to_string(i), config.ragesettings[i].autostop_type[1]);
		config.pushitem(&config.ragesettings[i].force_safepoints, "ragebot", "force_safepoints" + std::to_string(i), config.ragesettings[i].force_safepoints);

		for (int s = 0; s < 9; s++)
		{
			config.pushitem(&config.ragesettings[i].hitboxes[s], "ragebot", "hitboxes" + std::to_string(i) + std::to_string(s), config.ragesettings[i].hitboxes[s]);
		}

		for (int s = 0; s < 3; s++)
		{
			config.pushitem(&config.ragesettings[i].head_prefers[s], "ragebot", "head_prefers" + std::to_string(i) + std::to_string(s), config.ragesettings[i].head_prefers[s]);
		}

		for (int s = 0; s < 4; s++)
		{
			config.pushitem(&config.ragesettings[i].body_prefers[s], "ragebot", "body_prefers" + std::to_string(i) + std::to_string(s), config.ragesettings[i].body_prefers[s]);
		}

		//for (int s = 0; s < 4; s++)
		//{
		//	config.pushitem(&config.ragesettings[i].head_prefers[s], "ragebot", "head_prefers" + std::to_string(i) + std::to_string(s), config.ragesettings[i].head_prefers[s]);
		//	config.pushitem(&config.ragesettings[i].body_prefers[s], "ragebot", "body_prefers" + std::to_string(i) + std::to_string(s), config.ragesettings[i].body_prefers[s]);
		//}
	}

	config.pushitem(&config.doubletap_key, "rage", "doubletap_key", config.doubletap_key);
	config.pushitem(&config.doubletap_key_style, "rage", "doubletap_key_style", config.doubletap_key_style);

	config.pushitem(&config.hideshot_key, "rage", "hideshot_key", config.hideshot_key);
	config.pushitem(&config.hideshot_key_style, "rage", "hideshot_key_style", config.hideshot_key_style);

	config.pushitem(&config.knifebot, "rage", "enable_knifebot", config.knifebot);
	config.pushitem(&config.zeusbot, "rage", "enable_zeusbot", config.zeusbot);
	config.pushitem(&config.auto_revolver, "rage", "enable_auto_revolver", config.auto_revolver);
	config.pushitem(&config.zeus_hc, "rage", "enable_zeus_hc", config.zeus_hc);

	//aa
	config.pushitem(&config.slowwalk, "misc", "slowwalk", config.slowwalk);
	config.pushitem(&config.slowwalk_key, "misc", "slowwalk_key", config.slowwalk_key);
	config.pushitem(&config.slowwalk_key_style, "misc", "slowwalk_key_style", config.slowwalk_key_style);
	config.pushitem(&config.autopeek, "misc", "autopeek", config.autopeek);
	config.pushitem(&config.autopeek_key, "misc", "autopeek_key", config.autopeek_key);
	config.pushitem(&config.autopeek_key_style, "misc", "autopeek_key_style", config.autopeek_key_style);
	config.pushitem(&config.fakeduck, "misc", "fakeduck", config.fakeduck);
	config.pushitem(&config.fakeduck_key, "misc", "fakeduck_key", config.fakeduck_key);
	config.pushitem(&config.fakeduck_key_style, "misc", "fakeduck_key_style", config.fakeduck_key_style);
	config.pushitem(&config.thirdperson, "misc", "thirdperson_enable", config.thirdperson);
	config.pushitem(&config.thirdperson_key, "misc", "thirdperson", config.thirdperson_key);
	config.pushitem(&config.thirdperson_key_style, "misc", "thirdperson_style", config.thirdperson_key_style);
	config.pushitem(&config.thirdperson_distance, "misc", "thirdperson_distance", config.thirdperson_distance);

	config.pushitem(&config.enable_fakelag, "rage", "fakelag", config.enable_fakelag);
	config.pushitem(&config.limit_fakelag, "rage", "fakelag_limit", config.limit_fakelag);
	config.pushitem(&config.fakelag_variance, "rage", "fakelag_variance", config.fakelag_variance);
	config.pushitem(&config.fakelag_type, "rage", "fakelag_type", config.fakelag_type);

	config.pushitem(&config.enable_antiaim, "rage", "enable_antiaim", config.enable_antiaim);
	config.pushitem(&config.extended_ang, "rage", "extended_ang", config.extended_ang);
	config.pushitem(&config.extended_ang_enable, "rage", "extended_ang_enable", config.extended_ang_enable);
	config.pushitem(&config.desync_type, "rage", "desync_type", config.desync_type);
	config.pushitem(&config.pitch_type, "rage", "pitch_type", config.pitch_type);
	config.pushitem(&config.yaw_type, "rage", "desync_type", config.yaw_type);
	config.pushitem(&config.jitter_range, "rage", "jitter_range", config.jitter_range);
	config.pushitem(&config.base_angle, "rage", "base_angle", config.base_angle);
	config.pushitem(&config.desync_range, "rage", "desync_range", config.desync_range);
	config.pushitem(&config.inverted_desync_range, "rage", "inverted_desync_range", config.inverted_desync_range);
	config.pushitem(&config.desync_key, "rage", "desync_key", config.desync_key);
	config.pushitem(&config.desync_key_style, "rage", "desync_key_style", config.desync_key_style);
	config.pushitem( &config.backside, "rage", "backside", config.backside );
	config.pushitem( &config.backside_style, "rage", "backside_style", config.backside_style );
	config.pushitem(&config.leftside, "rage", "leftside", config.leftside);
	config.pushitem(&config.leftside_style, "rage", "leftside_style", config.leftside_style);
	config.pushitem(&config.rightside, "rage", "rightside", config.rightside);
	config.pushitem(&config.rightside_style, "rage", "rightside_style", config.rightside_style);

	config.pushitem(&config.box, "visual", "box", config.box);
	config.pushitem(&config.name, "visual", "name", config.name);
	config.pushitem(&config.health, "visual", "health", config.health);
	config.pushitem(&config.ammo, "visual", "ammo", config.ammo);
	config.pushitem(&config.weapon, "visual", "weapon", config.weapon);
	config.pushitem(&config.weapon_icon, "visual", "weapon_icon", config.weapon_icon);
	config.pushitem(&config.flags, "visual", "flags", config.flags);
	config.pushitem(&config.playerflags[0], "visual", "flags_0", config.playerflags[0]);
	config.pushitem(&config.playerflags[1], "visual", "flags_1", config.playerflags[1]);
	config.pushitem(&config.playerflags[2], "visual", "flags_2", config.playerflags[2]);
	config.pushitem(&config.playerflags[3], "visual", "flags_3", config.playerflags[3]);
	config.pushitem(&config.playerflags[4], "visual", "flags_4", config.playerflags[4]);
	config.pushitem(&config.playerflags[5], "visual", "flags_5", config.playerflags[5]);
	config.pushitem(&config.glow, "visual", "glow", config.glow);
	config.pushitem(&config.dormant, "visual", "dormant", config.dormant);
	config.pushitem(&config.dormancy, "visual", "dormancy", config.dormancy);
	config.pushitem(&config.nightmode, "visual", "nightmode", config.nightmode);
	config.pushitem(&config.recoilcross, "visual", "recoilcrosshair", config.recoilcross);
	config.pushitem(&config.indicators, "visual", "indicators", config.indicators);
	config.pushitem(&config.brightness, "visual", "brightness", config.brightness);

	config.pushitem(&config.dropped, "visual", "dropped", config.dropped);
	config.pushitem(&config.dropped_ammo, "visual", "dropped_ammo", config.dropped_ammo);
	config.pushitem(&config.bomb, "visual", "bomb", config.bomb);
	config.pushitem(&config.nades, "visual", "nades", config.nades);
	config.pushitem(&config.nadepred, "visual", "nadepred", config.nadepred);
	config.pushitem(&config.viewmodel_fov, "visual", "viewmodel_fov", config.viewmodel_fov);
	config.pushitem(&config.view_fov, "visual", "view_fov", config.view_fov);
	config.pushitem(&config.viewmodel_fov, "visual", "viewmodel_fov", config.disablepostprocess);
	config.pushitem(&config.enable_killfeed, "visual", "noscope", config.enable_killfeed);
	config.pushitem(&config.noscope, "visual", "noscope", config.noscope);
	config.pushitem(&config.noscopezoom, "visual", "noscopezoom", config.noscopezoom);
	config.pushitem(&config.noflash, "visual", "noflash", config.noflash);
	config.pushitem(&config.nosmoke, "visual", "nosmoke", config.nosmoke);
	config.pushitem(&config.radar, "visual", "radar", config.radar);
	config.pushitem(&config.novisrecoil, "visual", "novisrecoil", config.novisrecoil);

	config.pushitem(&config.changemolotov, "visual", "customfire", config.changemolotov);
	config.pushitem(&config.molotov_col[0], "visual", "molotov_col_r", config.molotov_col[0]);
	config.pushitem(&config.molotov_col[1], "visual", "molotov_col_g", config.molotov_col[1]);
	config.pushitem(&config.molotov_col[2], "visual", "molotov_col_b", config.molotov_col[2]);
	config.pushitem(&config.molotov_col[3], "visual", "molotov_col_a", config.molotov_col[3]);

	config.pushitem(&config.droppedweapons[0], "visual", "droppedweapons_0", config.droppedweapons[0]);
	config.pushitem(&config.droppedweapons[1], "visual", "droppedweapons_1", config.droppedweapons[1]);

	config.pushitem(&config.nades_col[0], "visual", "nades_col_r", config.nades_col[0]);
	config.pushitem(&config.nades_col[1], "visual", "nades_col_g", config.nades_col[1]);
	config.pushitem(&config.nades_col[2], "visual", "nades_col_b", config.nades_col[2]);
	config.pushitem(&config.nades_col[3], "visual", "nades_col_a", config.nades_col[3]);
	config.pushitem(&config.nadepred_col[0], "visual", "nadepred_col_r", config.nadepred_col[0]);
	config.pushitem(&config.nadepred_col[1], "visual", "nadepred_col_g", config.nadepred_col[1]);
	config.pushitem(&config.nadepred_col[2], "visual", "nadepred_col_b", config.nadepred_col[2]);
	config.pushitem(&config.nadepred_col[3], "visual", "nadepred_col_a", config.nadepred_col[3]);

	config.pushitem(&config.weapon_col[0], "visual", "weapon_col_r", config.weapon_col[0]);
	config.pushitem(&config.weapon_col[1], "visual", "weapon_col_g", config.weapon_col[1]);
	config.pushitem(&config.weapon_col[2], "visual", "weapon_col_b", config.weapon_col[2]);
	config.pushitem(&config.weapon_col[3], "visual", "weapon_col_a", config.weapon_col[3]);


	config.pushitem(&config.weapon_icon_col[0], "visual", "weapon_icon_col_r", config.weapon_icon_col[0]);
	config.pushitem(&config.weapon_icon_col[1], "visual", "weapon_icon_col_g", config.weapon_icon_col[1]);
	config.pushitem(&config.weapon_icon_col[2], "visual", "weapon_icon_col_b", config.weapon_icon_col[2]);
	config.pushitem(&config.weapon_icon_col[3], "visual", "weapon_icon_col_a", config.weapon_icon_col[3]);

	config.pushitem(&config.dropped_col[0], "visual", "dropped_col_r", config.dropped_col[0]);
	config.pushitem(&config.dropped_col[1], "visual", "dropped_col_g", config.dropped_col[1]);
	config.pushitem(&config.dropped_col[2], "visual", "dropped_col_b", config.dropped_col[2]);
	config.pushitem(&config.dropped_col[3], "visual", "dropped_col_a", config.dropped_col[3]);


	config.pushitem(&config.dropped_ammo_col[0], "visual", "dropped_ammo_col_r", config.dropped_ammo_col[0]);
	config.pushitem(&config.dropped_ammo_col[1], "visual", "dropped_ammo_col_g", config.dropped_ammo_col[1]);
	config.pushitem(&config.dropped_ammo_col[2], "visual", "dropped_ammo_col_b", config.dropped_ammo_col[2]);
	config.pushitem(&config.dropped_ammo_col[3], "visual", "dropped_ammo_col_a", config.dropped_ammo_col[3]);

	config.pushitem(&config.box_col[0], "visual", "box_col_r", config.box_col[0]);
	config.pushitem(&config.box_col[1], "visual", "box_col_g", config.box_col[1]);
	config.pushitem(&config.box_col[2], "visual", "box_col_b", config.box_col[2]);
	config.pushitem(&config.box_col[3], "visual", "box_col_a", config.box_col[3]);

	config.pushitem(&config.name_col[0], "visual", "name_col_r", config.name_col[0]);
	config.pushitem(&config.name_col[1], "visual", "name_col_g", config.name_col[1]);
	config.pushitem(&config.name_col[2], "visual", "name_col_b", config.name_col[2]);
	config.pushitem(&config.name_col[3], "visual", "name_col_a", config.name_col[3]);

	config.pushitem(&config.ammo_col[0], "visual", "ammo_col_r", config.ammo_col[0]);
	config.pushitem(&config.ammo_col[1], "visual", "ammo_col_g", config.ammo_col[1]);
	config.pushitem(&config.ammo_col[2], "visual", "ammo_col_b", config.ammo_col[2]);
	config.pushitem(&config.ammo_col[3], "visual", "ammo_col_a", config.ammo_col[3]);

	config.pushitem(&config.glow_col[0], "visual", "glow_col_r", config.glow_col[0]);
	config.pushitem(&config.glow_col[1], "visual", "glow_col_g", config.glow_col[1]);
	config.pushitem(&config.glow_col[2], "visual", "glow_col_b", config.glow_col[2]);
	config.pushitem(&config.glow_col[3], "visual", "glow_col_a", config.glow_col[3]);


	config.pushitem(&config.players.visible, "players", "players_visible", config.players.visible);
	config.pushitem(&config.players.invisible, "players", "players_invisible", config.players.invisible);
	config.pushitem(&config.players.pearlescent, "players", "players_pearlescent", config.players.pearlescent);
	config.pushitem(&config.players.phong, "players", "players_phong", config.players.phong);
	config.pushitem(&config.players.reflectivity, "players", "players_reflectivity", config.players.reflectivity);
	config.pushitem(&config.players.basemat, "players", "players_basemat", config.players.basemat);

	config.pushitem(&config.players.overlaymat, "players", "players_overlaymat", config.players.overlaymat);
	config.pushitem(&config.players.wireframe, "players", "players_wireframe", config.players.wireframe);
	config.pushitem(&config.players.overlaywireframe, "players", "players_overlaywireframe", config.players.overlaywireframe);
	config.pushitem(&config.players.overlay_col[0], "players", "players_overlay_col_r", config.players.overlay_col[0]);
	config.pushitem(&config.players.overlay_col[1], "players", "players_overlay_col_g", config.players.overlay_col[1]);
	config.pushitem(&config.players.overlay_col[2], "players", "players_overlay_col_b", config.players.overlay_col[2]);
	config.pushitem(&config.players.overlay_col[3], "players", "players_overlay_col_a", config.players.overlay_col[3]);

	config.pushitem(&config.players.overlayxqz, "players", "players_overlayxqz", config.players.overlayxqz);
	config.pushitem(&config.players.overlay_xqz_col[0], "players", "players_overlay_xqz_col_r", config.players.overlay_xqz_col[0]);
	config.pushitem(&config.players.overlay_xqz_col[1], "players", "players_overlay_xqz_col_g", config.players.overlay_xqz_col[1]);
	config.pushitem(&config.players.overlay_xqz_col[2], "players", "players_overlay_xqz_col_b", config.players.overlay_xqz_col[2]);
	config.pushitem(&config.players.overlay_xqz_col[3], "players", "players_overlay_xqz_col_a", config.players.overlay_xqz_col[3]);

	config.pushitem(&config.players.vis_col[0], "players", "players_vis_col_r", config.players.vis_col[0]);
	config.pushitem(&config.players.vis_col[1], "players", "players_vis_col_g", config.players.vis_col[1]);
	config.pushitem(&config.players.vis_col[2], "players", "players_vis_col_b", config.players.vis_col[2]);
	config.pushitem(&config.players.vis_col[3], "players", "players_vis_col_a", config.players.vis_col[3]);

	config.pushitem(&config.players.invis_col[0], "players", "players_invis_col_r", config.players.invis_col[0]);
	config.pushitem(&config.players.invis_col[1], "players", "players_invis_col_g", config.players.invis_col[1]);
	config.pushitem(&config.players.invis_col[2], "players", "players_invis_col_b", config.players.invis_col[2]);
	config.pushitem(&config.players.invis_col[3], "players", "players_invis_col_a", config.players.invis_col[3]);

	config.pushitem(&config.players.phong_col[0], "players", "players_phong_col_r", config.players.phong_col[0]);
	config.pushitem(&config.players.phong_col[1], "players", "players_phong_col_g", config.players.phong_col[1]);
	config.pushitem(&config.players.phong_col[2], "players", "players_phong_col_b", config.players.phong_col[2]);
	config.pushitem(&config.players.phong_col[3], "players", "players_phong_col_a", config.players.phong_col[3]);

	config.pushitem(&config.players.reflectivity_col[0], "players", "players_reflectivity_col_r", config.players.reflectivity_col[0]);
	config.pushitem(&config.players.reflectivity_col[1], "players", "players_reflectivity_col_g", config.players.reflectivity_col[1]);
	config.pushitem(&config.players.reflectivity_col[2], "players", "players_reflectivity_col_b", config.players.reflectivity_col[2]);
	config.pushitem(&config.players.reflectivity_col[3], "players", "players_reflectivity_col_a", config.players.reflectivity_col[3]);



	config.pushitem(&config.backtrack_chams.visible, "backtrack_chams", "backtrack_chams_visible", config.backtrack_chams.visible);
	config.pushitem(&config.backtrack_chams.alltick, "backtrack_chams", "backtrack_chams_alltick", config.backtrack_chams.alltick);
	config.pushitem(&config.backtrack_chams.gradient, "backtrack_chams", "backtrack_chams_gradient", config.backtrack_chams.gradient);
	config.pushitem(&config.backtrack_chams.invisible, "backtrack_chams", "backtrack_chams_invisible", config.backtrack_chams.invisible);
	config.pushitem(&config.backtrack_chams.pearlescent, "backtrack_chams", "backtrack_chams_pearlescent", config.backtrack_chams.pearlescent);
	config.pushitem(&config.backtrack_chams.phong, "backtrack_chams", "backtrack_chams_phong", config.backtrack_chams.phong);
	config.pushitem(&config.backtrack_chams.reflectivity, "backtrack_chams", "backtrack_chams_reflectivity", config.backtrack_chams.reflectivity);
	config.pushitem(&config.backtrack_chams.basemat, "players", "backtrack_chams_basemat", config.backtrack_chams.basemat);

	config.pushitem(&config.backtrack_chams.vis_col[0], "backtrack_chams", "backtrack_chams_vis_col_r", config.backtrack_chams.vis_col[0]);
	config.pushitem(&config.backtrack_chams.vis_col[1], "backtrack_chams", "backtrack_chams_vis_col_g", config.backtrack_chams.vis_col[1]);
	config.pushitem(&config.backtrack_chams.vis_col[2], "backtrack_chams", "backtrack_chams_vis_col_b", config.backtrack_chams.vis_col[2]);
	config.pushitem(&config.backtrack_chams.vis_col[3], "backtrack_chams", "backtrack_chams_vis_col_a", config.backtrack_chams.vis_col[3]);

	config.pushitem(&config.backtrack_chams.invis_col[0], "backtrack_chams", "backtrack_chams_invis_col_r", config.backtrack_chams.invis_col[0]);
	config.pushitem(&config.backtrack_chams.invis_col[1], "backtrack_chams", "backtrack_chams_invis_col_g", config.backtrack_chams.invis_col[1]);
	config.pushitem(&config.backtrack_chams.invis_col[2], "backtrack_chams", "backtrack_chams_invis_col_b", config.backtrack_chams.invis_col[2]);
	config.pushitem(&config.backtrack_chams.invis_col[3], "backtrack_chams", "backtrack_chams_invis_col_a", config.backtrack_chams.invis_col[3]);

	config.pushitem(&config.backtrack_chams.phong_col[0], "backtrack_chams", "backtrack_chams_phong_col_r", config.backtrack_chams.phong_col[0]);
	config.pushitem(&config.backtrack_chams.phong_col[1], "backtrack_chams", "backtrack_chams_phong_col_g", config.backtrack_chams.phong_col[1]);
	config.pushitem(&config.backtrack_chams.phong_col[2], "backtrack_chams", "backtrack_chams_phong_col_b", config.backtrack_chams.phong_col[2]);
	config.pushitem(&config.backtrack_chams.phong_col[3], "backtrack_chams", "backtrack_chams_phong_col_a", config.backtrack_chams.phong_col[3]);

	config.pushitem(&config.backtrack_chams.reflectivity_col[0], "backtrack_chams", "backtrack_chams_reflectivity_col_r", config.backtrack_chams.reflectivity_col[0]);
	config.pushitem(&config.backtrack_chams.reflectivity_col[1], "backtrack_chams", "backtrack_chams_reflectivity_col_g", config.backtrack_chams.reflectivity_col[1]);
	config.pushitem(&config.backtrack_chams.reflectivity_col[2], "backtrack_chams", "backtrack_chams_reflectivity_col_b", config.backtrack_chams.reflectivity_col[2]);
	config.pushitem(&config.backtrack_chams.reflectivity_col[3], "backtrack_chams", "backtrack_chams_reflectivity_col_a", config.backtrack_chams.reflectivity_col[3]);


	config.pushitem(&config.weapons.enabled, "weapons", "weapons_enabled", config.weapons.enabled);
	config.pushitem(&config.weapons.pearlescent, "weapons", "weapons_pearlescent", config.weapons.pearlescent);
	config.pushitem(&config.weapons.phong, "weapons", "weapons_phong", config.weapons.phong);
	config.pushitem(&config.weapons.reflectivity, "weapons", "weapons_reflectivity", config.weapons.reflectivity);
	config.pushitem(&config.weapons.basemat, "weapons", "weapons_basemat", config.weapons.basemat);

	config.pushitem(&config.weapons.overlaymat, "weapons", "weapons_overlaymat", config.weapons.overlaymat);
	config.pushitem(&config.weapons.wireframe, "weapons", "weapons_wireframe", config.weapons.wireframe);
	config.pushitem(&config.weapons.overlaywireframe, "weapons", "weapons_overlaywireframe", config.weapons.overlaywireframe);
	config.pushitem(&config.weapons.overlay_col[0], "weapons", "weapons_overlay_col_r", config.weapons.overlay_col[0]);
	config.pushitem(&config.weapons.overlay_col[1], "weapons", "weapons_overlay_col_g", config.weapons.overlay_col[1]);
	config.pushitem(&config.weapons.overlay_col[2], "weapons", "weapons_overlay_col_b", config.weapons.overlay_col[2]);
	config.pushitem(&config.weapons.overlay_col[3], "weapons", "weapons_overlay_col_a", config.weapons.overlay_col[3]);

	config.pushitem(&config.weapons.col[0], "weapons", "weapons_col_r", config.weapons.col[0]);
	config.pushitem(&config.weapons.col[1], "weapons", "weapons_col_g", config.weapons.col[1]);
	config.pushitem(&config.weapons.col[2], "weapons", "weapons_col_b", config.weapons.col[2]);
	config.pushitem(&config.weapons.col[3], "weapons", "weapons_col_a", config.weapons.col[3]);

	config.pushitem(&config.weapons.phong_col[0], "weapons", "weapons_phong_col_r", config.weapons.phong_col[0]);
	config.pushitem(&config.weapons.phong_col[1], "weapons", "weapons_phong_col_g", config.weapons.phong_col[1]);
	config.pushitem(&config.weapons.phong_col[2], "weapons", "weapons_phong_col_b", config.weapons.phong_col[2]);
	config.pushitem(&config.weapons.phong_col[3], "weapons", "weapons_phong_col_a", config.weapons.phong_col[3]);

	config.pushitem(&config.weapons.reflectivity_col[0], "weapons", "weapons_reflectivity_col_r", config.weapons.reflectivity_col[0]);
	config.pushitem(&config.weapons.reflectivity_col[1], "weapons", "weapons_reflectivity_col_g", config.weapons.reflectivity_col[1]);
	config.pushitem(&config.weapons.reflectivity_col[2], "weapons", "weapons_reflectivity_col_b", config.weapons.reflectivity_col[2]);
	config.pushitem(&config.weapons.reflectivity_col[3], "weapons", "weapons_reflectivity_col_a", config.weapons.reflectivity_col[3]);

	//clantag
	config.pushitem(&config.enable_antiaim, "misc", "ClanTag", config.clantag);
	//clantag

	config.pushitem(&config.hands.enabled, "hands", "hands_enabled", config.hands.enabled);
	config.pushitem(&config.hands.pearlescent, "hands", "hands_pearlescent", config.hands.pearlescent);
	config.pushitem(&config.hands.phong, "hands", "hands_phong", config.hands.phong);
	config.pushitem(&config.hands.reflectivity, "hands", "hands_reflectivity", config.hands.reflectivity);
	config.pushitem(&config.hands.basemat, "hands", "hands_basemat", config.hands.basemat);

	config.pushitem(&config.hands.overlaymat, "hands", "hands_overlaymat", config.hands.overlaymat);
	config.pushitem(&config.hands.wireframe, "hands", "hands_wireframe", config.hands.wireframe);
	config.pushitem(&config.hands.overlaywireframe, "hands", "hands_overlaywireframe", config.hands.overlaywireframe);
	config.pushitem(&config.hands.overlay_col[0], "hands", "hands_overlay_col_r", config.hands.overlay_col[0]);
	config.pushitem(&config.hands.overlay_col[1], "hands", "hands_overlay_col_g", config.hands.overlay_col[1]);
	config.pushitem(&config.hands.overlay_col[2], "hands", "hands_overlay_col_b", config.hands.overlay_col[2]);
	config.pushitem(&config.hands.overlay_col[3], "hands", "hands_overlay_col_a", config.hands.overlay_col[3]);

	config.pushitem(&config.hands.col[0], "hands", "hands_col_r", config.hands.col[0]);
	config.pushitem(&config.hands.col[1], "hands", "hands_col_g", config.hands.col[1]);
	config.pushitem(&config.hands.col[2], "hands", "hands_col_b", config.hands.col[2]);
	config.pushitem(&config.hands.col[3], "hands", "hands_col_a", config.hands.col[3]);

	config.pushitem(&config.hands.phong_col[0], "hands", "hands_phong_col_r", config.hands.phong_col[0]);
	config.pushitem(&config.hands.phong_col[1], "hands", "hands_phong_col_g", config.hands.phong_col[1]);
	config.pushitem(&config.hands.phong_col[2], "hands", "hands_phong_col_b", config.hands.phong_col[2]);
	config.pushitem(&config.hands.phong_col[3], "hands", "hands_phong_col_a", config.hands.phong_col[3]);

	config.pushitem(&config.hands.reflectivity_col[0], "hands", "hands_reflectivity_col_r", config.hands.reflectivity_col[0]);
	config.pushitem(&config.hands.reflectivity_col[1], "hands", "hands_reflectivity_col_g", config.hands.reflectivity_col[1]);
	config.pushitem(&config.hands.reflectivity_col[2], "hands", "hands_reflectivity_col_b", config.hands.reflectivity_col[2]);
	config.pushitem(&config.hands.reflectivity_col[3], "hands", "hands_reflectivity_col_a", config.hands.reflectivity_col[3]);


	config.pushitem(&config.bunny_hop, "misc", "bunny_hop", config.bunny_hop);
	config.pushitem(&config.movement_strafe, "misc", "movement_strafe", config.movement_strafe);
	config.pushitem(&config.rankreveal, "misc", "rankreveal", config.rankreveal);
	config.pushitem(&config.infinite_duck, "misc", "infinite_duck", config.infinite_duck);

	config.pushitem(&config.jumpbug, "misc", "jumpbug", config.jumpbug);
	config.pushitem(&config.jumpbugkey, "misc", "jumpbugkey", config.jumpbugkey);
	config.pushitem(&config.jumpbugkey_style, "misc", "jumpbugkey_style", config.jumpbugkey_style);

	config.pushitem(&config.edgejump, "misc", "edgejump", config.edgejump);
	config.pushitem(&config.edgejumpkey, "misc", "edgejumpkey", config.edgejumpkey);
	config.pushitem(&config.edgejumpkey_style, "misc", "edgejumpkey_style", config.edgejumpkey_style);
	config.pushitem(&config.faststop, "misc", "faststop", config.faststop);

	config.pushitem(&config.menu_watermark, "menu", "menu_watermark", config.menu_watermark);

	config.pushitem(&config.announcer.enabled, "announcer", "enabled", config.announcer.enabled);
	config.pushitem(&config.announcer.lang, "announcer", "lang", config.announcer.lang);
	config.pushitem(&config.announcer.log, "announcer", "log", config.announcer.log);
	config.pushitem(&config.announcer.events[0], "announcer", "event_1", config.announcer.events[0]);
	config.pushitem(&config.announcer.events[1], "announcer", "event_2", config.announcer.events[1]);
	config.pushitem(&config.announcer.events[2], "announcer", "event_3", config.announcer.events[2]);
}

bool loading_config;
void ColorPickerr(const char* name, float* color, bool alpha) {

	ImGuiWindow* window = ui::GetCurrentWindow();
	ImGuiStyle* style = &ui::GetStyle();

	ui::SameLine(0 + window->Size.x - 43 - 7 + 18.f);
	auto alphaSliderFlag = alpha ? ImGuiColorEditFlags_AlphaBar : ImGuiColorEditFlags_NoAlpha;

	ui::ColorEdit4(std::string{ "##" }.append(name).append("Picker").c_str(), color, alphaSliderFlag | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);
}
#define ColorSelect(x1,x2,x3) ui::SameLine(167.f); ColorPickerr(x1, x2, x3);


inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void ConfigSystem::pushitem(bool* pointer, std::string category, std::string name, bool def_value) {
	this->booleans.push_back({ pointer, category, name, def_value });
}
void ConfigSystem::pushitem(int* pointer, std::string category, std::string name, int def_value) {
	this->ints.push_back({ pointer, category, name, def_value });
}
void ConfigSystem::pushitem(float* pointer, std::string category, std::string name, float def_value) {
	this->floats.push_back({ pointer, category, name, def_value });
}
void ConfigSystem::pushitem(Color* pointer, std::string category, std::string name, Color def_value) {
	this->colors.push_back({ pointer, category, name, def_value });
}



using nlohmann::json;
void ConfigSystem::read(std::string path) {
	std::filesystem::create_directory("c:\\owned.me\\");
	path = "c:\\owned.me\\" + path + ".cfg";
	if (!file_exists(path)) return;

	json retData;
	std::ifstream reader(path);

	json data;
	reader >> data;

	reader.close();

	auto sanityChecks = [](json& d, std::string& c, std::string& n) -> bool {
		return d.find(c) != d.end() && d[c].find(n) != d[c].end();
	};

	for (auto& b : booleans)
		if (sanityChecks(data, b.category, b.name))
			*b.pointer = data[b.category][b.name].get<bool>();
	for (auto& i : ints)
		if (sanityChecks(data, i.category, i.name))
			*i.pointer = data[i.category][i.name].get<int>();
	for (auto& f : floats)
		if (sanityChecks(data, f.category, f.name))
			*f.pointer = data[f.category][f.name].get<float>();
	for (auto& c : colors) {
		if (sanityChecks(data, c.category, c.name)) {
			auto elem = data[c.category][c.name];
			c.pointer->r = elem["r"].get<int>();
			c.pointer->g = elem["g"].get<int>();
			c.pointer->b = elem["b"].get<int>();
			c.pointer->a = elem["a"].get<int>();
		}
	}
	for (auto& i : keybinds)
		if (sanityChecks(data, i.category, i.name))
			*i.pointer = data[i.category][i.name].get<int>();

}

void ConfigSystem::save(std::string path) {
	std::filesystem::create_directory("c:\\owned.me\\");
	path = "c:\\owned.me\\" + path + ".cfg";

	json retData;

	auto sanityChecks = [&](std::string category) -> void {
		if (retData.find(category) == retData.end())
			retData[category] = json();
	};

	for (auto& b : booleans) {
		sanityChecks(b.category);
		retData[b.category][b.name] = *b.pointer;
	}
	for (auto& i : ints) {
		sanityChecks(i.category);
		retData[i.category][i.name] = *i.pointer;
	}
	for (auto& f : floats) {
		sanityChecks(f.category);
		retData[f.category][f.name] = *f.pointer;
	}
	for (auto& c : colors) {
		sanityChecks(c.category);
		json d;
		Color clr = *c.pointer;
		d["r"] = clr.r;
		d["g"] = clr.g;
		d["b"] = clr.b;
		d["a"] = clr.a;
		retData[c.category][c.name] = d;
	}
	for (auto& i : keybinds) {
		sanityChecks(i.category);
		retData[i.category][i.name] = *i.pointer;
	}

	std::ofstream reader(path);
	reader.clear();
	reader << std::setw(4) << retData << std::endl;
	reader.close();
}

void ConfigSystem::Refresh() {
	config_files.clear();
	std::filesystem::create_directory("c:\\owned.me\\");

	for (const auto& p : std::filesystem::recursive_directory_iterator("c:\\owned.me\\")) {
		if (!std::filesystem::is_directory(p) &&
			p.path().extension().string() == ".cfg") {
			auto file = p.path().filename().string();
			config_files.push_back(file.substr(0, file.size() - 4));
		}
	}
}

void ConfigSystem::remove(std::string path) {
	std::filesystem::create_directory("c:\\owned.me\\");
	path = "c:\\owned.me\\" + path + ".cfg";

	std::filesystem::remove(path);
	Refresh();
}
float RandomFloat(float min, float max)
{
	static auto random_float = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));

	return random_float(min, max);
}
void ConfigSystem::reset() {
	for (auto& b : booleans)
		*b.pointer = b.default_value;
	for (auto& i : ints)
		*i.pointer = i.default_value;
	for (auto& c : colors)
		*c.pointer = c.default_value;
	for (auto& f : floats)
		*f.pointer = f.default_value;
	for (auto& i : keybinds)
		*i.pointer = i.default_value;

}

static std::string selected_cfg = "";

bool confirm_opened = false;
bool confirmed = false;
bool deleteshit = false;

void confirm_save_xd(bool deleting)
{
	confirm_opened = true;
	deleteshit = deleting;
}

void final_save()
{
	if (deleteshit)
	{
		config.remove(selected_cfg);

		std::stringstream ss;
		ss << "deleted config " << selected_cfg << ".    ";
		eventlog.add(ss.str().c_str(), Color(255, 255, 255, 255));
	}
	else
	{
		config.save(selected_cfg);

		std::stringstream ss;
		ss << "saved config " << selected_cfg << ".    ";
		eventlog.add(ss.str().c_str(), Color(255, 255, 255, 255));
	}


	confirm_opened = false;
	confirmed = true;
}
void no_confirm()
{
	confirm_opened = false;
	confirmed = false;
	deleteshit ? eventlog.add("deleting cancelled.    ", Color(255, 255, 255, 255)) : eventlog.add("saving cancelled.    ", Color(255, 255, 255, 255));

}

void windowshit()
{
	if (!confirm_opened && g::StoredAlpha > 0.f) {
		float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
		g::StoredAlpha = std::clamp(g::StoredAlpha - fc / 255.f, 0.f, 1.f);
	}

	if (confirm_opened && g::StoredAlpha < 1.f) {
		float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
		g::StoredAlpha = std::clamp(g::StoredAlpha + fc / 255.f, 0.f, 1.f);
	}
	if (!confirm_opened && g::StoredAlpha == 0.f)
		return;

	ui::Begin("confirmation", 0, ImVec2(0, 0), 1.f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_Speclist | ImGuiWindowFlags_PlayerlistTab);
	ui::SetWindowSize(ImVec2(240, 145));
	ui::SetCursorPosY(39);
	if (confirm_opened)
		deleteshit ? ui::Text("   are you sure want to delete this config?") : ui::Text("     are you sure want to save this config?");
	ui::SetCursorPosX(25 - 21);
	ui::SetCursorPosY(76 - 15);
	if (confirm_opened && ui::Button(deleteshit ? "delete" : "save", ImVec2(ui::GetWindowSize().x - 58 + 6, 0)))
		final_save();
	ui::SetCursorPosY(116 - 30);
	ui::SetCursorPosX(25 - 21);
	if (confirm_opened && ui::Button("cancel", ImVec2(ui::GetWindowSize().x - 58 + 6, 0)))
		no_confirm();

	ui::End();

}

void ConfigSystem::menu()
{
	Refresh();

	static char cfg_name[26];

	ui::SetCursorPos(ImVec2(137, 31));
	ui::begin_child("presets", ImVec2(195, 322)); {

		ui::SetCursorPosX(15);
		ui::ListBoxHeader("##cfglist", ImVec2(-1, -1 - 50 + 31 - 145 - 20));

		for (auto cfg : config_files)
			if (ui::Selectable(cfg.c_str(), cfg == selected_cfg))
				selected_cfg = cfg;

		ui::ListBoxFooter();
		ui::SetCursorPos(ImVec2(15, ui::GetWindowSize().y - 153 - 20));
		if (ui::InputText("", cfg_name, 26)) selected_cfg = std::string(cfg_name);
		if (std::string(cfg_name) != "")
		{
			ui::SetCursorPosX(4);
			if (ui::Button("create", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {

				if (selected_cfg.empty())
				{
					eventlog.add("failed to create/save config.    ", Color(255, 255, 255, 255));
				}
				else
				{
					std::stringstream ss;
					ss << "created & saved config " << selected_cfg << ".    ";
					eventlog.add(ss.str().c_str(), Color(255, 255, 255, 255));
					save(selected_cfg);
					//skinchanger::get().UpdateSkins();
				}
				Refresh();
			}
		}
		ui::SetCursorPosX(4);
		if (ui::Button("load", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {
			if (selected_cfg.empty())
			{
				eventlog.add("failed to load config.    ", Color(255, 255, 255, 255));
			}
			else {
				loading_config = true;
				std::stringstream ss;
				ss << "loaded config " << selected_cfg << ".    ";
				eventlog.add(ss.str().c_str(), Color(255, 255, 255, 255));

				read(selected_cfg);
				//skinchanger::get().UpdateSkins();
				loading_config = false;
			}
			Refresh();
		}
		ui::SetCursorPosX(4);
		if (ui::Button("save", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {
			if (selected_cfg.empty())
			{
				eventlog.add("failed to save config.    ", Color(255, 255, 255, 255));
			}
			else
			{
				confirm_save_xd(false);

			}
			Refresh();
		}

		windowshit();

		ui::SetCursorPosX(4);
		if (ui::Button("delete", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {
			if (selected_cfg.empty())
			{
				eventlog.add("failed to remove config.    ", Color(255, 255, 255, 255));
			}
			else
			{
				confirm_save_xd(true);

			}
			Refresh();
		}
		ui::SetCursorPosX(4);
		if (ui::Button("reset", ImVec2(ui::GetWindowSize().x - 58 + 6, 0)))
		{
			eventlog.add("successfully resetted menu state.    ", Color(255, 255, 255, 255));
			reset();
			//skinchanger::get().UpdateSkins();
		}


	};
	ui::end_child();
}