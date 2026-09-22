#include "ChineseUI.h"
#include <libultraship/libultraship.h>
#include <ImGui/imgui.h>
#include <string>

namespace ChineseUI {
void MergeFont(ImFont* destination, float size) {
    ImFontConfig config;
    config.MergeMode = true;
    config.DstFont = destination;
    config.OversampleH = config.OversampleV = 1;
    config.PixelSnapH = true;
    static ImVector<ImWchar> ranges;
    if (ranges.empty()) {
        ImFontGlyphRangesBuilder builder;
        AddTranslatedGlyphs(builder);
        builder.BuildRanges(&ranges);
    }
    const std::string path = Ship::Context::GetPathRelativeToAppDirectory("fonts/ChineseUI.otf");
    ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size, &config, ranges.Data);
}
}
