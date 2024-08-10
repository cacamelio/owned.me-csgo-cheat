#pragma once
enum
{
	SIDE_NONE = -1,
	SIDE_BACK,
	SIDE_LEFT,
	SIDE_RIGHT
};

class NetPos {
public:
	float  m_time;
	vec3_t m_pos;

public:
	__forceinline NetPos() : m_time{}, m_pos{} {};
	__forceinline NetPos(float time, vec3_t pos) : m_time{ time }, m_pos{ pos } {};
};

namespace fakelag {
	inline int lag;
	inline bool break_lc;
	inline std::deque< NetPos > m_net_pos;
	void createMove(UserCmd* cmd);
}

namespace antiaim {
	void createMove(UserCmd* cmd);
	float getPitch(UserCmd* cmd);
	float getYaw(UserCmd* cmd);
	bool condition(UserCmd* cmd, bool check = true);

	float atTargets();

	void setSides();

	inline int type = 0;
	inline int manualSide = -1;
	inline int finalManualSide = -1;
	inline bool flip = false;
	inline bool freezeCheck = false;
	inline bool breakingLby = false;
	inline float desyncAngle = 0.0f;
}