#include "ChineseUI.h"
#include <libultraship/libultraship.h>
#include <ImGui/imgui.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <array>
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
    // Switch homebrew loaders do not agree on the guest current directory:
    // some start at sdmc:/ and others at sdmc:/switch/soh. Try both layouts
    // so the packaged font is found on hardware and in Eden.
    const std::array<std::string, 4> candidates = {
        Ship::Context::GetPathRelativeToAppDirectory("fonts/ChineseUI.otf"),
        Ship::Context::GetPathRelativeToAppDirectory("switch/soh/fonts/ChineseUI.otf"),
        "fonts/ChineseUI.otf",
        "switch/soh/fonts/ChineseUI.otf",
    };
    std::string path = candidates.front();
    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate)) {
            path = candidate;
            break;
        }
    }
    if (ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size, &config, ranges.Data) == nullptr) {
        SPDLOG_ERROR("Could not load Chinese UI font from {}", path);
    }
}
}
