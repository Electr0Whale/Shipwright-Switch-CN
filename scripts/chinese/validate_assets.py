"""Validate generated game messages, glyph paths, and Chinese texture references."""
from pathlib import Path
import re
import json
root = Path(__file__).resolve().parents[2]
source = (root / 'soh/soh/z_message_CHI.cpp').read_text()
messages = re.findall(r'static const u8 sCHIMsgData_(0x[0-9A-Fa-f]+)\[\] = \{([^}]+)\};', source)
params = {5:1,6:1,7:2,12:1,14:1,17:2,18:2,19:1,20:1,21:3,30:1}
max_raw = max_page = 0
errors = []
codes = set()
for name, raw in messages:
    data = bytes(int(x, 16) for x in re.findall(r'0x([0-9A-Fa-f]+)', raw))
    max_raw = max(max_raw, len(data))
    if len(data) > 1280: errors.append(f'{name}: {len(data)} raw bytes exceed 1280')
    i = page = 0
    while i < len(data):
        ch = data[i]
        i += 1
        page += 1
        if ch >= 0xA0:
            if i == len(data):
                errors.append(f'{name}: truncated CJK pair')
                break
            codes.add((ch << 8) | data[i])
            i += 1
        elif ch in params:
            count = params[ch]
            if i + count > len(data): errors.append(f'{name}: truncated control {ch:02X}')
            i += count
            page += count
        elif ch in (15,16,22,23,24,25,29,31):
            # NAME and number substitutions can expand at runtime.
            page += 8
        max_page = max(max_page, page)
        if page >= 200: errors.append(f'{name}: conservative decoded page size {page} exceeds 199')
        if ch < 0xA0 and ch in (2,4,7,11,12): page = 0
    if data[-1] != 2: errors.append(f'{name}: no final END')
table = (root / 'soh/src/code/z_kanfont_chinese_tbl.inc').read_text()
glyph_names = re.findall(r'^\s*(gMsgCharChn\w+),', table, re.M)
if len(glyph_names) != 3005: errors.append(f'Unexpected glyph table size: {len(glyph_names)}')
for code in codes:
    if 0xAA9F <= code <= 0xAAAB: continue  # N64 button icons
    index = code - 0xA08C
    if not 0 <= index < len(glyph_names) or glyph_names[index] == 'gMsgCharChnEmptyTex':
        errors.append(f'Missing glyph for code {code:04X}')
if len({name for name, _ in messages}) != len(messages): errors.append('Duplicate message ID')
headers = [(root / 'soh/assets/textures/chinese_font/chinese_font.h'),
           (root / 'soh/assets/textures/chinese_textures.h')]
paths = set()
for header in headers:
    for path in re.findall(r'"__OTR__(.*?)"', header.read_text()):
        paths.add(path)
        base = root / 'soh/assets/custom' / path
        if not base.exists() and not list(base.parent.glob(base.name + '.*')):
            errors.append(f'Missing asset: {path}')

# The USA Rev 2 NES table has no PAL-only 0xFFFC entry.  The title and file
# select screens in the Switch baseline still load that ordered font, so keep
# the explicit null-safe ASCII fallback as a packaging invariant.
font_source = (root / 'soh/src/code/z_kanfont.c').read_text()
ordered_font_fragments = (
    '(_message_0xFFFC_nes != NULL)',
    '"0123456789\\x01"',
    '"ABCDEFGHIJKLMN\\x01"',
    '"OPQRSTUVWXYZ\\x01"',
    '"abcdefghijklmn\\x01"',
    '"opqrstuvwxyz\\x01"',
    '" -.\\x01"',
    '"\\x02"',
)
if any(fragment not in font_source for fragment in ordered_font_fragments):
    errors.append('Missing NTSC ordered-font fallback or null guard')

# gDPLoadTextureBlock consumes pixels.  A resource path string in either font
# buffer renders as striped blocks and can corrupt the initial message draw.
raw_font_fragments = (
    'static void Font_CopyTexture',
    'ResourceMgr_LoadTexOrDListByName(resourceName)',
    'memcpy(destination, texture, FONT_CHAR_TEX_SIZE)',
    'Font_CopyTexture(&font->charTexBuf',
    'Font_CopyTexture(fontBuf, fntTbl',
)
if any(fragment not in font_source for fragment in raw_font_fragments):
    errors.append('Font texture loaders must copy resolved texture pixels')

# The NTSC 1.2 Link's House intro starts with generic command 0x3D.  The
# exporter already serializes these commands as 12-word payloads; the importer
# must consume the same command set or it will stop at an embedded 0xFFFFFFFF
# and leave Cutscene_ProcessCommands with a truncated vector.
cutscene_factory = (root / 'soh/soh/resource/importer/CutsceneFactory.cpp').read_text()
generic_cutscene_cases = (
    'case 0x0B:', 'case 0x0D:', 'case 0x15:', 'case 0x16:',
    'case 0x1B:', 'case 0x1C:', 'case 0x20:', 'case 0x21:',
    'case 0x3B:', 'case 0x3D:', 'case 0x47:', 'case 0x49:',
    'case 0x4A:', 'case 0x6D:', 'case 0x70:', 'case 0x71:',
)
cutscene_importer_ok = all(fragment in cutscene_factory for fragment in generic_cutscene_cases)
if not cutscene_importer_ok:
    errors.append('Cutscene importer is missing an exporter generic-command case')
if 'case 74:' in cutscene_factory:
    errors.append('Cutscene command 0x4A must use the generic-command parser')

result = {'messages': len(messages), 'unique_codes': len(codes), 'max_raw_bytes': max_raw,
          'max_conservative_decoded_page': max_page, 'referenced_assets': len(paths),
          'ntsc_ordered_font_fallback': 'PASS' if not any(
              fragment not in font_source for fragment in ordered_font_fragments) else 'FAIL',
          'resolved_font_texture_loaders': 'PASS' if not any(
              fragment not in font_source for fragment in raw_font_fragments) else 'FAIL',
          'ntsc_cutscene_generic_commands': 'PASS' if cutscene_importer_ok else 'FAIL',
          'errors': sorted(set(errors))}
print(json.dumps(result, indent=2, ensure_ascii=False))
raise SystemExit(bool(errors))
