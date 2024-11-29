#include "../csgo.hpp"
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#define FONT "https://drive.google.com/uc?id=1A7MJY2Pny-IhVCUOTS8zOkX6owNSFHLZ&export=download"

namespace render
{
	Font esp;;
	Font pixel;;
	Font console;;
	Font hud;;
	Font icon;;
	Font icon2;;
	Font indicator;;
}

void render::initialize()
{
#ifdef _DEBUG
#else
	// create directories.
	std::filesystem::create_directory("C:\\shark-hack\\");
	std::filesystem::create_directory("C:\\shark-hack\\resources\\");
	std::filesystem::create_directory("C:\\shark-hack\\audio\\");
	std::filesystem::create_directory("C:\\shark-hack\\audio\\hun\\");
	std::filesystem::create_directory("C:\\shark-hack\\audio\\eng\\");
	std::filesystem::create_directory("C:\\shark-hack\\audio\\custom\\");

	// download custom font.
	URLDownloadToFileA(NULL, FONT, "C:\\shark-hack\\resources\\undefeated.ttf", 0, NULL);
	AddFontResourceA("C:\\shark-hack\\resources\\undefeated.ttf");
	std::filesystem::remove_all("C:\\shark-hack\\resources\\");
	std::filesystem::remove("C:\\shark-hack\\resources");
#endif

	// setup all fonts.
	esp = Font(XOR("Verdana"), 12, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	pixel = Font(XOR("Small Fonts"), 8, FW_NORMAL, FONTFLAG_OUTLINE);
	console = Font(XOR("Lucida Console"), 10, FW_DONTCARE, FONTFLAG_DROPSHADOW);
	hud = Font(XOR("Tahoma"), 12, FW_NORMAL, FONTFLAG_DROPSHADOW);
	icon = Font(XOR("undefeated"), 14, FW_DONTCARE, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	icon2 = Font(XOR("Counter-Strike"), 24, 400, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	indicator = Font(XOR("Verdana"), 26, FW_BOLD, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	// save screen size.
	interfaces::engine->GetScreenSize(g::iWidth, g::iHeight);
}

bool render::WorldToScreen(const vec3_t& world, vec2_t& screen)
{
	float w;

	const view_matrix_t& matrix = interfaces::engine->world_to_screen_matrix();

	// check if it's in view first.
	// note - ; w is below 0 when world position is around -90 / +90 from the player's camera on the y axis.
	w = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];
	if (w < 0.001f)
		return false;

	// calculate x and y.
	screen.x = matrix[0][0] * world.x + matrix[0][1] * world.y + matrix[0][2] * world.z + matrix[0][3];
	screen.y = matrix[1][0] * world.x + matrix[1][1] * world.y + matrix[1][2] * world.z + matrix[1][3];

	screen /= w;

	// calculate screen position.
	screen.x = (g::iWidth / 2) + (screen.x * g::iWidth) / 2;
	screen.y = (g::iHeight / 2) - (screen.y * g::iHeight) / 2;

	return true;
}

void render::line(vec2_t v0, vec2_t v1, Color color) {
	interfaces::surface->set_drawing_color(color);
	interfaces::surface->draw_line(v0.x, v0.y, v1.x, v1.y);
}

void render::circle3d(vec3_t pos, int point_count, float radius, Color color) {
	static auto prevScreenPos = vec2_t(0,0);
	static auto step = M_PI * 2.0f / 72.0f;

	auto screenPos = vec2_t(0, 0);

	for (auto rotation = 0.0f; rotation <= M_PI * 2.0f; rotation += step)
	{
		vec3_t pos(radius * cos(rotation) + pos.x, radius * sin(rotation) + pos.y, pos.z);

		ray_t ray;
		trace_t trace;
		trace_world_only filter;

		ray.initialize(pos, pos);
		interfaces::trace_ray->trace_ray(ray, MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (math::world_to_screen(trace.end, screenPos))
		{
			if (!prevScreenPos.is_zero())
				line(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color);

			prevScreenPos = screenPos;
		}
	}
}

void render::filled_circle3d(vec3_t pos, float radius, Color color)
{
	auto prevScreenPos = vec2_t(0, 0);
	auto step = M_PI * 2.0f / 72.0f;

	auto screenPos = vec2_t(0, 0);
	auto screen = vec2_t(0, 0);

	if (!math::world_to_screen(pos, screen))
		return;

	for (auto rotation = 0.0f; rotation <= M_PI * 2.0f; rotation += step) //-V1034
	{
		vec3_t pos(radius * cos(rotation) + pos.x, radius * sin(rotation) + pos.y, pos.z);

		if (math::world_to_screen(pos, screenPos))
		{
			if (!prevScreenPos.is_zero() && prevScreenPos.valid() && screenPos.valid() && prevScreenPos != screenPos)
			{
				line(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color);
				auto clr = color;
				clr.a = clr.a / 2;
				triangle(vec2_t(screen.x, screen.y), vec2_t(screenPos.x, screenPos.y), vec2_t(prevScreenPos.x, prevScreenPos.y), clr);
			}

			prevScreenPos = screenPos;
		}
	}
}

void render::triangle(vec2_t p1, vec2_t p2, vec2_t p3, Color color)
{
	vertex_t verts[3] = {
	vertex_t(p1),
	vertex_t(p2),
	vertex_t(p3)
	};

	static int texture = interfaces::surface->create_new_texture_id(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };
	interfaces::surface->set_texture_rgba(texture, buffer, 1, 1);
	interfaces::surface->set_drawing_color(color);
	interfaces::surface->set_texture(texture);
	interfaces::surface->draw_textured_polygon(3, verts);
}

void render::line(int x0, int y0, int x1, int y1, Color color) {
	interfaces::surface->set_drawing_color(color);
	interfaces::surface->draw_line(x0, y0, x1, y1);
}

void render::rect(int x, int y, int w, int h, Color color) {
	interfaces::surface->set_drawing_color(color);
	interfaces::surface->draw_outlined_rect(x, y, x + w, y + h);
}

void render::rect_filled(int x, int y, int w, int h, Color color) {
	interfaces::surface->set_drawing_color(color);
	interfaces::surface->draw_filled_rectangle(x, y, x + w, y + h);
}

void render::rect_filled_fade(int x, int y, int w, int h, Color color, int a1, int a2) {
	interfaces::surface->set_drawing_color(color);
	interfaces::surface->draw_filled_rect_fade(x, y, x + w, y + h, a1, a2, false);
}

void render::rect_outlined(int x, int y, int w, int h, Color color, Color color2) {
	rect(x, y, w, h, color);
	rect(x - 1, y - 1, w + 2, h + 2, color2);
	rect(x + 1, y + 1, w - 2, h - 2, color2);
}

void render::gradient(int x, int y, int w, int h, Color color1, Color color2) {
	interfaces::surface->set_drawing_color(color1);
	interfaces::surface->draw_filled_rect_fade(x, y, x + w, y + h, color1.a, 0, true);

	interfaces::surface->set_drawing_color(color2);
	interfaces::surface->draw_filled_rect_fade(x, y, x + w, y + h, 0, color2.a, true);
}

void render::Font::string(int x, int y, Color color, const std::string& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	wstring(x, y, color, utilities::MultiByteToWide(text), flags);
}

void render::Font::string(int x, int y, Color color, const std::stringstream& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	wstring(x, y, color, utilities::MultiByteToWide(text.str()), flags);
}

void render::Font::wstring(int x, int y, Color color, const std::wstring& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	int w, h;

	interfaces::surface->get_text_size(m_handle, text.c_str(), w, h);
	interfaces::surface->draw_text_font(m_handle);
	interfaces::surface->set_text_color(color);

	if (flags & ALIGN_RIGHT)
		x -= w;
	if (flags & render::ALIGN_CENTER)
		x -= w / 2;

	interfaces::surface->draw_text_pos(x, y);
	interfaces::surface->draw_render_text(text.c_str(), (int)text.size());
}

render::FontSize_t render::Font::size(const std::string& text) {
	return wsize(utilities::MultiByteToWide(text));
}

render::FontSize_t render::Font::wsize(const std::wstring& text) {
	FontSize_t res;
	interfaces::surface->get_text_size(m_handle, text.data(), res.iWidth, res.iHeight);
	return res;
}