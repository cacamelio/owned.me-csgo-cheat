#include "../features.hpp"

float indicators::add(const std::string& txt, Color clr)
{
	render::FontSize_t text_size = render::indicator.size(txt);

	text_size.iHeight += vertical_pandding * 2.f;

	const auto vec_text_size = vec2_t(text_size.iWidth, text_size.iHeight);

	auto vertical_offset = _Draw_queue.empty() ? g::vScreenSize.y - (g::vScreenSize.y - zone) / 2.f : std::get<float>(_Draw_queue.back()) - spacing - vec_text_size.y;

	_Draw_queue.emplace_back(std::make_tuple(txt, clr, vertical_offset, vec_text_size));

	return vertical_offset;
}

void indicators::draw()
{
	for (const auto& data : _Draw_queue) {
		const auto position = vec2_t(horizontal_offset, std::get<float>(data));
		const auto text_pos = position + vec2_t(text_offset, vertical_pandding);
		auto x1 = std::get<vec2_t>(data);
		auto x2 = vec2_t(.5f, 1.f);
		const auto half_size = vec2_t(x1.x * x2.x, x1.y * x2.y);
		render::gradient(position.x, position.y, half_size.x, half_size.y, Color(0, 0, 0, 0), Color(0, 0, 0, 170));
		auto next_pos = position + vec2_t(half_size.x, 0.f);
		render::gradient(next_pos.x, next_pos.y, half_size.x, half_size.y, Color(0, 0, 0, 170), Color(0, 0, 0, 0));
		auto text_posit = text_pos + 1.f;
		render::indicator.string(text_posit.x, text_posit.y, Color(0, 0, 0, .5f), std::get<std::string>(data));
		render::indicator.string(text_pos.x, text_pos.y, std::get<Color>(data), std::get<std::string>(data));

	}

	_Draw_queue.clear();
}

void indicators::list() {
	if (!g::pLocalPlayer)
		return;

	if (!config.indicators)
		return;

	if (config.enable_fakelag)
		add("LC", fakelag::break_lc ? Color(255, 0, 0) : Color(123, 194, 21));

	if (g_keyhandler.CheckKey(config.doubletap_key, config.doubletap_key_style))
		add("DT", exploit[dt].recharge ? Color(255, 0, 0) : Color(203, 203, 203));

	if (g_keyhandler.CheckKey(config.hideshot_key, config.hideshot_key_style))
		add("HS", Color(203, 203, 203));

	if(g_keyhandler.CheckKey(config.fakeduck_key, config.fakeduck_key_style))
		add("DUCK", Color(203, 203, 203));

	if (g_keyhandler.CheckKey(g_aimbot->rage_settings.min_dmg_key, g_aimbot->rage_settings.min_dmg_style) && g_aimbot->rage_settings.min_dmg_key)
		add("DMG", Color(123, 194, 21));

	for (int i = 1; i <= interfaces::entity_list->get_highest_index(); i++) {
		auto e = reinterpret_cast<BaseEntity*>(interfaces::entity_list->get_client_entity(i));

		if (!e)
			continue;
		if (e->IsPlayer())
			continue;
		if (e->Dormant())
			continue;
		auto clientclass = e->GetClientClass();
		if (!clientclass)
			continue;

		switch (clientclass->m_ClassID)
		{
		case 129:
			bomb(e);
		}
	}
}

float scaleDamageArmor(float flDamage, int armor_value)
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (armor_value > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(armor_value)) {
			flArmor = static_cast<float>(armor_value) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

void indicators::bomb(BaseEntity* bomb_ent) {

	if (!config.indicators)
		return;

	static auto bomb_time = interfaces::console->FindVar("mp_c4timer")->GetFloat();

	std::string result;

	bomb_ent->BombSite() == 0 ? result += "A Site - " : result += "B Site - ";

	result += std::to_string((int)(bomb_time));
	result += "s";

	add(result, Color(252, 243, 105));

	float flDistance = g::pLocalPlayer->GetEyePosition().distance_to(bomb_ent->GetEyePosition());
	float a = 450.7f;
	float b = 75.68f;
	float c = 789.2f;
	float d = ((flDistance - b) / c);
	float flDamage = a * exp(-d * d);
	float damage;
	char bombdamagestringdead[24];
	char bombdamagestringalive[24];
	damage = float((std::max)((int)ceilf(scaleDamageArmor(flDamage, g::pLocalPlayer->Armor())), 0));
	sprintf_s(bombdamagestringdead, sizeof(bombdamagestringdead) - 1, "");
	std::string dmg;
	Color clr;

	if (damage >= g::pLocalPlayer->Health())
	{
		dmg = "FATAL";
		clr = Color(255, 0, 0);
	}
	else
	{
		dmg += " HP - ";  dmg += std::to_string((int)(damage)); /*dmg += "HP";*/
		clr = Color(255, 255, 255);
	}

	add(dmg, clr);
}