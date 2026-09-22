"""One-time, explicit backport of NTSC 1.2 asset descriptions from the pinned CN tree."""
from pathlib import Path
import shutil
import json

root = Path(__file__).resolve().parents[2]
cn = root.parent / 'Shipwright-CN'
for relative in ('soh/assets/xml/N64_NTSC_12',):
    shutil.copytree(cn / relative, root / relative, dirs_exist_ok=True)
shutil.copy2(cn / 'soh/assets/extractor/Config_N64_NTSC_12.xml',
             root / 'soh/assets/extractor/Config_N64_NTSC_12.xml')
p = root / 'OTRExporter/rom_info.py'
s = p.read_text()
if 'ROM_INFO_TABLE[Checksums.OOT_NTSC_12]' not in s:
    s = s.replace('class RomDmaEntry:', 'ROM_INFO_TABLE[Checksums.OOT_NTSC_12] = RomVersion("CFG/filelists/ntsc_oot.txt", 0x7960, "N64_NTSC_12")\n\nclass RomDmaEntry:')
    p.write_text(s)
p = root / 'docs/supportedHashes.json'
data = json.loads(p.read_text())
if not any(x['sha1'] == '41b3bdc48d98c48529219919015a1af22f5057c2' for x in data):
    data.append({'name': 'NTSC 1.2 (US)', 'sha1': '41b3bdc48d98c48529219919015a1af22f5057c2'})
    p.write_text(json.dumps(data, indent=4) + '\n')
