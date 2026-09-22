#pragma once
struct ImFont;
struct ImFontGlyphRangesBuilder;
namespace ChineseUI {
void Initialize();
void AddTranslatedGlyphs(ImFontGlyphRangesBuilder& builder);
void MergeFont(ImFont* destination, float size);
}
