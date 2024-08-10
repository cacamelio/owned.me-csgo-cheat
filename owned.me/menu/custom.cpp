#include "custom.h"

#pragma warning (disable : 4244) // O_o

struct tab_anim {
    float active_text_anim, active_outline_alpha, active_rect_alpha, active_line_alpha;
};

bool p_interface::tab(const char* label, bool selected)
{
    ImGuiWindow* window = ui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ui::CalcTextSize(label, NULL, true);
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + label_size.x + 11, pos.y + 19));
    ui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x, rect.Max.y), style.FramePadding.y);
    if (!ui::ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ui::ButtonBehavior(rect, id, &hovered, &held, NULL);

    static std::map <ImGuiID, tab_anim> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end())
    {
        anim.insert({ id, { 0.0f, 0.0f, 0.0f, 0.0f } });
        it_anim = anim.find(id);
    }

    it_anim->second.active_text_anim = ImLerp(it_anim->second.active_text_anim, (selected ? 0.52f : 0.0f), 0.1f * (1.0f - ui::GetIO().DeltaTime));
    it_anim->second.active_outline_alpha = ImLerp(it_anim->second.active_outline_alpha, (selected ? 0.5f : 0.0f), 0.1f * (1.0f - ui::GetIO().DeltaTime));
    it_anim->second.active_rect_alpha = ImLerp(it_anim->second.active_rect_alpha, (selected ? 1.0f : 0.0f), 0.1f * (1.0f - ui::GetIO().DeltaTime));
    it_anim->second.active_line_alpha = ImLerp(it_anim->second.active_line_alpha, (selected ? 0.1f : 0.0f), 0.06f * (1.0f - ui::GetIO().DeltaTime));

    window->DrawList->AddRectFilled(rect.Min, rect.Max, ImColor(15 / 255.0f, 15 / 255.0f, 15 / 255.0f, g.Style.Alpha * it_anim->second.active_rect_alpha), 2.0f, ImDrawCornerFlags_Top);
    window->DrawList->AddRect(rect.Min, rect.Max, ImColor(146 / 255.0f, 157 / 255.0f, 223 / 255.0f, selected ? g.Style.Alpha * it_anim->second.active_outline_alpha : 0.0f), 2.0f, ImDrawCornerFlags_Top);

    if (selected) {
        window->DrawList->AddRectFilled(ImVec2(rect.Min.x, rect.Min.y + 5), rect.Max, ImColor(15 / 255.0f, 15 / 255.0f, 15 / 255.0f, g.Style.Alpha));
        window->DrawList->AddLine(ImVec2(rect.Min.x, rect.Min.y + 3), ImVec2(rect.Min.x, rect.Max.y - 2), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * it_anim->second.active_line_alpha));
        window->DrawList->AddLine(ImVec2(rect.Max.x - 1, rect.Min.y + 3), ImVec2(rect.Max.x - 1, rect.Max.y - 2), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * it_anim->second.active_line_alpha));
    }

    window->DrawList->AddText(ImVec2((rect.Min.x + rect.Max.x) / 2.f - (label_size.x / 2.f) + 1, (rect.Min.y + rect.Max.y) / 2.f - (label_size.y / 2.f) - 2), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.4f), label);
    window->DrawList->AddText(ImVec2((rect.Min.x + rect.Max.x) / 2.f - (label_size.x / 2.f) + 1, (rect.Min.y + rect.Max.y) / 2.f - (label_size.y / 2.f) - 2), ImColor(146 / 255.0f, 157 / 255.0f, 223 / 255.0f, g.Style.Alpha * it_anim->second.active_text_anim), label);

    return pressed;
}

struct subtab_anim {
    float active_text_anim, active_gradient_alpha, active_rect_alpha;
};

bool p_interface::subtab(const char* label, bool selected)
{
    ImGuiWindow* window = ui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ui::CalcTextSize(label, NULL, true);
    ImVec2 pos = window->DC.CursorPos;

    const ImRect rect(pos, ImVec2(pos.x + 101, pos.y + 18));
    ui::ItemSize(ImVec4(rect.Min.x, rect.Min.y, rect.Max.x + 2.f, rect.Max.y), style.FramePadding.y);
    if (!ui::ItemAdd(rect, id))
        return false;

    bool hovered, held;
    bool pressed = ui::ButtonBehavior(rect, id, &hovered, &held, NULL);

    static std::map <ImGuiID, subtab_anim> anim;
    auto it_anim = anim.find(id);
    if (it_anim == anim.end())
    {
        anim.insert({ id, { 0.0f, 0.0f, 0.0f } });
        it_anim = anim.find(id);
    }

    it_anim->second.active_text_anim = ImLerp(it_anim->second.active_text_anim, (selected ? 0.52f : 0.0f), 0.1f * (1.0f - ui::GetIO().DeltaTime));
    it_anim->second.active_gradient_alpha = ImLerp(it_anim->second.active_gradient_alpha, (selected ? 0.08f : 0.0f), 0.03f * (1.0f - ui::GetIO().DeltaTime));
    it_anim->second.active_rect_alpha = ImLerp(it_anim->second.active_rect_alpha, (selected ? 0.09f : 0.0f), 0.08f * (1.0f - ui::GetIO().DeltaTime));

    window->DrawList->AddRectFilledMultiColor(rect.Min, rect.Max, ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.08f + it_anim->second.active_gradient_alpha), ImColor(1.0f, 1.0f, 1.0f, 0.0f), ImColor(1.0f, 1.0f, 1.0f, 0.0f), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.08f + it_anim->second.active_gradient_alpha));
    window->DrawList->AddRectFilled(ImVec2(rect.Min.x, rect.Min.y), ImVec2(rect.Min.x + 2, rect.Max.y), ImColor(146 / 255.0f, 157 / 255.0f, 223 / 255.0f, g.Style.Alpha * 0.45f + it_anim->second.active_rect_alpha));
    window->DrawList->AddText(ImVec2((rect.Min.x + 7), (rect.Min.y + rect.Max.y) / 2.f - (label_size.y / 2.f) - 1), ImColor(1.0f, 1.0f, 1.0f, g.Style.Alpha * 0.4f), label);
    window->DrawList->AddText(ImVec2((rect.Min.x + 7), (rect.Min.y + rect.Max.y) / 2.f - (label_size.y / 2.f) - 1), ImColor(146 / 255.0f, 157 / 255.0f, 223 / 255.0f, g.Style.Alpha * it_anim->second.active_text_anim), label);

    return pressed;
}
