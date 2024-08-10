#include "../../features/features.hpp"

bool __fastcall hooks::hkIsHLTV(IVEngineClient* IEngineClient)
{
	static auto IsHLTV = detour::is_hl_tv.GetOriginal<decltype(&hkIsHLTV)>();

	static const auto SetupVelocity = pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D"));
	static const auto AccumulateLayers = pattern::Scan(XOR("client.dll"), XOR("84 C0 75 0D F6 87"));

	if (_ReturnAddress() == (uint32_t*)(AccumulateLayers) && IEngineClient->is_in_game())
		return true;

	if (_ReturnAddress() == (uint32_t*)(SetupVelocity) && g::bUpdateMatrix && IEngineClient->is_in_game())
		return true;

	return IsHLTV(IEngineClient);
}