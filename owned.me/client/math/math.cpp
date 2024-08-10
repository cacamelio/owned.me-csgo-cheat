#include "../utilities/csgo.hpp"
#include "../../hack/features/visuals/esp.h"

typedef __declspec(align(16)) union
{
	float f[4];
	__m128 v;
} m128;
__forceinline __m128 sqrt_ps(const __m128 squared)
{
	return _mm_sqrt_ps(squared);
}

static const float invtwopi = 0.1591549f;
static const float twopi = 6.283185f;
static const float threehalfpi = 4.7123889f;
static const float pi = 3.141593f;
static const float halfpi = 1.570796f;
static const __m128 signmask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

__forceinline __m128 cos_52s_ps(const __m128 x)
{
	const auto c1 = _mm_set1_ps(0.9999932946f);
	const auto c2 = _mm_set1_ps(-0.4999124376f);
	const auto c3 = _mm_set1_ps(0.0414877472f);
	const auto c4 = _mm_set1_ps(-0.0012712095f);
	const auto x2 = _mm_mul_ps(x, x);
	return _mm_add_ps(c1, _mm_mul_ps(x2, _mm_add_ps(c2, _mm_mul_ps(x2, _mm_add_ps(c3, _mm_mul_ps(c4, x2))))));
}

__forceinline void sincos_ps(__m128 angle, __m128* sin, __m128* cos) {
	const auto anglesign = _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(signmask, angle));
	angle = _mm_andnot_ps(signmask, angle);
	angle = _mm_sub_ps(angle, _mm_mul_ps(_mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_mul_ps(angle, _mm_set1_ps(invtwopi)))), _mm_set1_ps(twopi)));

	auto cosangle = angle;
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi), angle))));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(pi)), signmask));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(threehalfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(twopi), angle))));

	auto result = cos_52s_ps(cosangle);
	result = _mm_xor_ps(result, _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_cmplt_ps(angle, _mm_set1_ps(threehalfpi))), signmask));
	*cos = result;

	const auto sinmultiplier = _mm_mul_ps(anglesign, _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(_mm_cmpgt_ps(angle, _mm_set1_ps(pi)), signmask)));
	*sin = _mm_mul_ps(sinmultiplier, sqrt_ps(_mm_sub_ps(_mm_set1_ps(1.f), _mm_mul_ps(result, result))));
}

float math::random_float(float min, float max) {
	static auto random_float = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));
	return random_float(min, max);
}

void math::random_seed(int seed) {
	static auto fn = (decltype(&random_seed))(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));
	return fn(seed);
}

bool math::GetBoundingBox(BaseEntity* entity, bbox_t& box)
{
	const auto collideable = entity->Collideables();

	if (collideable == nullptr)
		return false;
	vec3_t top, down, air, s[2];

	vec3_t adjust = vec3_t(0, 0, -15) * entity->DuckAmount();

	if (!(entity->Flags() & FL_ONGROUND) && (entity->MoveType() != MOVETYPE_LADDER))
		air = vec3_t(0, 0, 10);
	else
		air = vec3_t(0, 0, 0);

	down = entity->AbsOrigin() + air;
	top = down + vec3_t(0, 0, 72) + adjust;

	if (visuals::WorldToScreen(top, s[1]) && visuals::WorldToScreen(down, s[0]))
	{
		vec3_t delta = s[1] - s[0];

		box.h = fabsf(delta.y) + 6;
		box.w = box.h / 2 + 5;

		box.x = s[1].x - (box.w / 2) + 2;
		box.y = s[1].y - 1;

		return true;
	}

	return false;
}
vec3_t math::calculate_angle(const vec3_t& source, const vec3_t& destination, const vec3_t& viewAngles) {
	vec3_t delta = source - destination;
	auto radians_to_degrees = [](float radians) { return radians * 180 / static_cast<float>(M_PI); };
	vec3_t angles;
	angles.x = radians_to_degrees(atanf(delta.z / std::hypotf(delta.x, delta.y))) - viewAngles.x;
	angles.y = radians_to_degrees(atanf(delta.y / delta.x)) - viewAngles.y;
	angles.z = 0.0f;

	if (delta.x >= 0.0)
		angles.y += 180.0f;

	angles.normalize_aimbot();
	return angles;
}

vec3_t math::calculate_angle(vec3_t& a, vec3_t& b) {
	vec3_t angles;
	vec3_t delta;
	delta.x = (a.x - b.x);
	delta.y = (a.y - b.y);
	delta.z = (a.z - b.z);

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);
	angles.x = (float)(atanf(delta.z / hyp) * 57.295779513082f);
	angles.y = (float)(atanf(delta.y / delta.x) * 57.295779513082f);

	angles.z = 0.0f;
	if (delta.x >= 0.0) { angles.y += 180.0f; }
	return angles;
}

void math::sin_cos(float r, float* s, float* c) {
	*s = sin(r);
	*c = cos(r);
}

void math::matrix_copy(const matrix_t& in, matrix_t& out) {
	std::memcpy(out.base(), in.base(), sizeof(matrix_t));
}

void math::concat_transforms(const matrix_t& in1, const matrix_t& in2, matrix_t& out) {
	if (&in1 == &out) {
		matrix_t in1b;
		matrix_copy(in1, in1b);
		concat_transforms(in1b, in2, out);
		return;
	}

	if (&in2 == &out) {
		matrix_t in2b;
		matrix_copy(in2, in2b);
		concat_transforms(in1, in2b, out);
		return;
	}

	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}

vec3_t math::angle_vector(vec3_t angle) {
	auto sy = sin(angle.y / 180.f * static_cast<float>(M_PI));
	auto cy = cos(angle.y / 180.f * static_cast<float>(M_PI));

	auto sp = sin(angle.x / 180.f * static_cast<float>(M_PI));
	auto cp = cos(angle.x / 180.f * static_cast<float>(M_PI));

	return vec3_t(cp * cy, cp * sy, -sp);
}

void math::transform_vector(const vec3_t & a, const matrix_t & b, vec3_t & out) {
	//out.x = a.dot(b.mat_val[0]) + b.mat_val[0][3];
	//out.y = a.dot(b.mat_val[1]) + b.mat_val[1][3];
	//out.z = a.dot(b.mat_val[2]) + b.mat_val[2][3];
	out = {
		a.dot(vec3_t(b[0][0], b[0][1], b[0][2])) + b[0][3],
		a.dot(vec3_t(b[1][0], b[1][1], b[1][2])) + b[1][3],
		a.dot(vec3_t(b[2][0], b[2][1], b[2][2])) + b[2][3]
	};
}

void math::vector_i_rotate(const vec3_t& in1, const matrix_t& in2, vec3_t& out)
{
	out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
	out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
	out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
}

void math::vector_i_transform(const vec3_t& in1, const matrix_t& in2, vec3_t& out)
{
	out.x = (in1.x - in2[0][3]) * in2[0][0] + (in1.y - in2[1][3]) * in2[1][0] + (in1.z - in2[2][3]) * in2[2][0];
	out.y = (in1.x - in2[0][3]) * in2[0][1] + (in1.y - in2[1][3]) * in2[1][1] + (in1.z - in2[2][3]) * in2[2][1];
	out.z = (in1.x - in2[0][3]) * in2[0][2] + (in1.y - in2[1][3]) * in2[1][2] + (in1.z - in2[2][3]) * in2[2][2];
}

void math::vector_angles(vec3_t & forward, vec3_t & angles) {
	vec3_t view;

	if (!forward[0] && !forward[1])
	{
		view[0] = 0.0f;
		view[1] = 0.0f;
	}
	else
	{
		view[1] = atan2(forward[1], forward[0]) * 180.0f / M_PI;

		if (view[1] < 0.0f)
			view[1] += 360.0f;

		view[2] = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		view[0] = atan2(forward[2], view[2]) * 180.0f / M_PI;
	}

	angles[0] = -view[0];
	angles[1] = view[1];
	angles[2] = 0.f;
}
void math::angle_vectors(const vec3_t& angles, vec3_t& forward)
{
	float sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

void math::angle_to_vectors(const vec3_t& angles, vec3_t* forward, vec3_t* right, vec3_t* up)
{
	float cp = cos(DEG2RAD(angles.x)), sp = sin(DEG2RAD(angles.x));
	float cy = cos(DEG2RAD(angles.y)), sy = sin(DEG2RAD(angles.y));
	float cr = cos(DEG2RAD(angles.z)), sr = sin(DEG2RAD(angles.z));

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
		right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
		right->z = -1.f * sr * cp;
	}

	if (up)
	{
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}

/*static*/ int math::clip_ray_to_hitbox(const ray_t& ray, studio_box_t* pbox, matrix_t& matrix, trace_t& tr)
{
	static auto fn = pattern::Scan(XOR("client.dll"), XOR("55 8B EC 83 E4 F8 F3 0F 10 42 ?"));

	if (!fn || !pbox)
		return -1;

	tr.flFraction = 1.0f;
	tr.startSolid = false;
	return reinterpret_cast <int(__cdecl*)(const ray_t&, studio_box_t*, matrix_t&, trace_t&)> (fn)(ray, pbox, matrix, tr);
}
float __vectorcall  math::dist_segment_to_segment(vec3_t s1, vec3_t s2, vec3_t k1, vec3_t k2) {
	vec3_t   u = s2 - s1;
	vec3_t   v = k2 - k1;
	vec3_t   w = s1 - k1;
	float    a = u.dot(u);
	float    b = u.dot(v);
	float    c = v.dot(v);
	float    d = u.dot(w);
	float    e = v.dot(w);
	float    D = a * c - b * b;
	float    sc, sN, sD = D;
	float    tc, tN, tD = D;

	if (D < 0.00000001f) {
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else {
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0f) {
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f) {
		tN = 0.0;

		if (-d < 0.0f)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {
		tN = tD;

		if ((-d + b) < 0.0f)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}

	sc = (abs(sN) < 0.00000001f ? 0.0f : sN / sD);
	tc = (abs(tN) < 0.00000001f ? 0.0f : tN / tD);

	vec3_t  dP = w + (u * sc) - (v * tc);

	return dP.length();
}
void math::angle_vectors(vec3_t & angles, vec3_t * forward, vec3_t * right, vec3_t * up) {
	float sp, sy, sr, cp, cy, cr;

	sin_cos(DEG2RAD(angles.x), &sp, &cp);
	sin_cos(DEG2RAD(angles.y), &sy, &cy);
	sin_cos(DEG2RAD(angles.z), &sr, &cr);

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = -1 * sr * sp * cy + -1 * cr * -sy;
		right->y = -1 * sr * sp * sy + -1 * cr * cy;
		right->z = -1 * sr * cp;
	}

	if (up) {
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}

vec3_t math::vector_add(vec3_t & a, vec3_t & b) {
	return vec3_t(a.x + b.x,
		a.y + b.y,
		a.z + b.z);
}
void math::angle_matrix(const vec3_t& ang, const vec3_t& pos, matrix_t& out) {
	g::AngleMatrix(ang, out);
	out.set_origin(pos);
}

void math::normalize_angles(vec3_t& angles)
{
	angles.x = std::clamp(std::remainder(angles.x, 180.f), -89.f, 89.f);
	angles.y = std::clamp(std::remainder(angles.y, 360.f), -180.f, 180.f);
}

float math::get_fov(const vec3_t& viewangles, const vec3_t& aim_angle)
{
	vec3_t delta = aim_angle - viewangles;
	normalize_angles(delta);

	return std::min(sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f)), 180.0f);
}

vec3_t math::vector_subtract(vec3_t & a, vec3_t & b) {
	return vec3_t(a.x - b.x,
		a.y - b.y,
		a.z - b.z);
}

vec3_t math::vector_multiply(vec3_t & a, vec3_t & b) {
	return vec3_t(a.x * b.x,
		a.y * b.y,
		a.z * b.z);
}

vec3_t math::vector_divide(vec3_t & a, vec3_t & b) {
	return vec3_t(a.x / b.x,
		a.y / b.y,
		a.z / b.z);
}

matrix_t math::angle_matrix(const vec3_t angles)
{
	matrix_t result{};

	m128 angle, sin, cos;
	angle.f[0] = DEG2RAD(angles.x);
	angle.f[1] = DEG2RAD(angles.y);
	angle.f[2] = DEG2RAD(angles.z);
	sincos_ps(angle.v, &sin.v, &cos.v);

	result[0][0] = cos.f[0] * cos.f[1];
	result[1][0] = cos.f[0] * sin.f[1];
	result[2][0] = -sin.f[0];

	const auto crcy = cos.f[2] * cos.f[1];
	const auto crsy = cos.f[2] * sin.f[1];
	const auto srcy = sin.f[2] * cos.f[1];
	const auto srsy = sin.f[2] * sin.f[1];

	result[0][1] = sin.f[0] * srcy - crsy;
	result[1][1] = sin.f[0] * srsy + crcy;
	result[2][1] = sin.f[2] * cos.f[0];

	result[0][2] = sin.f[0] * crcy + srsy;
	result[1][2] = sin.f[0] * crsy - srcy;
	result[2][2] = cos.f[2] * cos.f[0];

	return result;
}

vec3_t math::vector_rotate(const vec3_t& in1, const matrix_t& in2)
{
	return vec3_t(in1.dot(in2.at(0)), in1.dot(in2.at(1)), in1.dot(in2.at(2)));
}

vec3_t math::vector_rotate(const vec3_t& in1, const vec3_t& in2)
{
	const auto matrix = angle_matrix(in2);
	return vector_rotate(in1, matrix);
}

bool math::screen_transform(const vec3_t & point, vec3_t & screen) {
	auto matrix = interfaces::engine->world_to_screen_matrix();

	float w = matrix[3][0] * point.x + matrix[3][1] * point.y + matrix[3][2] * point.z + matrix[3][3];
	screen.x = matrix[0][0] * point.x + matrix[0][1] * point.y + matrix[0][2] * point.z + matrix[0][3];
	screen.y = matrix[1][0] * point.x + matrix[1][1] * point.y + matrix[1][2] * point.z + matrix[1][3];
	screen.z = 0.0f;

	int inverse_width = static_cast<int>((w < 0.001f) ? -1.0f / w :
		1.0f / w);

	screen.x *= inverse_width;
	screen.y *= inverse_width;
	return (w < 0.001f);
}

bool math::world_to_screen(const vec3_t & origin, vec2_t & screen) {
	static std::uintptr_t view_matrix;
	if ( !view_matrix )
		view_matrix = *reinterpret_cast< std::uintptr_t* >( reinterpret_cast< std::uintptr_t >( pattern::Scan( XOR("client.dll"), XOR("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9") ) ) + 3 ) + 176;

	const auto& matrix = *reinterpret_cast< view_matrix_t* >( view_matrix );

	const auto w = matrix.m[ 3 ][ 0 ] * origin.x + matrix.m[ 3 ][ 1 ] * origin.y + matrix.m[ 3 ][ 2 ] * origin.z + matrix.m[ 3 ][ 3 ];
	if ( w < 0.001f )
		return false;

	int x, y;
	interfaces::engine->GetScreenSize( x, y );
	
	screen.x = static_cast<float>(x) / 2.0f;
	screen.y = static_cast<float>(y) / 2.0f;

	screen.x *= 1.0f + ( matrix.m[ 0 ][ 0 ] * origin.x + matrix.m[ 0 ][ 1 ] * origin.y + matrix.m[ 0 ][ 2 ] * origin.z + matrix.m[ 0 ][ 3 ] ) / w;
	screen.y *= 1.0f - ( matrix.m[ 1 ][ 0 ] * origin.x + matrix.m[ 1 ][ 1 ] * origin.y + matrix.m[ 1 ][ 2 ] * origin.z + matrix.m[ 1 ][ 3 ] ) / w;

	return true;
}
