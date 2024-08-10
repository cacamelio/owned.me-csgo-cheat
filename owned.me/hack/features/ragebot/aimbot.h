#pragma once

enum weapon_type_rage
{
	wep_auto = 0,
	wep_scout,
	wep_awp,
	wep_deagle,
	wep_revolver,
	wep_pistol,
	wep_rifle,
	wep_shotgun,
	wep_smg
};

enum hitscan_mode 
{
	normal = 0,
	lethal = 1,
	lethal2 = 3,
	prefer = 4
};

struct hitscan_data_t 
{
	float  m_damage;
	vec3_t m_pos;
	int m_hitbox;
	int m_mode;

	__forceinline hitscan_data_t() : m_damage{ 0.f }, m_pos{ }, m_hitbox{ }, m_mode{} {}
};

struct hitbox_t 
{
	hitbox_t(const int index, const hitscan_mode mode)
	{
		this->m_index = index;
		this->m_mode =  mode;
	}

	hitbox_t(const int index, const float damage, const vec3_t point, const bool lethal)
	{
		this->m_index = index;
		this->m_damage = damage;
		this->m_point = point;
		this->m_lethal = lethal;
	}

	__forceinline bool operator==(const hitbox_t& c) const 
	{
		return m_index == c.m_index && m_mode == c.m_mode;
	}

	hitscan_mode	m_mode;
	int				m_index;
	float			m_damage;
	vec3_t			m_point;
	bool			m_lethal;
};

class aim_data_t
{
public:
	struct target_data_t 
	{
		target_data_t(BaseEntity* player, const int& idx)
		{
			this->player = player;
			this->index = idx;
		}

		BaseEntity* player;
		int			index;
	};
public:
	std::vector <vec3_t> aim_data_t::setup_hitbox_points(C_LagRecord& record, std::vector< vec3_t >& points, int index, vec3_t* pos = nullptr);
	void setup_hitboxes(C_LagRecord* record);
	int get_minimum_damage(BaseEntity* pPlayer, bool IsAutoWall);
	vec3_t get_best_damage(C_LagRecord& log, float& damage, int& hitbox, vec3_t* pos = nullptr);
	bool select_main_target(C_LagRecord* record, float damage);
	float get_point_scale(C_LagRecord log, int ihitbox, const vec3_t* pos);
public:
	std::vector<hitbox_t> enabled_hitboxes;
	std::vector<target_data_t> targets;
	BaseEntity* player;
	int			index;
	int			best_damage;
};

inline aim_data_t* g_aim_data = new aim_data_t();

class aimbot 
{
public:
	void run();
	void quick_stop();
	bool predict_stop();
	static bool can_shoot(float time = interfaces::globals->cur_time, bool check = false);
	void calc_time();

	bool get_aimbot_state() const
	{
		return aimbot_called == interfaces::globals->cur_time;
	}

	void select_targets();
	C_LagRecord* find_newest_record(BaseEntity* target_entity);
	C_LagRecord* find_oldest_record(BaseEntity* target_entity);
	void scan_targets();
	void setup_weapons();

	bool should_quick_stop = false;
	bool revolver_fire = false;
	float spike_shot_called = 0.f;
	float aimbot_called = 0.f;
	ragebot_t rage_settings = ragebot_t();
};

inline aimbot* g_aimbot = new aimbot( );

class hit_chance 
{
public:
	struct hit_chance_data_t 
	{
		float random[2];
		float inaccuracy[2];
		float spread[2];
	};

	struct hitbox_data_t 
	{
		hitbox_data_t(const vec3_t& min, const vec3_t& max, float radius, studio_box_t* hitbox, int bone, const vec3_t& rotation) 
		{
			m_min = min;
			m_max = max;
			m_radius = radius;
			m_hitbox = hitbox;
			m_bone = bone;
			m_rotation = rotation;
		}

		vec3_t m_min{ };
		vec3_t m_max{ };
		float m_radius{ };
		studio_box_t* m_hitbox{ };
		int m_bone{ };
		vec3_t m_rotation{ };
	};

	void build_seed_table();

	vec3_t get_spread_direction(BaseWeapon* weapon, vec3_t angles, int seed);
	bool can_intersect_hitbox(const vec3_t start, const vec3_t end, vec3_t spread_dir, C_LagRecord& log, int hitbox);
	std::vector<hitbox_data_t> get_hitbox_data(C_LagRecord& log, int hitbox);
	bool intersects_bb_hitbox(vec3_t start, vec3_t delta, vec3_t min, vec3_t max);
	bool __vectorcall intersects_hitbox(vec3_t eye_pos, vec3_t end_pos, vec3_t min, vec3_t max, float radius);
	bool can_hit(C_LagRecord& log, BaseWeapon* weapon, vec3_t angles, int hitbox);
private:
	std::array<hit_chance_data_t, 256> hit_chance_records = {};
};

inline hit_chance* g_hit_chance = new hit_chance();