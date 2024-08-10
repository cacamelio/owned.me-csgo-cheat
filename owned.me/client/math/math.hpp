#pragma once

struct bbox_t
{
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
};

namespace math {
	
	constexpr float pi = 3.1415926535897932384f; 
	constexpr float pi_2 = pi * 2.f;             

	float random_float(float min, float max);
	void random_seed(int seed);
	vec3_t calculate_angle(vec3_t& a, vec3_t& b);
	vec3_t calculate_angle(const vec3_t& source, const vec3_t& destination, const vec3_t& viewAngles);
	void sin_cos(float r, float* s, float* c);
	void matrix_copy(const matrix_t& in, matrix_t& out);
	void concat_transforms(const matrix_t& in1, const matrix_t& in2, matrix_t& out);
	vec3_t angle_vector(vec3_t angle);
	void transform_vector(const vec3_t&, const matrix_t&, vec3_t&);
	void vector_i_rotate(const vec3_t& in1, const matrix_t& in2, vec3_t& out);
	void vector_i_transform(const vec3_t& in1, const matrix_t& in2, vec3_t& out);
	void vector_angles(vec3_t&, vec3_t&);
	void angle_vectors(vec3_t&, vec3_t*, vec3_t*, vec3_t*);
	void angle_vectors(const vec3_t&, vec3_t&);
	void angle_to_vectors(const vec3_t& angles, vec3_t* forward, vec3_t* right = nullptr, vec3_t* up = nullptr);
	int clip_ray_to_hitbox(const ray_t& ray, studio_box_t* pbox, matrix_t& matrix, trace_t& tr);
	float __vectorcall  dist_segment_to_segment(vec3_t s1, vec3_t s2, vec3_t k1, vec3_t k2);
	vec3_t vector_add(vec3_t&, vec3_t&);
	vec3_t vector_subtract(vec3_t&, vec3_t&);
	vec3_t vector_multiply(vec3_t&, vec3_t&);
	vec3_t vector_divide(vec3_t&, vec3_t&);
	matrix_t angle_matrix(const vec3_t angles);
	vec3_t vector_rotate(const vec3_t& in1, const matrix_t& in2);
	vec3_t vector_rotate(const vec3_t& in1, const vec3_t& in2);
	bool screen_transform(const vec3_t& point, vec3_t& screen);
	bool world_to_screen(const vec3_t& origin, vec2_t& screen);
	bool GetBoundingBox(BaseEntity* entity, bbox_t& box);
	void angle_matrix(const vec3_t& ang, const vec3_t& pos, matrix_t& out);
	void normalize_angles(vec3_t& angles);
	float get_fov(const vec3_t& viewangles, const vec3_t& aim_angle);
	inline float DistancePointToLine(vec3_t Point, vec3_t LineOrigin, vec3_t Dir)
	{
		auto PointDir = Point - LineOrigin;

		auto TempOffset = PointDir.dot(Dir) / (Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z);
		if (TempOffset < 0.000001f)
			return FLT_MAX;

		auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

		return (Point - PerpendicularPoint).length();
	}

	__forceinline float NormalizeYaw(float f) 
	{
		while (f < -180.0f)
			f += 360.0f;

		while (f > 180.0f)
			f -= 360.0f;

		return f;
	}

	inline void fast_rsqrt(float a, float* out)
	{
		const auto xx = _mm_load_ss(&a);
		auto xr = _mm_rsqrt_ss(xx);
		auto xt = _mm_mul_ss(xr, xr);
		xt = _mm_mul_ss(xt, xx);
		xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
		xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
		xr = _mm_mul_ss(xr, xt);
		_mm_store_ss(out, xr);
	}


	__forceinline float fast_vec_normalize(vec3_t& vec)
	{
		const auto sqrlen = vec.length_sqr() + 1.0e-10f;
		float invlen;
		fast_rsqrt(sqrlen, &invlen);
		vec.x *= invlen;
		vec.y *= invlen;
		vec.z *= invlen;
		return sqrlen * invlen;
	}

	__forceinline void NormalizeAngle(float& angle) 
	{
		float rot;

		if (!std::isfinite(angle)) {
			angle = 0.f;
			return;
		}

		if (angle >= -180.f && angle <= 180.f)
			return;

		rot = std::round(std::abs(angle / 360.f));

		angle = (angle < 0.f) ? angle + (360.f * rot) : angle - (360.f * rot);
	}

	__forceinline float NormalizedAngle(float angle) 
	{
		NormalizeAngle(angle);
		return angle;
	}

	__forceinline float NormalizeAnglee(float flAngle)
	{
		flAngle = fmod(flAngle, 360.0f);
		if (flAngle > 180.0f)
			flAngle -= 360.0f;
		if (flAngle < -180.0f)
			flAngle += 360.0f;

		return flAngle;
	}

	__forceinline float AngleNormalize(float angle)
	{
		angle = fmodf(angle, 360.0f);
		if (angle > 180)
		{
			angle -= 360;
		}
		if (angle < -180)
		{
			angle += 360;
		}
		return angle;
	}


	__forceinline float AngleDiff(float destAngle, float srcAngle)
	{
		float delta = fmodf(destAngle - srcAngle, 360.0f);
		if (destAngle > srcAngle)
		{
			if (delta >= 180)
				delta -= 360;
		}
		else
		{
			if (delta <= -180)
				delta += 360;
		}
		return delta;
	}

	__forceinline float blend(float a, float b, float multiplier) 
	{
		return a + static_cast<int>(multiplier * (b - a));
	}

	template < typename t >
	__forceinline void clamp(t& n, const t& lower, const t& upper) 
	{
		n = std::max(lower, std::min(n, upper));
	}

	inline float simple_spline(float value) {

		float squared_value = value * value;

		return (3 * squared_value - 2 * squared_value * value);

	}

	inline float simple_spline_remap_val_clamped(float val, float a, float b, float c, float d) {

		if (a == b)
			return val >= b ? d : c;

		float clamped_value = (val - a) / (b - a);
		clamped_value = std::clamp(clamped_value, 0.f, 1.f);
		return c + (d - c) * simple_spline(clamped_value);

	}

	inline float lerp(float percent, float a, float b) {

		return a + (b - a) * percent;

	};
};


namespace random
{
	constexpr auto time = __TIME__;
	constexpr auto seed = static_cast<unsigned>(time[7]) + static_cast<unsigned>(time[6]) * 10 + static_cast<unsigned>(time[4]) * 60 + static_cast<unsigned>(time[3]) * 600 + static_cast<unsigned>(time[1]) * 3600 + static_cast<unsigned>(time[0]) * 36000;

	template <int n>
	struct gen
	{
	private:
		static constexpr unsigned a = 16807;
		static constexpr unsigned m = 2147483647;

		static constexpr unsigned s = gen<n - 1>::value;
		static constexpr unsigned lo = a * (s & 0xFFFFu);
		static constexpr unsigned hi = a * (s >> 16u);
		static constexpr unsigned lo2 = lo + ((hi & 0x7FFFu) << 16u);
		static constexpr unsigned hi2 = hi >> 15u;
		static constexpr unsigned lo3 = lo2 + hi;

	public:
		static constexpr unsigned max = m;
		static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
	};

	template <>
	struct gen<0>
	{
		static constexpr unsigned value = seed;
	};

	template <int n, int m>
	struct _int
	{
		static constexpr auto value = gen<n + 1>::value % m;
	};

	template <int n>
	struct _char
	{
		static const char value = static_cast<char>(1 + _int<n, 0x7F - 1>::value);
	};
}
