#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include "localization/ChineseUI.h"
#include <cassert>
#include <cmath>
#include <cstring>
#include <string>
#include <cstdio>

static int language = 3;
extern "C" int32_t CVarGetInteger(const char*, int32_t) { return language; }

int main(int argc, char** argv) {
    assert(argc == 2);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(1280, 720);
    ImFont* base = io.Fonts->AddFontDefault();
    ImFontGlyphRangesBuilder builder;
    ChineseUI::AddTranslatedGlyphs(builder);
    ImVector<ImWchar> ranges;
    builder.BuildRanges(&ranges);
    ImFontConfig config;
    config.MergeMode = true;
    config.DstFont = base;
    assert(io.Fonts->AddFontFromFileTTF(argv[1], 16.0f, &config, ranges.Data));
    assert(io.Fonts->Build());
    for (int i = 0; i + 1 < ranges.Size && ranges[i]; i += 2)
        for (unsigned c = ranges[i]; c <= ranges[i + 1]; ++c)
            if (c >= 32 && !base->FindGlyphNoFallback(c)) {
                std::fprintf(stderr, "Missing glyph U+%04X\n", c);
                return 1;
            }
    assert(base->FindGlyphNoFallback('A'));
    ChineseUI::Initialize();
    assert(std::strcmp(ImGui::LocalizeText("Settings"), "设置") == 0);
    assert(std::strcmp(ImGui::LocalizeText("Settings##stable"), "设置##stable") == 0);
    assert(std::strcmp(ImGui::LocalizeText("Settings###stable"), "设置###stable") == 0);
    assert(std::strcmp(ImGui::LocalizeText("D:/user/Settings"), "D:/user/Settings") == 0);
    assert(std::strcmp(ImGui::LocalizeText("\xEF\x81\x94 Settings  "), "\xEF\x81\x94 设置  ") == 0);
    ImGui::NewFrame();
    ImGui::Begin("Settings###test");
    auto id = ImGui::GetID("Settings##stable");
    auto width = ImGui::CalcTextSize("Settings##stable", nullptr, true).x;
    assert(std::abs(width - ImGui::CalcTextSize("设置").x) < 0.01f);
    const char bounded[] = "SettingsUNKNOWN";
    assert(std::abs(ImGui::CalcTextSize(bounded, bounded + 8).x - width) < 0.01f);
    ImGui::Button("Settings##stable");
    ImGui::Text("Icon size : %dpx", 24);
    language = 0;
    assert(ImGui::GetID("Settings##stable") == id);
    assert(std::strcmp(ImGui::LocalizeText("Settings"), "Settings") == 0);
    language = 3;
    ImGui::End();
    ImGui::Render();
    ImGui::DestroyContext();
    std::puts("PASS: catalog lookup, suffix IDs, icons, bounded strings, formatting, font coverage, language switching");
}
