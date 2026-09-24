#include "global.h"

#include <string.h>

#include "message_data_static.h"
#include "textures/nes_font_static/nes_font_static.h"
#include "textures/message_static/message_static.h"
// SOH [Chinese] - custom CJK glyph textures (16x16 I4), shipped in soh.otr
#include "textures/chinese_font/chinese_font.h"

extern s32 ResourceMgr_CopyFontTexture(void* destination, const char* filePath, size_t size);

// SOH [Chinese] - Chinese message table, populated by OTRMessage_InitChinese()
// (introduced in a later upstream revision, backported here for the port).
extern MessageTableEntry* sChiMessageEntryTablePtr;

static const char* fntTbl[] =
{
    gMsgChar20SpaceTex,
    gMsgChar21ExclamationMarkTex,
    gMsgChar22QuotationMarkTex,
    gMsgChar23NumberSignTex,
    gMsgChar24DollarSignTex,
    gMsgChar25PercentSignTex,
    gMsgChar26AmpersandTex,
    gMsgChar27ApostropheTex,
    gMsgChar28LeftParenthesesTex,
    gMsgChar29RightParenthesesTex,
    gMsgChar2AAsteriskTex,
    gMsgChar2BPlusSignTex,
    gMsgChar2CCommaTex,
    gMsgChar2DHyphenMinusTex,
    gMsgChar2EFullStopTex,
    gMsgChar2FSolidusTex,
    gMsgChar30Digit0Tex,
    gMsgChar31Digit1Tex,
    gMsgChar32Digit2Tex,
    gMsgChar33Digit3Tex,
    gMsgChar34Digit4Tex,
    gMsgChar35Digit5Tex,
    gMsgChar36Digit6Tex,
    gMsgChar37Digit7Tex,
    gMsgChar38Digit8Tex,
    gMsgChar39Digit9Tex,
    gMsgChar3AColonTex,
    gMsgChar3BSemicolonTex,
    gMsgChar3CLessThanSignTex,
    gMsgChar3DEqualsSignTex,
    gMsgChar3EGreaterThanSignTex,
    gMsgChar3FQuestionMarkTex,
    gMsgChar40CommercialAtTex,
    gMsgChar41LatinCapitalLetterATex,
    gMsgChar42LatinCapitalLetterBTex,
    gMsgChar43LatinCapitalLetterCTex,
    gMsgChar44LatinCapitalLetterDTex,
    gMsgChar45LatinCapitalLetterETex,
    gMsgChar46LatinCapitalLetterFTex,
    gMsgChar47LatinCapitalLetterGTex,
    gMsgChar48LatinCapitalLetterHTex,
    gMsgChar49LatinCapitalLetterITex,
    gMsgChar4ALatinCapitalLetterJTex,
    gMsgChar4BLatinCapitalLetterKTex,
    gMsgChar4CLatinCapitalLetterLTex,
    gMsgChar4DLatinCapitalLetterMTex,
    gMsgChar4ELatinCapitalLetterNTex,
    gMsgChar4FLatinCapitalLetterOTex,
    gMsgChar50LatinCapitalLetterPTex,
    gMsgChar51LatinCapitalLetterQTex,
    gMsgChar52LatinCapitalLetterRTex,
    gMsgChar53LatinCapitalLetterSTex,
    gMsgChar54LatinCapitalLetterTTex,
    gMsgChar55LatinCapitalLetterUTex,
    gMsgChar56LatinCapitalLetterVTex,
    gMsgChar57LatinCapitalLetterWTex,
    gMsgChar58LatinCapitalLetterXTex,
    gMsgChar59LatinCapitalLetterYTex,
    gMsgChar5ALatinCapitalLetterZTex,
    gMsgChar5BLeftSquareBracketTex,
    gMsgChar5CYenSignTex,
    gMsgChar5DRightSquareBracketTex,
    gMsgChar5ECircumflexAccentTex,
    gMsgChar5FLowLineTex,
    gMsgChar60GraveAccentTex,
    gMsgChar61LatinSmallLetterATex,
    gMsgChar62LatinSmallLetterBTex,
    gMsgChar63LatinSmallLetterCTex,
    gMsgChar64LatinSmallLetterDTex,
    gMsgChar65LatinSmallLetterETex,
    gMsgChar66LatinSmallLetterFTex,
    gMsgChar67LatinSmallLetterGTex,
    gMsgChar68LatinSmallLetterHTex,
    gMsgChar69LatinSmallLetterITex,
    gMsgChar6ALatinSmallLetterJTex,
    gMsgChar6BLatinSmallLetterKTex,
    gMsgChar6CLatinSmallLetterLTex,
    gMsgChar6DLatinSmallLetterMTex,
    gMsgChar6ELatinSmallLetterNTex,
    gMsgChar6FLatinSmallLetterOTex,
    gMsgChar70LatinSmallLetterPTex,
    gMsgChar71LatinSmallLetterQTex,
    gMsgChar72LatinSmallLetterRTex,
    gMsgChar73LatinSmallLetterSTex,
    gMsgChar74LatinSmallLetterTTex,
    gMsgChar75LatinSmallLetterUTex,
    gMsgChar76LatinSmallLetterVTex,
    gMsgChar77LatinSmallLetterWTex,
    gMsgChar78LatinSmallLetterXTex,
    gMsgChar79LatinSmallLetterYTex,
    gMsgChar7ALatinSmallLetterZTex,
    gMsgChar7BLeftCurlyBracketTex,
    gMsgChar7CVerticalLineTex,
    gMsgChar7DRightCurlyBracketTex,
    gMsgChar7ETildeTex,
    gMsgChar7FBlankTex,
    gMsgChar80LatinCapitalLetterAWithGraveTex,
    gMsgChar81LatinCapitalLetterIWithCircumflexTex,
    gMsgChar82LatinCapitalLetterAWithCircumflexTex,
    gMsgChar83LatinCapitalLetterAWithDiaeresisTex,
    gMsgChar84LatinCapitalLetterCWithCedillaTex,
    gMsgChar85LatinCapitalLetterEWithGraveTex,
    gMsgChar86LatinCapitalLetterEWithAcuteTex,
    gMsgChar87LatinCapitalLetterEWithCircumflexTex,
    gMsgChar88LatinCapitalLetterEWithDiaeresisTex,
    gMsgChar89LatinCapitalLetterIWithDiaeresisTex,
    gMsgChar8ALatinCapitalLetterOWithCircumflexTex,
    gMsgChar8BLatinCapitalLetterOWithDiaeresisTex,
    gMsgChar8CLatinCapitalLetterUWithGraveTex,
    gMsgChar8DLatinCapitalLetterUWithCircumflexTex,
    gMsgChar8ELatinCapitalLetterUWithDiaeresisTex,
    gMsgChar8FLatinSmallLetterSharpSTex,
    gMsgChar90LatinSmallLetterAWithGraveTex,
    gMsgChar91LatinSmallLetterAWithAcuteTex,
    gMsgChar92LatinSmallLetterAWithCircumflexTex,
    gMsgChar93LatinSmallLetterAWithDiaeresisTex,
    gMsgChar94LatinSmallLetterCWithCedillaTex,
    gMsgChar95LatinSmallLetterEWithGraveTex,
    gMsgChar96LatinSmallLetterEWithAcuteTex,
    gMsgChar97LatinSmallLetterEWithCircumflexTex,
    gMsgChar98LatinSmallLetterEWithDiaeresisTex,
    gMsgChar99LatinSmallLetterIWithDiaeresisTex,
    gMsgChar9ALatinSmallLetterOWithCircumflexTex,
    gMsgChar9BLatinSmallLetterOWithDiaeresisTex,
    gMsgChar9CLatinSmallLetterUWithGraveTex,
    gMsgChar9DLatinSmallLetterUWithCircumflexTex,
    gMsgChar9ELatinSmallLetterUWithDiaeresisTex,
    gMsgChar9FButtonATex,
    gMsgCharA0ButtonBTex,
    gMsgCharA1ButtonCTex,
    gMsgCharA2ButtonLTex,
    gMsgCharA3ButtonRTex,
    gMsgCharA4ButtonZTex,
    gMsgCharA5ButtonCUpTex,
    gMsgCharA6ButtonCDownTex,
    gMsgCharA7ButtonCLeftTex,
    gMsgCharA8ButtonCRightTex,
    gMsgCharA9ZTargetSignTex,
    gMsgCharAAControlStickTex,
    gMsgCharABControlPadTex,
};

const char* msgStaticTbl[] =
{
    gDefaultMessageBackgroundTex,
    gSignMessageBackgroundTex,
    gNoteStaffMessageBackgroundTex,
    gFadingMessageBackgroundTex,
    gMessageContinueTriangleTex,
    gMessageEndSquareTex,
    gMessageArrowTex
};

// The fixed font buffers always hold the original 16x16 I4 pixels. The
// message renderer can additionally use these paths to draw alternate CJK
// textures directly, preserving their high-resolution metadata.
static const char* sChineseGlyphPaths[FONT_CHAR_MULTIPLIER];

static void Font_SetChineseGlyphPath(u16 codePointIndex, const char* path) {
    if (codePointIndex % FONT_CHAR_TEX_SIZE == 0 && codePointIndex / FONT_CHAR_TEX_SIZE < FONT_CHAR_MULTIPLIER) {
        sChineseGlyphPaths[codePointIndex / FONT_CHAR_TEX_SIZE] = path;
    }
}

const char* Font_GetChineseGlyphPath(u16 codePointIndex) {
    if (codePointIndex % FONT_CHAR_TEX_SIZE != 0 || codePointIndex / FONT_CHAR_TEX_SIZE >= FONT_CHAR_MULTIPLIER) {
        return NULL;
    }
    return sChineseGlyphPaths[codePointIndex / FONT_CHAR_TEX_SIZE];
}

static void Font_CopyTexture(void* destination, const char* resourceName) {
    if (!ResourceMgr_CopyFontTexture(destination, resourceName, FONT_CHAR_TEX_SIZE)) {
        memset(destination, 0, FONT_CHAR_TEX_SIZE);
    }
}

// SOH [Chinese] - All-CharChn glyph lookup table (codes 0xA08C..0xAC48)
#include "z_kanfont_chinese_tbl.inc"

void func_8006EE50(Font* font, u16 arg1, u16 arg2) {
}

/**
 * Loads a texture from nes_font_static for the requested `character` into the character texture buffer
 * at `codePointIndex`. The value of `character` is the ASCII codepoint subtract ' '/0x20.
 */
void Font_LoadChar(Font* font, u8 character, u16 codePointIndex) {
    //DmaMgr_SendRequest1(&font->charTexBuf[codePointIndex],
                        //&_nes_font_staticSegmentRomStart[character * FONT_CHAR_TEX_SIZE], FONT_CHAR_TEX_SIZE,
                        //__FILE__, __LINE__);

    Font_SetChineseGlyphPath(codePointIndex, NULL);
    if (character < ARRAY_COUNT(fntTbl) && codePointIndex <= sizeof(font->charTexBuf) - FONT_CHAR_TEX_SIZE) {
        Font_CopyTexture(&font->charTexBuf[codePointIndex], fntTbl[character]);
    }
}

// #region SOH [Chinese] - Load Chinese character glyph via OTR path
/**
 * Loads a Chinese character glyph into the character texture buffer at `codePointIndex`.
 * `character` is the 2-byte iQue encoding:
 *   0xA08C–0xA775  main CJK range
 *   0xAA9F–0xAAAB  button / icon codes (redirected to NES font textures below)
 *   0xAAAC–0xAC48  extended CJK range
 *
 * The glyph table is contiguous: glyphIndex = character - 0xA08C. Undefined codes and
 * button codes resolve to a blank texture. Note that this mirrors upstream
 * Shipwright-CN's All-CharChn design so the generated .inc stays drop-in compatible.
 */
void Font_LoadCharChinese(Font* font, u16 character, u16 codePointIndex) {
    if (codePointIndex > sizeof(font->charTexBuf) - FONT_CHAR_TEX_SIZE) {
        return;
    }
    Font_SetChineseGlyphPath(codePointIndex, NULL);

    // iQue button/icon codes (0xAA9F–0xAAAB) — map to the NES font textures the
    // N64/American builds already use for the same icons.
    if (character >= 0xAA9F && character <= 0xAAAB) {
        static const char* buttonIconTbl[] = {
            gMsgChar9FButtonATex,      // 0xAA9F
            gMsgCharA0ButtonBTex,      // 0xAAA0
            gMsgCharA1ButtonCTex,      // 0xAAA1
            gMsgCharA2ButtonLTex,      // 0xAAA2
            gMsgCharA3ButtonRTex,      // 0xAAA3
            gMsgCharA4ButtonZTex,      // 0xAAA4
            gMsgCharA5ButtonCUpTex,    // 0xAAA5
            gMsgCharA6ButtonCDownTex,  // 0xAAA6
            gMsgCharA7ButtonCLeftTex,  // 0xAAA7
            gMsgCharA8ButtonCRightTex, // 0xAAA8
            gMsgCharA9ZTargetSignTex,  // 0xAAA9
            gMsgCharAAControlStickTex, // 0xAAAA
            gMsgCharABControlPadTex,   // 0xAAAB
        };
        s32 btnIndex = character - 0xAA9F;
        Font_CopyTexture(&font->charTexBuf[codePointIndex], buttonIconTbl[btnIndex]);
        return;
    }

    Font_LoadChar(font, '?' - ' ', codePointIndex);
    // Contiguous table index: glyphIndex = code – 0xA08C
    s32 glyphIndex = character - 0xA08C;
    if (glyphIndex >= 0 && glyphIndex < ARRAY_COUNT(chineseFontTbl)) {
        Font_CopyTexture(&font->charTexBuf[codePointIndex], chineseFontTbl[glyphIndex]);
        Font_SetChineseGlyphPath(codePointIndex, chineseFontTbl[glyphIndex]);
    }
}
// #endregion

void* Font_FetchCharTexture(u8 character) {
    return fntTbl[character];
}

/**
 * Loads a message box icon from message_static, such as the ending triangle/square or choice arrow into the
 * icon buffer.
 * The different icons are given in the MessageBoxIcon enum.
 */
void Font_LoadMessageBoxIcon(Font* font, u16 icon) {
    Font_CopyTexture(font->iconBuf, msgStaticTbl[4 + icon]);
}

/**
 * Loads a full set of character textures based on their ordering in the message with text id 0xFFFC into
 * the font buffer.
 */
void Font_LoadOrderedFont(Font* font) {
    // NTSC ROMs do not contain the PAL-only 0xFFFC NES message.  The ordered
    // font is nevertheless used by the title and file-select code in this
    // branch, so reproduce the PAL message's ASCII ordering when it is absent.
    // See assets/text/message_data.h in the OoT decompilation.
    static const char sOrderedFontFallback[] =
        "0123456789\x01"
        "ABCDEFGHIJKLMN\x01"
        "OPQRSTUVWXYZ\x01"
        "abcdefghijklmn\x01"
        "opqrstuvwxyz\x01"
        " -.\x01"
        "\x02";
    size_t len;
    size_t jj;
    u8* fontBuf;
    s32 codePointIndex;
    s32 fontBufIndex;
    s32 offset;

    const char* orderedFontMessage =
        (_message_0xFFFC_nes != NULL) ? _message_0xFFFC_nes : sOrderedFontFallback;

    len = strlen(orderedFontMessage);
    if (len >= sizeof(font->msgBuf)) {
        len = sizeof(font->msgBuf) - 1;
    }
    memcpy(font->msgBuf, orderedFontMessage, len);
    font->msgBuf[len] = MESSAGE_END;

    osSyncPrintf("msg_data=%x,  msg_data0=%x   jj=%x\n", font->msgOffset, font->msgLength, jj = len);

    len = jj;
    for (fontBufIndex = 0, codePointIndex = 0; font->msgBuf[codePointIndex] != MESSAGE_END; codePointIndex++) {
        if ((size_t)codePointIndex > len) {
            osSyncPrintf("ＥＲＲＯＲ！！  エラー！！！  error───！！！！\n");
            return;
        }

        if (font->msgBuf[codePointIndex] != MESSAGE_NEWLINE) {
            fontBuf = font->fontBuf + fontBufIndex * 8;

            osSyncPrintf("nes_mes_buf[%d]=%d\n", codePointIndex, font->msgBuf[codePointIndex]);

            offset = (font->msgBuf[codePointIndex] - '\x20') * FONT_CHAR_TEX_SIZE;
            if (offset < 0 || offset / FONT_CHAR_TEX_SIZE >= ARRAY_COUNT(fntTbl) ||
                (size_t)fontBufIndex * 8 > sizeof(font->fontBuf) - FONT_CHAR_TEX_SIZE) {
                osSyncPrintf("Font_LoadOrderedFont: invalid glyph or destination index\n");
                return;
            }
            Font_CopyTexture(fontBuf, fntTbl[offset / FONT_CHAR_TEX_SIZE]);
            //DmaMgr_SendRequest1(fontBuf, fontStatic + offset, FONT_CHAR_TEX_SIZE, __FILE__, __LINE__);
            fontBufIndex += FONT_CHAR_TEX_SIZE / 8;
        }
    }
}
