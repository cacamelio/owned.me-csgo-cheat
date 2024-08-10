#include "menu.h"
#include "imgui/fonts.h"
#include "imgui/imgui_internal.h"
#include "../hack/features/skins/skins.h"
#include "../hack/features/misc/announcer.h"
#include "custom.h"

Menu g_menu;
Input g_input;
Keyhandler g_keyhandler;
 
std::size_t NumFiles(std::filesystem::path path)
{
	using std::filesystem::directory_iterator;
	return std::distance(directory_iterator(path), directory_iterator{});
}

void Menu::Draw()
{
	Menu::Handle();

	if (!this->m_bIsOpened && ui::GetStyle().Alpha == 0.f)
		return;

	ui::SetNextWindowSize(ImVec2(550, 384));
	ui::Begin("owned.me", 0, ImGuiWindowFlags_NoDecoration); {
		auto draw = ui::GetWindowDrawList();
		ImVec2 pos = ui::GetWindowPos();
		ImVec2 size = ui::GetWindowSize();

		ImGuiContext& g = *GImGui;

		draw->AddRectFilled(ImVec2(pos.x + 2, pos.y + 1), ImVec2(pos.x + size.x - 2, pos.y + 21), ImColor(10 / 255.0f, 10 / 255.0f, 10 / 255.0f, g.Style.Alpha), 3.f); // upper rect
		draw->AddRect(ImVec2(pos.x + 1, pos.y + 1), ImVec2(pos.x + size.x - 1, pos.y + size.y - 1), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.08f), 3.f); // outline around the window
		draw->AddLine(ImVec2(pos.x + 2, pos.y + 22), ImVec2(pos.x + size.x - 2, pos.y + 22), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.10f)); // upper line
		draw->AddRectFilledMultiColor(ImVec2(pos.x + size.x / 2 + 10, pos.y + 23), ImVec2(pos.x + size.x - 2, pos.y + 24), ImColor(146, 157, 223, 0), ImColor(146 / 255.0f, 157 / 255.0f, 223 / 255.0f, g.Style.Alpha), ImColor(146 / 255.0f, 157 / 255.0f, 223 / 255.0f, g.Style.Alpha), ImColor(146, 157, 223, 0)); // upper gradient

		draw->AddLine(ImVec2(pos.x + 2, pos.y + size.y - 22), ImVec2(pos.x + size.x - 2, pos.y + size.y - 22), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.10f)); // bottom line

		draw->AddRectFilled(ImVec2(pos.x + 12, pos.y + 32), ImVec2(pos.x + 127, pos.y + size.y - 32), ImColor(10 / 255.0f, 10 / 255.0f, 10 / 255.0f, g.Style.Alpha)); // subtabs rect
		draw->AddRect(ImVec2(pos.x + 12, pos.y + 31), ImVec2(pos.x + 127, pos.y + size.y - 31), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.08f)); // subtabs outline

		draw->AddText(ImVec2(pos.x + 7, pos.y + 5), ImColor(146 / 255.0f, 157 / 255.0f, 223 / 255.0f, g.Style.Alpha), "owned.me");

		std::string str(__DATE__);
		draw->AddText(ImVec2(pos.x + 7, pos.y + size.y - 18), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.4f), str.c_str());

		ui::SetCursorPos(ImVec2(115, 6));
		ui::BeginGroup();
		if (p_interface::tab("legit", 0 == this->m_bCurrentTab)) { this->m_bCurrentTab = 0; } ui::SameLine();
		if (p_interface::tab("rage", 1 == this->m_bCurrentTab)) { this->m_bCurrentTab = 1; } ui::SameLine();
		if (p_interface::tab("anti-aim", 2 == this->m_bCurrentTab)) { this->m_bCurrentTab = 2; } ui::SameLine();
		if (p_interface::tab("visuals", 3 == this->m_bCurrentTab)) { this->m_bCurrentTab = 3; } ui::SameLine();
		if (p_interface::tab("misc", 4 == this->m_bCurrentTab)) { this->m_bCurrentTab = 4; } ui::SameLine();
		if (p_interface::tab("skins", 5 == this->m_bCurrentTab)) { this->m_bCurrentTab = 5; }
		ui::EndGroup();

		if (this->m_bCurrentTab == 0) {
			static int curr_weapon = 0;

			ui::SetCursorPos(ImVec2(18, 40));
			ui::BeginGroup();
			if (p_interface::subtab("auto", 0 == curr_weapon)) curr_weapon = 0;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("scout", 1 == curr_weapon)) curr_weapon = 1;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("awp", 2 == curr_weapon)) curr_weapon = 2;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("deagle", 3 == curr_weapon)) curr_weapon = 3;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("revolver", 4 == curr_weapon)) curr_weapon = 4;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("pistols", 5 == curr_weapon)) curr_weapon = 5;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("rifles", 6 == curr_weapon)) curr_weapon = 6;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("shotguns", 7 == curr_weapon)) curr_weapon = 7;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("smgs", 8 == curr_weapon)) curr_weapon = 8;
			ui::EndGroup();

			ui::SetCursorPos(ImVec2(137, 31));
			ui::begin_child("aimbot", ImVec2(195, 230));
			{
				auto& legit_set = config.legitsettings[curr_weapon];

				SetCursorPosX(5);
				ui::Checkbox("enabled", &legit_set.legit_enable);
				ui::Keybind("##aim_key", &legit_set.legit_key, &legit_set.legit_key_style);
				SetCursorPosX(5);
				ui::Checkbox("silent", &legit_set.legit_silent);
				SetCursorPosX(5);
				ui::MultiSelect("hitboxes", &legit_set.hitboxes, { "head","body","hands","legs" });
				SetCursorPosX(5);
				ui::MultiSelect("extra checks", &legit_set.checks, { "aim at air", "aim at flash", "aim at team", "aim at wall", "aim at smoke" });
				SetCursorPosX(5);
				ui::SliderFloat("maximum fov", &legit_set.fov, 1.f, 100.f);
				SetCursorPosX(5);
				ui::SliderFloat("smooth", &legit_set.smooth, 1.f, 100.f);
				SetCursorPosX(5);
				ui::SliderFloat("recoil compensation", &legit_set.rcs, 0.0f, 1.f);
				SetCursorPosX(5);
				ui::SliderInt("reaction time", &legit_set.kill_delay, 0, 350);

				if (legit_set.checks[3])
				{
					SetCursorPosX(5);
					ui::SliderFloat("minimum wall damage", &legit_set.wall_dmg, 0.f, 100.f);
				}
			}
			ui::end_child();

			ui::SetCursorPos(ImVec2(137, 270));
			ui::begin_child("backtrack", ImVec2(195, 83));
			{
				SetCursorPosX(5);
				ui::Checkbox("enabled", &config.backtrack_enable);
			}
			ui::end_child();

			ui::SetCursorPos(ImVec2(342, 31));
			ui::begin_child("triggerbot", ImVec2(195, 322));
			{
				auto& trig_set = config.triggersettings[curr_weapon];

				SetCursorPosX(5);
				ui::Checkbox("enabled", &trig_set.trigger_enable);
				ui::Keybind("##trigger_key", &trig_set.trigger_key, &trig_set.trigger_key_style);
				SetCursorPosX(5);
				ui::MultiSelect("hitboxes", &trig_set.hitgroup, { "head","chest","stomach","left arm","right arm","left leg","right leg" });
				SetCursorPosX(5);
				ui::SliderInt("minimum damage", &trig_set.minDamage, 0, 100);
				SetCursorPosX(5);
				ui::SliderInt("minimum wall damage", &trig_set.minDamageAutoWall, 0, 100);
				SetCursorPosX(5);
				ui::SliderInt("reaction time", &trig_set.shotDelay, 0, 300);
				SetCursorPosX(5);
				ui::SliderFloat("burst time", &trig_set.burstTime, 0.0f, 1.0f);
				SetCursorPosX(5);
				ui::SliderFloat("distance", &trig_set.distance, 1.0f, 1000.0f);
				SetCursorPosX(5);
				ui::Checkbox("shoot while blind", &trig_set.ignoreFlash);
				SetCursorPosX(5);
				ui::Checkbox("shoot through smoke", &trig_set.ignoreSmoke);
				SetCursorPosX(5);
				ui::Checkbox("scoped only", &trig_set.scopedOnly);
			}
			ui::end_child();
		}

		if (this->m_bCurrentTab == 1) {
			static int curr_weapon = 0;

			ui::SetCursorPos(ImVec2(18, 40));
			ui::BeginGroup();
			if (p_interface::subtab("auto", 0 == curr_weapon)) curr_weapon = 0;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("scout", 1 == curr_weapon)) curr_weapon = 1;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("awp", 2 == curr_weapon)) curr_weapon = 2;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("deagle", 3 == curr_weapon)) curr_weapon = 3;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("revolver", 4 == curr_weapon)) curr_weapon = 4;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("pistols", 5 == curr_weapon)) curr_weapon = 5;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("rifles", 6 == curr_weapon)) curr_weapon = 6;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("shotguns", 7 == curr_weapon)) curr_weapon = 7;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("smgs", 8 == curr_weapon)) curr_weapon = 8;
			ui::EndGroup();

			ui::SetCursorPos(ImVec2(137, 31));
			ui::begin_child("general", ImVec2(195, 170));
			{
				auto& rage_set = config.ragesettings[curr_weapon];
				SetCursorPosX(5);
				ui::Checkbox("enabled", &rage_set.ragebot_enable);
				SetCursorPosX(5);
				ui::Checkbox("automatic fire", &rage_set.autoshoot_enable);

				if (curr_weapon == 0 || curr_weapon == 1 || curr_weapon == 2)
				{
					SetCursorPosX(5);
					ui::Checkbox("automatic scope", &rage_set.autoscope_enable);
				}

				if (curr_weapon == 4)
				{
					SetCursorPosX(5);
					ui::Checkbox("automatic revolver", &config.auto_revolver);
				}

				SetCursorPosX(5);
				ui::Checkbox("knifebot", &config.knifebot);
				SetCursorPosX(5);
				ui::Checkbox("zeusbot", &config.zeusbot);
				SetCursorPosX(5);
				ui::SliderInt("zeusbot hithance", &config.zeus_hc, 0, 100);
			}
			ui::end_child();

			ui::SetCursorPos(ImVec2(137, 210));
			ui::begin_child("exploits", ImVec2(195, 143));
			{
				SetCursorPosX(5);
				ui::Text("double tap");
				ui::Keybind("#dt", &config.doubletap_key, &config.doubletap_key_style);
				SetCursorPosX(5);
				ui::Text("hide shot");
				ui::Keybind("#hs", &config.hideshot_key, &config.hideshot_key_style);
			}
			ui::end_child();

			ui::SetCursorPos(ImVec2(342, 31));
			ui::begin_child("accuracy", ImVec2(195, 322));
			{
				auto& rage_set = config.ragesettings[curr_weapon];
				SetCursorPosX(5);
				ui::Checkbox("automatic stop", &rage_set.autostop_enable);
				SetCursorPosX(5);
				ui::MultiSelect("automatic stop type", &rage_set.autostop_type, { "early", "between shots" });
				SetCursorPosX(5);
				ui::MultiSelect("hitboxes", &rage_set.hitboxes, { "head", "neck", "upper chest", "chest", "lower chest", "body", "pelvis", "arms", "legs", "feet" });
				SetCursorPosX(5);
				ui::SingleSelect("hitbox prefer", &rage_set.hitbox_prefer, { "none", "head", "body" });
				SetCursorPosX(5);
				ui::MultiSelect("head prefers", &rage_set.head_prefers, { "onshot", "standing", "high velocity", "air" /*"safepoint"*/ });
				SetCursorPosX(5);
				ui::MultiSelect("body prefers", &rage_set.body_prefers, { "lethal", "lethal x2", "standing", "high velocity", "air" /*"safepoint"*/ });
				SetCursorPosX(5);
				ui::SliderInt("hithance", &rage_set.hitchance_amount, 0, 100);
				SetCursorPosX(5);
				ui::SliderInt("minimum damage", &rage_set.min_dmg, 0, 110);
				SetCursorPosX(5);
				ui::SliderInt("minimum wall damage", &rage_set.min_dmg_awall, 0, 110);
				SetCursorPosX(5);
				ui::Text("minimum override damage");
				ui::Keybind("#dmg", &rage_set.min_dmg_key, &rage_set.min_dmg_style);
				SetCursorPosX(5);
				ui::SliderInt("##over_dmg", &rage_set.min_dmg_override, 0, 110);
				SetCursorPosX(5);
				ui::Checkbox("force safety", &rage_set.force_safepoints);
				SetCursorPosX(5);
				ui::Checkbox("adpative multipoints scale", &rage_set.adaptive_scale);
				SetCursorPosX(5);
				ui::SliderInt("multipoints scale", &rage_set.mp_scale, 0, 100);
			}
			ui::end_child();
		}

		if (this->m_bCurrentTab == 2) {
			ui::SetCursorPos(ImVec2(18, 40));
			ui::BeginGroup();
			if (p_interface::subtab("anti-aimbot", 0 == this->m_bCurrentSubTab)) this->m_bCurrentSubTab = 0;
			ui::EndGroup();

			ui::SetCursorPos(ImVec2(137, 31));
			ui::begin_child("anti-aimbot angles", ImVec2(195, 220));
			{
				//ui::Checkbox("Desync chams", &config.desync_chams);
				//ColorSelect("##pc", config.chams_des_col, true);
				ui::SetCursorPosX(5);
				ui::Checkbox("enabled", &config.enable_antiaim);

				ui::SetCursorPosX(5);
				ui::SingleSelect("pitch", &config.pitch_type, { "none", "down", "up" });
				ui::SetCursorPosX(5);
				ui::SingleSelect("yaw base", &config.base_angle, { "local view", "at targets" });
				ui::SetCursorPosX(5);
				ui::SingleSelect("yaw", &config.yaw_type, { "static", "jitter" });

				if (config.yaw_type == 1)
				{
					SetCursorPosX(5);
					ui::SliderInt("jitter range", &config.jitter_range, 1, 180);
				}

				ui::SetCursorPosX(5);
				ui::SingleSelect("fake", &config.desync_type, { "none", "static", "jitter" });

				if (config.desync_type)
				{
					SetCursorPosX(5);
					ui::SliderInt("angle offset", &config.desync_range, 1, 60);
					SetCursorPosX(5);
					ui::SliderInt("inverted angle offset", &config.inverted_desync_range, 1, 60);

					if (config.desync_type == 1)
					{
						ui::SetCursorPosX(5);
						ui::Text("inverter");
						ui::Keybind("#inverter_key", &config.desync_key, &config.desync_key_style);
					}
				}

				ui::SetCursorPosX(5);
				ui::Text("back dir");
				ui::Keybind("#back_dir", &config.backside);
				ui::SetCursorPosX(5);
				ui::Text("left dir");
				ui::Keybind("#left_dir", &config.leftside);
				ui::SetCursorPosX(5);
				ui::Text("right dir");
				ui::Keybind("#right_dir", &config.rightside);
			}
			ui::end_child();

			ui::SetCursorPos(ImVec2(137, 260));
			ui::begin_child("extended angles", ImVec2(195, 93));
			{
				ui::SetCursorPosX(5);
				ui::Checkbox("enabled", &config.extended_ang_enable);
				ui::SetCursorPosX(5);
				ui::SliderFloat("pitch roll", &config.extended_ang, -50, 50);
			}
			ui::end_child();

			ui::SetCursorPos(ImVec2(342, 31));
			ui::begin_child("fake-lags", ImVec2(195, 166));
			{
				ui::SetCursorPosX(5);
				ui::Checkbox("enabled", &config.enable_fakelag);
				ui::SetCursorPosX(5);
				ui::SingleSelect("amount", &config.fakelag_type, { "maximum", "break", "fluctuate" });
				SetCursorPosX(5);
				ui::SliderInt("variance", &config.fakelag_variance, 1, 100);
				SetCursorPosX(5);
				ui::SliderInt("limit", &config.limit_fakelag, 2, 14);
			}
			ui::end_child();

			ui::SetCursorPos(ImVec2(342, 206));
			ui::begin_child("other", ImVec2(195, 147));
			{
				ui::SetCursorPosX(5);
				ui::Checkbox("fakeduck", &config.fakeduck);
				ui::Keybind("#fd", &config.fakeduck_key, &config.fakeduck_key_style);
				ui::SetCursorPosX(5);
				ui::Checkbox("slowwalk", &config.slowwalk);
				ui::Keybind("#sw", &config.slowwalk_key, &config.slowwalk_key_style);
				ui::SetCursorPosX(5);
				ui::Checkbox("quick peek", &config.autopeek);
				ui::Keybind("#qp", &config.autopeek_key, &config.autopeek_key_style);
			}
			ui::end_child();
		}

		if (this->m_bCurrentTab == 3) {
			ui::SetCursorPos(ImVec2(18, 40));
			ui::BeginGroup();
			if (p_interface::subtab("esp", 0 == this->m_bCurrentSubTab)) this->m_bCurrentSubTab = 0;
			ui::SetCursorPosX(18);
			if (p_interface::subtab("world", 1 == this->m_bCurrentSubTab)) this->m_bCurrentSubTab = 1;
			ui::EndGroup();

			if (this->m_bCurrentSubTab == 0) {
				ui::SetCursorPos(ImVec2(137, 31));
				ui::begin_child("esp", ImVec2(195, 322));
				{
					ui::SetCursorPosX(5);
					ui::Checkbox("bounding box", &config.box);
					ColorSelect("##box_col", config.box_col, true);
					
					ui::SetCursorPos(ImVec2(5, 29));
					ui::Checkbox("name", &config.name);
					ColorSelect("##name_col", config.name_col, true);

					ui::SetCursorPos(ImVec2(5, 49));
					ui::Checkbox("weapon", &config.weapon);
					ColorSelect("##weapon_col", config.weapon_col, true);

					////ui::Checkbox("weapon icon", &config.weapon_icon);
					////ColorSelect("##weaponicon_col", config.weapon_icon_col, true);

					ui::SetCursorPos(ImVec2(5, 69));
					ui::Checkbox("ammo", &config.ammo);
					ColorSelect("##ammo_col", config.ammo_col, true);

					ui::SetCursorPos(ImVec2(5, 89));
					ui::Checkbox("glow", &config.glow);
					ColorSelect("##glow", config.glow_col, true);

					ui::SetCursorPos(ImVec2(5, 109));
					ui::Checkbox("health", &config.health);

					ui::SetCursorPosX(5);
					ui::Checkbox("flags", &config.flags);

					if (config.flags)
					{
						ui::SetCursorPosX(5);
						ui::MultiSelect("##pflags", &config.playerflags, { "money", "armor", "scoped", "flashed", "reload", "hparmor text" });
					}

					ui::SetCursorPosX(5);
					ui::Checkbox("dormant", &config.dormant);

					if (config.dormant)
					{
						ui::SetCursorPosX(5);
						ui::SliderInt("##dormancy", &config.dormancy, 1, 10, "%ds");
					}
				}
				ui::end_child();

				ui::SetCursorPos(ImVec2(342, 31));
				ui::begin_child("colored models", ImVec2(195, 322));
				{
					static int selected = 0;
					ui::SetCursorPosX(5);
					ui::SingleSelect("model", &selected, { "players", "hands"/*, "weapons"*/ });

					switch (selected)
					{
					case 0:
					{
						ui::SetCursorPosX(5);
						ui::SingleSelect("##basemat", &config.players.basemat, { "normal", "flat" });
						ui::SetCursorPosX(5);
						ui::Checkbox("enabled", &config.players.visible);
						ColorSelect("##visible_col", config.players.vis_col, true);

						ui::SetCursorPos(ImVec2(5, 97));
						ui::Checkbox("behind walls", &config.players.invisible);
						ColorSelect("##invisible_col", config.players.invis_col, true);

						ui::SetCursorPos(ImVec2(5, 117));
						ui::Checkbox("wireframe", &config.players.wireframe);
						//ui::Checkbox("backtrack chams", &config.players.backtrack);
						//ColorSelect("##backtrack_col", config.players.backtrack_col, true);

						if (config.players.basemat == 0)
						{
							ui::SetCursorPos(ImVec2(5, 137));
							ui::Checkbox("phong", &config.players.phong);
							ColorSelect("##phong_col", config.players.phong_col, true);
							ui::SetCursorPos(ImVec2(5, 157));
							ui::Checkbox("reflectivity", &config.players.reflectivity);
							ColorSelect("##reflectivity_col", config.players.reflectivity_col, true);
							ui::SetCursorPos(ImVec2(5, 177));
							ui::SliderInt("pearlescent", &config.players.pearlescent, 0, 100, "%d%%");
							ui::SetCursorPos(ImVec2(5, 207));
							ui::SingleSelect("overlay material", &config.players.overlaymat, { "none", "glow", "devglow" });
							ColorSelect("##overlayco", config.players.overlay_col, true);
							ui::SetCursorPos(ImVec2(5, 247));
							ui::Checkbox("overlay wireframe", &config.players.overlaywireframe);
							if (config.players.overlaymat != 0)
							{
								ui::SetCursorPos(ImVec2(5, 267));
								ui::Checkbox("overlay behind walls", &config.players.overlayxqz);
								ColorSelect("##overlayco2", config.players.overlay_xqz_col, true);
							}
						}
						else
						{
							ui::SetCursorPos(ImVec2(5, 137));
							ui::SingleSelect("overlay material", &config.players.overlaymat, { "none", "glow", "devglow" });
							ColorSelect("##overlayco", config.players.overlay_col, true);

							ui::SetCursorPos(ImVec2(5, 177));
							ui::Checkbox("overlay wireframe", &config.players.overlaywireframe);

							if (config.players.overlaymat != 0)
							{
								ui::SetCursorPos(ImVec2(5, 197));
								ui::Checkbox("overlay behind walls", &config.players.overlayxqz);
								ColorSelect("##overlayco2", config.players.overlay_xqz_col, true);
							}
						}
					}
					break;
					case 1:
					{
						ui::SetCursorPosX(5);
						ui::SingleSelect("##basemat", &config.hands.basemat, { "normal", "flat" });
						ui::SetCursorPosX(5);
						ui::Checkbox("enabled", &config.hands.enabled);
						ColorSelect("##col", config.hands.col, true);

						ui::SetCursorPos(ImVec2(5, 97));
						ui::Checkbox("wireframe", &config.hands.wireframe);
						if (config.hands.basemat == 0)
						{
							ui::SetCursorPos(ImVec2(5, 117));
							ui::Checkbox("phong", &config.hands.phong);
							ColorSelect("##phong_col", config.hands.phong_col, true);
							ui::SetCursorPos(ImVec2(5, 137));
							ui::Checkbox("reflectivity", &config.hands.reflectivity);
							ColorSelect("##reflectivity_col", config.hands.reflectivity_col, true);
							ui::SetCursorPos(ImVec2(5, 157));
							ui::SliderInt("pearlescent", &config.hands.pearlescent, 0, 100, "%d%%");
							ui::SetCursorPos(ImVec2(5, 187));
							ui::SingleSelect("overlay material", &config.hands.overlaymat, { "none", "glow", "devglow" });
							ColorSelect("##overlayco", config.hands.overlay_col, true);
							ui::SetCursorPos(ImVec2(5, 227));
							ui::Checkbox("overlay wireframe", &config.hands.overlaywireframe);
						}
						else
						{
							ui::SetCursorPos(ImVec2(5, 117));
							ui::SingleSelect("overlay material", &config.hands.overlaymat, { "none", "glow", "devglow" });
							ColorSelect("##overlayco", config.hands.overlay_col, true);
							ui::SetCursorPos(ImVec2(5, 157));
							ui::Checkbox("overlay wireframe", &config.hands.overlaywireframe);
						}
					}
					break;
					//case 2:
					//{
					//	ui::SetCursorPosX(5);
					//	ui::SingleSelect("##basemat", &config.weapons.basemat, { "normal", "flat" });
					//	ui::SetCursorPosX(5);
					//	ui::Checkbox("enabled", &config.weapons.enabled);
					//	ColorSelect("##col", config.weapons.col, true);
					//	ui::SetCursorPosX(5);
					//	ui::Checkbox("wireframe", &config.weapons.wireframe);
					//	if (config.weapons.basemat == 0)
					//	{
					//		ui::SetCursorPosX(5);
					//		ui::Checkbox("phong", &config.weapons.phong);
					//		ColorSelect("##phong_col", config.weapons.phong_col, true);
					//		ui::SetCursorPosX(5);
					//		ui::Checkbox("reflectivity", &config.weapons.reflectivity);
					//		ColorSelect("##reflectivity_col", config.weapons.reflectivity_col, true);
					//		ui::SetCursorPosX(5);
					//		ui::SliderInt("pearlescent", &config.weapons.pearlescent, 0, 100, "%d%%");
					//	}

					//	ui::SetCursorPosX(5);
					//	ui::SingleSelect("overlay material", &config.weapons.overlaymat, { "none", "glow", "devglow" });
					//	ColorSelect("##overlayco", config.weapons.overlay_col, true);
					//	ui::SetCursorPosX(5);
					//	ui::Checkbox("overlay wireframe", &config.weapons.overlaywireframe);

					//}
					//break;
					//}
					}
				}
				ui::end_child();
			}

			if (this->m_bCurrentSubTab == 1) {
				ui::SetCursorPos(ImVec2(137, 31));
				ui::begin_child("effects", ImVec2(195, 166));
				{
					ui::SetCursorPosX(5);
					ui::Checkbox("remove scope", &config.noscope);
					ui::SetCursorPosX(5);
					ui::Checkbox("remove scope zoom", &config.noscopezoom);
					ui::SetCursorPosX(5);
					ui::Checkbox("remove smoke", &config.nosmoke);
					ui::SetCursorPosX(5);
					ui::Checkbox("remove flash effects", &config.noflash);
					ui::SetCursorPosX(5);
					ui::Checkbox("remove visual recoil", &config.novisrecoil);
					ui::SetCursorPosX(5);
					ui::Checkbox("disable post processing", &config.disablepostprocess);
					ui::SetCursorPosX(5);
					ui::Checkbox("preverse killfeed", &config.enable_killfeed);

					//ui::Checkbox("full bright", &config.fullbright);
					ui::SetCursorPosX(5);
					ui::Checkbox("night mode", &config.nightmode);
					if (config.nightmode)
					{
						ui::SetCursorPosX(5);
						ui::SliderInt("##nval", &config.brightness, 1, 100, "%d%%");
					}
					ui::SetCursorPosX(5);
					ui::Checkbox("custom molotov color", &config.changemolotov);
					ColorSelect("##fcol", config.molotov_col, true);
				}
				ui::end_child();

				ui::SetCursorPos(ImVec2(137, 206));
				ui::begin_child("extra", ImVec2(195, 147));
				{
					ui::SetCursorPosX(5);
					ui::Checkbox("custom viewmodel offset", &config.custom_viewmodel);
					if (config.custom_viewmodel)
					{
						ui::SetCursorPosX(5);
						ui::SliderFloat("offset x", &config.viewmodel_offset_x, -25.f, 25.f, "%.1f");
						ui::SetCursorPosX(5);
						ui::SliderFloat("offset y", &config.viewmodel_offset_y, -25.f, 25.f, "%.1f");
						ui::SetCursorPosX(5);
						ui::SliderFloat("offset z", &config.viewmodel_offset_z, -25.f, 25.f, "%.1f");
					}

					ui::SetCursorPosX(5);
					ui::SliderInt("viewmodel fov", &config.viewmodel_fov, 0, 60, "%d%%");
					ui::SetCursorPosX(5);
					ui::SliderInt("view fov", &config.view_fov, 0, 60, "%d%%");
				}
				ui::end_child();

				ui::SetCursorPos(ImVec2(342, 31));
				ui::begin_child("others", ImVec2(195, 322));
				{
					ui::SetCursorPosX(5);
					ui::MultiSelect("dropped weapons", &config.droppedweapons, { "text", "icon" });
					ColorSelect("##dropped_col", config.dropped_col, true);

					ui::SetCursorPos(ImVec2(5, 49));
					ui::Checkbox("dropped weapons ammo", &config.dropped_ammo);
					ColorSelect("##dropped_ammo_col", config.dropped_ammo_col, true);
					ui::SetCursorPos(ImVec2(5, 69));
					ui::Checkbox("grenades", &config.nades);
					ColorSelect("##nades_col", config.nades_col, true);
					ui::SetCursorPos(ImVec2(5, 89));
					ui::Checkbox("grenade prediciton", &config.nadepred);
					ColorSelect("##pc", config.nadepred_col, true);
					ui::SetCursorPos(ImVec2(5, 109));
					ui::Checkbox("bomb", &config.bomb);
					ui::SetCursorPos(ImVec2(5, 129));
					ui::Checkbox("radar", &config.radar);
					ui::SetCursorPos(ImVec2(5, 149));
					ui::Checkbox("recoil crosshair", &config.recoilcross);
					ui::SetCursorPos(ImVec2(5, 169));
					ui::Checkbox("indicators", &config.indicators);
					ui::SetCursorPosX(5);
					ui::Checkbox("third person", &config.thirdperson);
					ui::Keybind("#thirdperson_key", &config.thirdperson_key, &config.thirdperson_key_style);
					ui::SetCursorPosX(5);
					ui::SliderFloat("third person distance", &config.thirdperson_distance, 60, 300);
				}
				ui::end_child();
			}
		}

		if (this->m_bCurrentTab == 4) {
			ui::SetCursorPos(ImVec2(18, 40));
			ui::BeginGroup();
			if (p_interface::subtab("misc", 0 == this->m_bCurrentSubTab)) this->m_bCurrentSubTab = 0;
			ui::EndGroup();

			config.menu();

			ui::SetCursorPos(ImVec2(342, 31));
			ui::begin_child("menu", ImVec2(195, 106));
			{
				SetCursorPosX(5);
				ui::Checkbox("watermark", &config.menu_watermark);
			}
			ui::end_child();

			ui::SetCursorPos(ImVec2(342, 146));
			ui::begin_child("miscellaneous", ImVec2(195, 207));
			{
				SetCursorPosX(5);
				ui::Checkbox("reveal competitive ranks", &config.rankreveal);
				SetCursorPosX(5);
				ui::Checkbox("bunny hop", &config.bunny_hop);
				SetCursorPosX(5);
				ui::Checkbox("automatic strafer", &config.movement_strafe);
				SetCursorPosX(5);
				ui::Checkbox("infinite duck", &config.infinite_duck, false);
				SetCursorPosX(5);
				ui::Checkbox("jump bug", &config.jumpbug);
				ui::Keybind("##jbug", &config.jumpbugkey, &config.jumpbugkey_style);
				SetCursorPosX(5);
				ui::Checkbox("edge jump", &config.edgejump);
				ui::Keybind("##ej", &config.edgejumpkey, &config.edgejumpkey_style);
				SetCursorPosX(5);
				ui::Checkbox("fast stop", &config.faststop);
				SetCursorPosX(5);
				ui::Checkbox("ClanTag", &config.clantag);
			}
			ui::end_child();
		}

		if (this->m_bCurrentTab == 5) {
			ui::SetCursorPos(ImVec2(18, 40));
			ui::BeginGroup();
			if (p_interface::subtab("skinchanger", 0 == this->m_bCurrentSubTab)) this->m_bCurrentSubTab = 0;
			ui::EndGroup();

			static bool faszom = true;

			if (faszom)
			{
				skins::get().first_time_render = true;
				faszom = false;
			}

			skins::get().menu();
		}
	}

	ui::End();
}

void Menu::DrawEnd() 
{
	if (!this->m_bInitialized)
		return;

	ui::EndFrame();
	ui::Render();
	ImGui_ImplDX9_RenderDrawData(ui::GetDrawData());
}

bool Menu::IsMenuInitialized() 
{
	return this->m_bInitialized;
}

bool Menu::IsMenuOpened() 
{
	return this->m_bIsOpened;
}

void Menu::SetMenuOpened(bool v) 
{
	this->m_bIsOpened = v;
}

void Menu::Initialize(IDirect3DDevice9* pDevice)
{
	if (this->m_bInitialized)
		return;

	ui::CreateContext();
	auto io = ui::GetIO();
	auto style = &ui::GetStyle();

	style->WindowRounding = 0.f;
	style->AntiAliasedLines = false;
	style->AntiAliasedFill = false;
	style->ScrollbarRounding = 0.f;
	style->ScrollbarSize = 6.f;
	style->WindowPadding = ImVec2(0, 0);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1.f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(65 / 255.f, 65 / 255.f, 65 / 255.f, 1.f);

	this->m_bIsOpened = true;

	IDirect3DSwapChain9* pChain = nullptr;
	D3DPRESENT_PARAMETERS pp = {};
	D3DDEVICE_CREATION_PARAMETERS param = {};
	pDevice->GetCreationParameters(&param);
	pDevice->GetSwapChain(0, &pChain);

	if (pChain)
		pChain->GetPresentParameters(&pp);

	ImGui_ImplWin32_Init(param.hFocusWindow);
	ImGui_ImplDX9_Init(pDevice);


	ImFontConfig cfg;

	cfg.RasterizerFlags = 1 << 7 | 1 << 4; // semi-disable antialiasing

	float m_flDpiScale = 1;
	io.Fonts->AddFontFromMemoryTTF(fs_tahoma_8px_ttf, fs_tahoma_8px_ttf_len, 12.0f * m_flDpiScale, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontFromMemoryTTF(fs_tahoma_8px_ttf, fs_tahoma_8px_ttf_len, 12.0f * m_flDpiScale, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontFromMemoryTTF(keybinds_font, 25600, 10.f * m_flDpiScale, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontFromMemoryTTF(undefeated_ttf, undefeated_ttf_len, 22.f * m_flDpiScale, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontDefault();

	ImGuiFreeType::BuildFontAtlas(io.Fonts, 0x00);

	this->m_bInitialized = true;
}

void Menu::DrawBegin(IDirect3DDevice9* device) {

	static bool once{ false };
	if (!once) 
	{
		g_menu.Initialize(device);

		once = true;
	}

	if (!this->m_bInitialized)
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ui::NewFrame();
}

void Menu::Handle()
{
	if (!this->m_bIsOpened && ui::GetStyle().Alpha > 0.f)
	{
		float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
		ui::GetStyle().Alpha = std::clamp(ui::GetStyle().Alpha - fc / 255.f, 0.f, 1.f);
	}

	if (this->m_bIsOpened && ui::GetStyle().Alpha < 1.f)
	{
		float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
		ui::GetStyle().Alpha = std::clamp(ui::GetStyle().Alpha + fc / 255.f, 0.f, 1.f);
	}

}

bool Keyhandler::IsKeyDown(int key) 
{
	return HIWORD(GetKeyState(key));
}

bool Keyhandler::IsKeyUp(int key) 
{
	return !HIWORD(GetKeyState(key));
}

bool Keyhandler::CheckKey(int key, int keystyle) 
{
	switch (keystyle) {
	case 0:
		return true;
	case 1:
		return this->IsKeyDown(key);
	case 2:
		return LOWORD(GetKeyState(key));
	case 3:
		return this->IsKeyUp(key);
	default:
		return true;
	}
}