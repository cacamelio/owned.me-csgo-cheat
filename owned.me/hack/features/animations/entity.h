#pragma once
enum ADVANCED_ACTIVITY : int
{
	ACTIVITY_NONE = 0,
	ACTIVITY_JUMP,
	ACTIVITY_LAND
};

template < class T >
__forceinline T Interpolate(const T& flCurrent, const T& flTarget, const int iProgress, const int iMaximum)
{
	return flCurrent + ((flTarget - flCurrent) / iMaximum) * iProgress;
}

class C_AnimationSync
{
public:
	virtual void Instance(int Stage);
	virtual void UpdatePlayerAnimations(BaseEntity* pPlayer, C_LagRecord& LagRecord, C_LagRecord PreviousRecord, bool bHasPreviousRecord, int32_t iRotationMode);
	virtual void MarkAsDormant(int32_t iPlayerID) { m_LeftDormancy[iPlayerID] = true; };
	virtual void UnmarkAsDormant(int32_t iPlayerID) { m_LeftDormancy[iPlayerID] = false; };
	virtual bool HasLeftOutOfDormancy(int32_t iPlayerID) { return m_LeftDormancy[iPlayerID]; };
	virtual void SetPreviousRecord(int32_t iPlayerID, C_LagRecord LagRecord) { m_PreviousRecord[iPlayerID] = LagRecord; };
	virtual C_LagRecord GetPreviousRecord(int32_t iPlayerID) { return m_PreviousRecord[iPlayerID]; };
	virtual bool GetCachedMatrix(BaseEntity* pPlayer, matrix_t* aMatrix);
	virtual void OnUpdateClientSideAnimation(BaseEntity* pPlayer);

	struct
	{
		std::map < int, int32_t > m_MissedShots = { };
		std::map < int, int32_t > m_LastMissedShots = { };
		std::map < int, int32_t > m_BruteSide = { };
		std::map < int, int32_t > m_LastBruteSide = { };
		std::map < int, int32_t > m_LastTickbaseShift = { };
		std::map < int, float_t > m_LastValidTime = { };
		std::map < int, vec3_t > m_LastValidOrigin = { };
		std::map < int, bool > m_AnimResoled = { };
		std::map < int, bool > m_FirstSinceTickbaseShift = { };
	} m_ResolverData;

private:
	std::map < int, std::array < vec3_t, MAXSTUDIOBONES > > m_BoneOrigins;
	std::map < int, std::array < matrix_t, MAXSTUDIOBONES > > m_CachedMatrix = { };
	std::map < int, C_LagRecord > m_PreviousRecord = { };
	std::map < int, bool > m_LeftDormancy = { };
};

inline C_AnimationSync* g_AnimationSync = new C_AnimationSync();