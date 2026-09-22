"""Find untranslated literal UI arguments. Dynamic labels require a separate visual review."""
from pathlib import Path
import ast
import re
import json
root = Path(__file__).resolve().parents[2]
translations = {}
for p in Path(__file__).parent.glob('ui*.tsv'):
    for line in p.read_text(encoding='utf-8').splitlines():
        if line and not line.startswith('#'):
            k, v = line.split('\t', 1)
            translations[k.replace('\\n','\n').replace('\\t','\t')] = v
literal = r'"(?:\\.|[^"\\])*"'
pattern = re.compile(r'(ImGui|UIWidgets)::(\w+)\(\s*(' + literal + r'(?:\s*' + literal + r')*)', re.S)
skip = {'BeginTable','PushID','GetID','SetClipboardText','OpenPopup',
        'SetWindowFocus','SetWindowPos','SetWindowSize','SetWindowCollapsed','SetItemDefaultFocus',
        'EnhancementCombobox','PaddedEnhancementCombobox','EnhancementColor','EnhancementColorWithReset'}
report = {}
total = translated = 0
for base in (root / 'soh/soh', root / 'libultraship/src'):
    for p in base.rglob('*.cpp'):
        if '/localization/' in p.as_posix(): continue
        s = p.read_text(encoding='utf-8-sig', errors='replace')
        # Strip comments to avoid counting examples as UI.
        s = re.sub(literal + r'|/\*.*?\*/|//[^\n]*',
                   lambda m: m[0] if m[0].startswith('"') else ' ', s, flags=re.S)
        for match in pattern.finditer(s):
            if match[2] in skip: continue
            try: text = ''.join(ast.literal_eval(x) for x in re.findall(literal, match[3]))
            except (ValueError, SyntaxError): continue
            text = text.split('##')[0]
            if not re.search('[A-Za-z]{2}', text) or text.startswith(('gEnhancements.', 'gCosmetics.')): continue
            total += 1
            if text in translations: translated += 1
            else: report.setdefault(text, []).append(str(p.relative_to(root)))
result = {'literal_occurrences': total, 'translated_occurrences': translated,
          'untranslated_unique': len(report), 'untranslated': report}
print(json.dumps(result, ensure_ascii=False, indent=2))
