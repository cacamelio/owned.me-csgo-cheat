#pragma once

class CWeaponData {
public:
	char		u0[0x4];
	char		*weapon_name;
	char		u1[0xC];
	__int32		weapon_max_clip;
	char		u2[0xC];
	__int32		weapon_max_reserved_ammo;
	char		u3[0x4];
	char		*weapon_model_path;
	char		*weapon_model_path_alt;
	char		*weapon_dropped_model_path;
	char		*weapon_default_clip;
	char		u4[0x44];
	char		*weapon_bullet_type;
	char		u5[0x4];
	char		*weapon_hud_name;
	char		*weapon_name_alt;
	char		u6[0x38];
	__int32		weapon_type;
	__int32		weapon_type_alt;
	__int32		weapon_price;
	__int32		weapon_reward;
	char		*weapon_type_name;
	float		weapon_unknown_float;
	char		u7[0xC];
	unsigned char	weapon_full_auto;
	char		u8[ 0x3 ];
	__int32		weapon_damage;
	float       weapon_headshot_multipier;
	float		weapon_armor_ratio;
	__int32		weapon_bullets;
	float		weapon_penetration;
	char		u9[0x8];
	float		weapon_range;
	float		weapon_range_mod;
	float		weapon_throw_velocity;
	char		u10[0xC];
	unsigned char	weapon_has_silencer;
	char		u11[0xF];
	float		weapon_max_speed;
	float		weapon_max_speed_alt;
	float		weapon_attack_move_factor;
	float		weapon_spread;
	float		weapon_spread_alt;
	float		weapon_inaccuracy_crouch;
	float		weapon_inaccuracy_crouch_alt;
	float		weapon_inaccuracy_stand;
	float		weapon_inaccuracy_stand_alt;
	float		weapon_inaccuracy_jump;
	float		weapon_inaccuracy_jump_alt;
	float		weapon_inaccuracy_land; //0x0160
	float		weapon_inaccuracy_land_alt; //0x0164
	float		weapon_inaccuracy_ladder; //0x0168
	float		weapon_inaccuracy_ladder_alt; //0x016c
	float		weapon_inaccuracy_fire; //0x0170
	float		weapon_inaccuracy_fire_alt; //0x0174
	float		weapon_inaccuracy_move; //0x0178
	float		weapon_inaccuracy_move_alt; //0x017c
	float		weapon_inaccuracy_reload; //0x0180
	int			weapon_recoil_seed; //0x0184
	float		weapon_recoil_angle; //0x0188
	float		weapon_recoil_angle_alt; //0x018c
	float		weapon_recoil_variance; //0x0190
	float		weapon_recoil_angle_variance_alt; //0x0194
	float		weapon_recoil_magnitude; //0x0198
	float		weapon_recoil_magnitude_alt; //0x019c
	float		weapon_recoil_magnatiude_veriance; //0x01a0
	float		weapon_recoil_magnatiude_veriance_alt; //0x01a4
	float		weapon_recovery_time_crouch; //0x01a8
	float		weapon_recovery_time_stand; //0x01ac
	float		weapon_recovery_time_crouch_final; //0x01b0
	float		weapon_recovery_time_stand_final; //0x01b4
	int			weapon_recovery_transitition_start_bullet; //0x01b8
	int			weapon_recovery_transitition_end_bullet; //0x01bc
	bool		weapon_unzoom_after_shot; //0x01c0
	char		pad_01c1[31]; //0x01c1
	char*		weapon_weapon_class; //0x01e0
	char		pad_01e4[56]; //0x01e4
	float		weapon_inaccuracy_pitch_shift; //0x021c
	float		weapon_inaccuracy_sound_threshold; //0x0220
	float		weapon_bot_audible_range; //0x0224
	char		pad_0228[12]; //0x0228
	bool		weapon_has_burst_mode; //0x0234
	//char		u12[0x28];
	//__int32		weapon_recoil_seed;
	//char		u13[0x68];
	//char		*weapon_traces_type;
};
