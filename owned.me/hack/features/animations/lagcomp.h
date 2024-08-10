#pragma once

class C_LagCompensation
{
public:
	virtual void Instance(int Stage);
	virtual void FillRecord(BaseEntity* Player, C_LagRecord& Record);
	virtual void ResetData();
	virtual void ClearRecords(int32_t iPlayerID) { g::m_CachedPlayerRecords[iPlayerID].clear(); };
	virtual void SetBreakingLagCompensation(int32_t iPlayerID) { m_BreakingLagcompensation[iPlayerID] = true; };
	virtual void UnsetBreakingLagCompensation(int32_t iPlayerID) { m_BreakingLagcompensation[iPlayerID] = false; };
	virtual bool IsBreakingLagCompensation(int32_t iPlayerID) { return m_BreakingLagcompensation[iPlayerID]; };
	virtual float_t GetLerpTime();
	virtual bool IsValidTime(float_t flTime);
	virtual std::deque < C_LagRecord >& GetPlayerRecords(int32_t iPlayerID) { return g::m_CachedPlayerRecords[iPlayerID]; };
	virtual C_LagRecord& GetPreviousRecord(int32_t iPlayerID) { return g::m_CachedPlayerRecords[iPlayerID].front(); };
	virtual C_LagRecord& GetOldestRecord(int32_t iPlayerID) { return g::m_CachedPlayerRecords[iPlayerID].back(); };
private:
	std::array < bool, 65 > m_BreakingLagcompensation = { };
};

inline C_LagCompensation* g_LagCompensation = new C_LagCompensation();