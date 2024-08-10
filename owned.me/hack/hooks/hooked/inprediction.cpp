#include "../../features/features.hpp"

bool __fastcall hooks::hkInPrediction(PlayerPrediction* prediction, uint32_t e)
{
	static auto oInPrediction = detour::in_prediction.GetOriginal<decltype(&hkInPrediction)>();
	static auto maintain_sequence_transitions = pattern::Scan("client.dll", "84 C0 74 17 8B 87");

	if (g::bUpdateBones && _ReturnAddress() == maintain_sequence_transitions)
		return true;

	return oInPrediction(prediction, e);
}