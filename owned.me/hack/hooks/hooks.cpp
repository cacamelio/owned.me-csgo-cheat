#include "../../client/utilities/csgo.hpp"
#include "../features/features.hpp"

bool hooks::initialize()
{

	g::hWindow = FindWindowA(XOR("Valve001"), NULL);
	g::OldWndProc = (WNDPROC)SetWindowLongA(g::hWindow, GWL_WNDPROC, utilities::ForceCast<LONG>(WndProc));

	if (MH_Initialize() != MH_OK)
		throw std::runtime_error(XOR("failed to initialize hooks."));

	static auto create_move_target = (void*)GetVFunc(interfaces::client, 22); // 22
	MH_CreateHook(create_move_target, hooks::player::hook, (void**)&oCreateMove);
	MH_EnableHook(create_move_target);

	if (!detour::create_move.Create(GetVFunc(interfaces::clientmode, 24), &hkCreateMove))
		throw std::runtime_error(XOR("failed to initialize hkCreateMove"));

	if (!detour::paint_traverse.Create(GetVFunc(interfaces::panel, 41), &hkPaintTraverse))
		throw std::runtime_error(XOR("failed to initialize hkPaintTraverse"));

	if (!detour::frame_stage_notify.Create(GetVFunc(interfaces::client, 37), &hkFrameStageNotify))
		throw std::runtime_error(XOR("failed to initialize hkFrameStageNotify"));

	if (!detour::lock_cursor.Create(GetVFunc(interfaces::surface, 67), &hkLockCursor))
		throw std::runtime_error(XOR("failed to initialize hkLockCursor"));

	if (!detour::post_entity.Create(GetVFunc(interfaces::client, 6), &hkLevelInitPostEntity))
		throw std::runtime_error(XOR("failed to initialize hkLevelInitPostEntity"));

	if (!detour::shutdown.Create(GetVFunc(interfaces::client, 7), &hkLevelShutdown))
		throw std::runtime_error(XOR("failed to initialize hkLevelShutdown"));

	if (!detour::post_screen_effects.Create(GetVFunc(interfaces::clientmode, 44), &hkDoPostScreenEffects))
		throw std::runtime_error(XOR("failed to initialize hkDoPostScreenEffects"));

	if (!detour::present.Create(GetVFunc(interfaces::directx, 17), &hkPresent))
		throw std::runtime_error(XOR("failed to initialize hkPresent"));

	if (!detour::reset.Create(GetVFunc(interfaces::directx, 16), &hkReset))
		throw std::runtime_error(XOR("failed to initialize hkReset"));

	if (!detour::draw_model.Create(GetVFunc(interfaces::studio_render, 29), &hkDrawModel))
		throw std::runtime_error(XOR("failed to initialize hkDrawModel"));

	if (!detour::override_config.Create(GetVFunc(interfaces::material_system, 21), &hkOverrideConfig))
		throw std::runtime_error(XOR("failed to initialize hkOverrideConfig"));

	if (!detour::list_leaves_in_box.Create(GetVFunc(interfaces::engine->get_bsp_tree_query(), 6), &hkListLeavesInBox))
		throw std::runtime_error(XOR("failed to initialize hkListLeavesInBox"));

	if (!detour::is_hl_tv.Create(GetVFunc(interfaces::engine, 93), &hkIsHLTV))
		throw std::runtime_error(XOR("failed to initialize hkIsHLTV"));

	if (!detour::view_model_fov.Create(GetVFunc(interfaces::clientmode, 35), &hkViewModel))
		throw std::runtime_error(XOR("failed to initialize hkViewModel"));

	if (!detour::override_view.Create(GetVFunc(interfaces::clientmode, 18), &hkOverrideView))
		throw std::runtime_error(XOR("failed to initialize hkViewModel"));

	if (!detour::scene_end.Create(GetVFunc(interfaces::render_view, 9), &hkSceneEnd))
		throw std::runtime_error(XOR("failed to initialize hkSceneEnd"));

	if (!detour::emit_sound.Create(GetVFunc(interfaces::engine_sound, 5), &hkEmitSound))
		throw std::runtime_error(XOR("failed to initialize hkEmitSound"));

	if (!detour::in_prediction.Create(GetVFunc(interfaces::prediction, 14), &hkInPrediction))
		throw std::runtime_error(XOR("failed to initialize hkInPrediction"));

	// hook player vtable releated hooks.
	hooks::player::Hook();

	// hook convar releated hooks.
	hooks::vars::Hook();

	console::log("[setup] hooks initialized!\n");
	return true;
}

bool hooks::vars::Hook()
{

	interfaces::console->FindVar("viewmodel_offset_x")->callbacks.SetSize(false);
	interfaces::console->FindVar("viewmodel_offset_y")->callbacks.SetSize(false);
	interfaces::console->FindVar("viewmodel_offset_z")->callbacks.SetSize(false);

	ParticleCollectionSimulateAdr = (void*)(pattern::Scan(XOR("client"), XOR("55 8B EC 83 E4 F8 83 EC 30 56 57 8B F9 0F")));
	MH_CreateHook(ParticleCollectionSimulateAdr, hkParticleCollectionSimulate, &oParticleCollectionSimulate);
	MH_EnableHook(ParticleCollectionSimulateAdr);
	return true;

}

bool hooks::player::Hook()
{
	// DoExtraBoneProcessing
	hooks::player::DoExtraBoneProcessing = (void*)(pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F8 81 ? ? ? ? ? 53 56 8B F1 57 89 74 24 1C")));
	MH_CreateHook(hooks::player::DoExtraBoneProcessing, hkDoExtraBoneProcessing, &hooks::player::oDoExtraBoneProcessing);
	MH_EnableHook(hooks::player::DoExtraBoneProcessing);

	static auto create_move_target = (void*)GetVFunc(interfaces::client, 22); // 22
	MH_CreateHook(create_move_target, hook, (void**)&oCreateMove);
	MH_EnableHook(create_move_target);

	static auto setup_bones_target = (void*)pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F0 B8 D8")); // 13
	MH_CreateHook(setup_bones_target, hkSetupBones, (void**)&o_setup_bones);
	MH_EnableHook(setup_bones_target);

	static auto update_clientside_animation_target = (void*)pattern::Scan(XOR("client.dll"), XOR("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"));
	MH_CreateHook(update_clientside_animation_target, hkUpdateAnims, (void**)&o_update_anims);
	MH_EnableHook(update_clientside_animation_target);

	static auto write_user_cmd_target = (void*)GetVFunc(interfaces::client, 24);
	MH_CreateHook(write_user_cmd_target, hkWriteUsercmdDeltaToBuffer, (void**)&o_write_user);
	MH_EnableHook(write_user_cmd_target);

	static auto physic_sim = (void*)pattern::Scan(XOR("client.dll"), XOR("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23"));
	MH_CreateHook(physic_sim, hkPhysicSim, (void**)&o_physic);
	MH_EnableHook(physic_sim);

	//static auto setcolbounds_target = (void*)pattern::Scan(XOR("client.dll"), XOR("53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 18 56 57 8B 7B"));
	//MH_CreateHook(setcolbounds_target, hkSetCollisionBounds, (void**)&o_setcol_bounds);
	//MH_EnableHook(setcolbounds_target);


	// todo:
	// standardblendingrules			// "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"
	// updateclientsideanimation		// "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"
	// setupbones						// "55 8B EC 83 E4 F0 B8 D8"
	// shouldskipanimframe				// "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"
	// processinterpolatedlist			// "53 0F B7 1D ? ? ? ? 56"
	// buildtransformations				

	return true;
}

void hooks::player::UnHook()
{
	static auto create_move_target = (void*)GetVFunc(interfaces::client, 22);
	static auto setup_bones_target = (void*)pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F0 B8 D8")); // 13
	static auto update_clientside_animation_target = (void*)pattern::Scan(XOR("client.dll"), XOR("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"));
	static auto setcolbounds_target = (void*)pattern::Scan(XOR("client.dll"), XOR("53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 18 56 57 8B 7B"));
	MH_DisableHook(hooks::player::DoExtraBoneProcessing);
	MH_DisableHook(create_move_target);
	MH_DisableHook(setup_bones_target);
	MH_DisableHook(update_clientside_animation_target);
	//MH_DisableHook(setcolbounds_target);
}


void hooks::vars::UnHook()
{
	MH_DisableHook(ParticleCollectionSimulateAdr);
}