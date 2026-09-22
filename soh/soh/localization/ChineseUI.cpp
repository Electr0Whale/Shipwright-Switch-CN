#include "ChineseUI.h"
#include "public/bridge/consolevariablebridge.h"
#include <ImGui/imgui.h>
#include <array>
#include <cstring>
#include <string>
#include <unordered_map>

namespace ChineseUI {
static const std::unordered_map<std::string, std::string> translations = {
#include "ChineseUI.inc"
};

// Called only for presentation. ImGui hashes the original labels, so language
// changes cannot change widget identity, keyboard focus or saved window state.
static void TranslateRange(const char** begin, const char** end) {
    if (!*begin || CVarGetInteger("gLanguages", 3) != 3) return;
    const char* finish = *end ? *end : *begin + strlen(*begin);
    std::string original(*begin, finish);
    const auto marker = original.find("##");
    const std::string visible = original.substr(0, marker);
    size_t first = 0, last = visible.size();
    // Window menu entries decorate the unchanged label with a FontAwesome icon
    // and padding. Translate only the label, retaining those decorations.
    if (visible.size() >= 3 && static_cast<unsigned char>(visible[0]) == 0xEF &&
        (static_cast<unsigned char>(visible[1]) & 0xC0) == 0x80 &&
        (static_cast<unsigned char>(visible[2]) & 0xC0) == 0x80) first = 3;
    while (first < last && visible[first] == ' ') ++first;
    while (last > first && visible[last - 1] == ' ') --last;
    auto found = translations.find(visible);
    if (found != translations.end()) { first = 0; last = visible.size(); }
    else found = translations.find(visible.substr(first, last - first));
    if (found == translations.end()) return;
    if (marker == std::string::npos && first == 0 && last == visible.size()) {
        *begin = found->second.c_str();
        *end = *begin + found->second.size();
    } else {
        // A small ring keeps nested measurement/render calls independent.
        static thread_local std::array<std::string, 32> scratch;
        static thread_local size_t next = 0;
        auto& value = scratch[next++ % scratch.size()];
        value = visible.substr(0, first) + found->second + visible.substr(last);
        if (marker != std::string::npos) value += original.substr(marker);
        *begin = value.c_str();
        *end = *begin + value.size();
    }
}

void AddTranslatedGlyphs(ImFontGlyphRangesBuilder& builder) {
    for (const auto& entry : translations) builder.AddText(entry.second.c_str());
}

void Initialize() {
    ImGui::SetTextLocalizationCallback(TranslateRange);
}

}
