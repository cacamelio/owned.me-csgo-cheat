#pragma once
#include "../../sdk/math/vector3d.hpp"

class IVDebugOverlay {
public:
	bool world_to_screen(const vec3_t& in, vec3_t& out) {
		using original_fn = int(__thiscall*)(IVDebugOverlay*, const vec3_t&, vec3_t&);
		int return_value = (*(original_fn * *)this)[13](this, in, out);
		return static_cast<bool>(return_value != 1);
	}

	bool screen_position(const vec3_t& in, vec3_t& out) {
		using original_fn = bool(__thiscall*)(IVDebugOverlay*, const vec3_t&, vec3_t&);
		return (*(original_fn * *)this)[11](this, std::ref(in), std::ref(out));
	}
	void add_capsule_overlay(const vec3_t& mins, const vec3_t& maxs, float pillradius, int r, int g, int b, int a, float duration, int unk, int occlude) {
		using AddCapsuleOverlay_t = void(__thiscall*)(void*, const vec3_t&, const vec3_t&, float&, int, int, int, int, float, int, int);
		return (*(AddCapsuleOverlay_t**)this)[23](this, mins, maxs, pillradius, r, g, b, a, duration, unk, occlude);
	}
};