#pragma once

namespace movement
{
	void AutoJump();
	void AutoStrafe();
	void FastStop();
	void SlowWalk();
	void InfiniteDuck();
	void EdgeJump(int iFlags);
	void JumpBug(int unFlags, int preFlags);
	void AutoPeek();
	void AutoPeekIndicator();
	void FixMove(UserCmd* cmd, vec3_t& ang);

	void run();

	inline bool m_bIsJumpbugging;
	inline bool should_move = false;
	inline float max_speed = 0.f;

	inline float_t m_flAnimationTime = 0.0f;

	inline vec3_t m_vecStartPosition = vec3_t(0, 0, 0);

	inline bool m_bTurnedOn = false;
	inline bool m_bNegativeSide = false;
	inline bool m_bRetreat = false;
	inline bool m_bRetreated = false;
	inline bool m_bWaitAnimationProgress = false;
}