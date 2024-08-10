#include "../../features/features.hpp"

float __fastcall hooks::hkViewModel() 
{
	static auto ohkViewModel = detour::view_model_fov.GetOriginal<decltype(&hkViewModel)>();

	// override viewmodel fov.
	if (g::pLocalPlayer)
		return ohkViewModel() + config.viewmodel_fov;
	else
		return ohkViewModel();
}

void __fastcall hooks::hkOverrideView(void* _this, void* edx, CViewSetup* setup) {

	static auto ohkOverrideView = detour::override_view.GetOriginal<decltype(&hkOverrideView)>();
	if (g::pLocalPlayer)
	{
		// override fov / remove scope.
		if (g::pLocalPlayer->IsScoped() && config.noscopezoom) 
		{
			setup->m_fov = 90 + config.view_fov;
		}
		if (config.view_fov != 0.0 && !g::pLocalPlayer->IsScoped())
		{
			setup->m_fov = 90 + config.view_fov;
		}

		// remove visual recoil.
		if (config.novisrecoil)
		{
			vec3_t viewPunch = g::pLocalPlayer->PunchAngle();
			vec3_t aimPunch = g::pLocalPlayer->AimPunchAngle();
			setup->m_angles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
			setup->m_angles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
			setup->m_angles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		}

		// remove flashbang effects.
		static bool bResetFlash = false;
		if (config.noflash)
		{
			g::pLocalPlayer->FlashAlpha() = 0.f;
			bResetFlash = true;
		}
		if (!config.noflash && bResetFlash)
		{
			g::pLocalPlayer->FlashAlpha() = 255.f;
			bResetFlash = false;
		}
	}

	// disable post processing.
	static bool* bDisablePostProcessing = *reinterpret_cast<bool**>(pattern::Scan(XOR("client.dll"), XOR("83 EC 4C 80 3D")) + 0x5);
	if (config.disablepostprocess)
		*bDisablePostProcessing = true;
	else
		*bDisablePostProcessing = false;

	if (!g::pLocalPlayer->IsAlive())
	{
		interfaces::input->m_bCameraInThirdPerson = false;
		ohkOverrideView(_this, edx, setup);
		return;
	}

	misc::ThirdPerson();

	if (g::pLocalPlayer) {

		static float maxduck = 0;
		static bool in_transition = false;

		if (g::bFakeDuck)
		{
			in_transition = true;
			maxduck += interfaces::globals->frame_time * 150;

			if (maxduck > interfaces::game_movement->get_player_view_offset(false).z + 0.064f)
				maxduck = interfaces::game_movement->get_player_view_offset(false).z + 0.064f;
		}
		else
		{
			if (maxduck > g::pLocalPlayer->ViewOffset().z)
				maxduck -= interfaces::globals->frame_time * 150;

			if (maxduck < g::pLocalPlayer->ViewOffset().z) {
				maxduck = g::pLocalPlayer->ViewOffset().z;

				in_transition = false;
			}
		}

		if (in_transition)
			setup->m_origin = g::pLocalPlayer->AbsOrigin() + vec3_t(0.0f, 0.0f, maxduck);
	}


	ohkOverrideView(_this, edx, setup);
}
