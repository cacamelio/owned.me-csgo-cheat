#pragma once
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)

namespace misc
{
	void killfeed();
	void RecoilCrosshair();
	void RankRevealer();
	void ViewModelOffset();
	void DisableInterpolation();
	void ThirdPerson();
	void FakeDuck();

	inline float tp_distance = 0.0f;
}