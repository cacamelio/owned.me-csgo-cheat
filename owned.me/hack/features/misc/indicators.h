#pragma once

class indicators {
public:
    float add(const std::string& txt, Color clr);
    void draw();
    void list();
    void bomb(BaseEntity* bomb_ent);
private:
    static constexpr float zone = 380.f, spacing = 4.f, vertical_pandding = 2.f, horizontal_offset = 10.f, text_offset = 10.f;

    std::vector<std::tuple<std::string, Color,
        float /* Vertical offset */, vec2_t /* Text size */>> _Draw_queue{};

};

inline indicators* g_indicators = new indicators();