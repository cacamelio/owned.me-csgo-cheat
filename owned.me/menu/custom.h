#pragma once

#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>

#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

using namespace ui;

namespace p_interface {
    bool tab(const char* label, bool selected);
    bool subtab(const char* label, bool selected);
}
