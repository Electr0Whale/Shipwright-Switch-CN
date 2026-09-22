"""Audit option tables as well as literal UI calls without changing stored values."""
from pathlib import Path
import ast
import json
import re

root = Path(__file__).resolve().parents[2]
literal = r'"(?:\\.|[^"\\])*"'
catalog = set()
for p in Path(__file__).parent.glob('ui*.tsv'):
    for line in p.read_text(encoding='utf-8').splitlines():
        if line and not line.startswith('#'):
            catalog.add(line.split('\t')[0].replace('\\n', '\n').replace('\\t', '\t'))
missing = {}
for base in (root / 'soh/soh', root / 'libultraship/src'):
    for p in list(base.rglob('*.cpp')) + list(base.rglob('*.h')):
        if 'localization' in p.parts: continue
        source = p.read_text(encoding='utf-8-sig', errors='replace')
        source = re.sub(literal + r'|/\*.*?\*/|//[^\n]*',
                        lambda m: m[0] if m[0].startswith('"') else ' ', source, flags=re.S)
        groups = []
        # Static string option arrays, including enum-backed randomizer choices.
        groups += re.findall(r'(?:const\s+char\s*\*|std::(?:vector|array)\s*<\s*(?:std::string|const\s+char\s*\*)[^;=]*>)[^;=]*=\s*\{(.*?)\};', source, flags=re.S)
        # 3drando setting labels and options are also used in the Switch UI.
        groups += re.findall(r'Option::(?:Bool|U8)\s*\((.*?);', source, flags=re.S)
        groups += re.findall(r'COSMETIC_OPTION\(\s*' + literal + r'\s*,\s*(' + literal + ')', source)
        groups += re.findall(r'\{\s*COSMETICS_GROUP_\w+\s*,\s*(' + literal + ')', source)
        for group in groups:
            for token in re.findall(literal, group):
                try: value = ast.literal_eval(token)
                except (SyntaxError, ValueError): continue
                if not re.search('[A-Za-z]{2}', value) or value in catalog: continue
                if value.startswith(('__OTR__', 'gEnhancements.', 'gCosmetics.', 'gRandomizer.')): continue
                missing.setdefault(value, []).append(str(p.relative_to(root)))
print(json.dumps({'untranslated_unique': len(missing), 'untranslated': missing}, ensure_ascii=False, indent=2))
