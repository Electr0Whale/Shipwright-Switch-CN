"""Build and package the pinned Switch Chinese port on Windows/devkitPro + WSL."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import shlex
import shutil
import subprocess
import sys
import time
import zipfile

ROOT = Path(__file__).resolve().parents[2]
ROM_SHA1 = '41b3bdc48d98c48529219919015a1af22f5057c2'


def posix(path, prefix):
    path = Path(path).resolve().as_posix()
    if len(path) < 3 or path[1] != ':':
        raise ValueError(f'Expected a Windows drive path: {path}')
    return f'{prefix}/{path[0].lower()}{path[2:]}'


def run(args, log, cwd=ROOT):
    print(f'Running {log.name}', flush=True)
    with log.open('w', encoding='utf-8') as out:
        subprocess.run(args, cwd=cwd, stdout=out, stderr=subprocess.STDOUT, check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom', type=Path)
    parser.add_argument('--output', type=Path, default=ROOT / 'dist' / ('chinese-' + time.strftime('%Y%m%d-%H%M%S')))
    parser.add_argument('--build-root', type=Path, help='Default is a new build directory inside output')
    parser.add_argument('--devkitpro', type=Path, default=Path('C:/devkitPro'))
    parser.add_argument('--distro', default='Ubuntu')
    parser.add_argument('--jobs', type=int, default=6)
    args = parser.parse_args()
    if hashlib.sha1(args.rom.read_bytes()).hexdigest() != ROM_SHA1:
        raise SystemExit('ROM SHA-1 does not match USA Rev 2; no output generated.')
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    logs = output / 'checks'
    logs.mkdir()
    build = (args.build_root or output / 'build').resolve()
    host, switch = build / 'host', build / 'switch'
    host.mkdir(parents=True, exist_ok=False)
    switch.mkdir(parents=True, exist_ok=False)
    sd = output / 'local-rom-package' / 'switch' / 'soh'
    sd.mkdir(parents=True)
    bash = args.devkitpro / 'msys2/usr/bin/bash.exe'
    os.environ['DEVKITPRO_NATIVE'] = str(args.devkitpro.resolve())
    quote = shlex.quote

    def linux(command, name, cwd=ROOT):
        run(['wsl', '-d', args.distro, '--', 'bash', '-c',
             'cd ' + quote(posix(cwd, '/mnt')) + ' && ' + command], logs / name)

    def msys(command, name):
        run([str(bash), '-c', 'cd ' + quote(posix(ROOT, '')) + ' && ' + command], logs / name)

    run([sys.executable, str(ROOT / 'scripts/chinese/build_ui.py'),
         '--font', str(ROOT.parent / 'Shipwright-CN/scripts/chinese/message/charmap/SourceHanSansSC-Regular.otf'),
         '--font-output', str(ROOT / 'packaging/fonts/ChineseUI.otf')], logs / 'catalog.log')
    run([sys.executable, str(ROOT / 'scripts/chinese/generate_file_select_titles.py'),
         '--font', str(ROOT / 'packaging/fonts/ChineseUI.otf')], logs / 'file-select-titles.log')
    run([sys.executable, str(ROOT / 'scripts/chinese/validate_assets.py')], logs / 'assets.json')
    h = quote(posix(host, '/mnt'))
    linux(f'cmake -S . -B {h} -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5', 'configure-host.log')
    linux(f'cmake --build {h} --target ZAPD -j{args.jobs}', 'build-host.log')
    launcher = sys.executable.replace('\\', '/') + ';' + (ROOT / 'scripts/chinese/msys_compiler.py').as_posix()
    s = quote(posix(switch, ''))
    configure = (f'cmake -S . -B {s} -G Ninja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake '
                 '-DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5')
    for kind in ('C_COMPILER', 'CXX_COMPILER', 'C_LINKER', 'CXX_LINKER'):
        configure += f' -DCMAKE_{kind}_LAUNCHER=' + quote(launcher)
    msys(configure, 'configure-switch.log')
    msys(f'cmake --build {s} --target soh_nro -j{args.jobs}', 'build-switch.log')
    # Absolute, fresh archive paths avoid the old exporter's existing-file path.
    command = ['python3', 'extract_assets.py', posix(args.rom, '/mnt'), '-z', posix(host / 'ZAPD/ZAPD.out', '/mnt'),
               '--xml-root', '../soh/assets/xml', '--custom-assets-path', '../soh/assets/custom',
               '--custom-otr-file', posix(sd / 'soh.otr', '/mnt'), '--otr-file', posix(sd / 'oot.otr', '/mnt'),
               '--port-ver', '8.0.5']
    linux(shlex.join(command), 'extraction.log', ROOT / 'OTRExporter')
    extraction_log = (logs / 'extraction.log').read_text(errors='replace')
    if '[error]' in extraction_log or 'Unimplemented' in extraction_log:
        raise RuntimeError('Exporter reported unresolved resources; see extraction.log')
    linux(f'g++ -std=c++17 -Ilibultraship/extern/StormLib/src scripts/chinese/inspect_otr.cpp '
          f'{h}/libultraship/extern/StormLib/libstorm.a -lz -o {h}/inspect_otr', 'build-inspector.log')
    for archive in ('oot.otr', 'soh.otr'):
        linux(f'{h}/inspect_otr ' + quote(posix(sd / archive, '/mnt')), archive + '.tsv')
    shutil.copy2(switch / 'soh/soh.nro', sd / 'soh.nro')
    shutil.copytree(ROOT / 'packaging/fonts', sd / 'fonts')
    shutil.copytree(ROOT / 'packaging/licenses', sd / 'licenses')
    for filename in ('README-zh-CN.md', 'BASELINES.json'):
        shutil.copy2(ROOT / 'packaging' / filename, sd / filename)
    manifest = {}
    for path in sorted(sd.rglob('*')):
        if path.is_file():
            manifest[path.relative_to(sd).as_posix()] = {
                'bytes': path.stat().st_size, 'sha256': hashlib.sha256(path.read_bytes()).hexdigest()}
    (sd / 'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2), encoding='utf-8')
    with zipfile.ZipFile(output / 'LOCAL-ONLY-Switch-Chinese.zip', 'w', zipfile.ZIP_DEFLATED) as archive:
        for path in sorted(sd.rglob('*')):
            if path.is_file(): archive.write(path, path.relative_to(output / 'local-rom-package'))
    # Keep source delivery separate from the ROM-derived package.
    source_zip = output / 'SOURCE-CHINESE-PATCH.zip'
    with zipfile.ZipFile(source_zip, 'w', zipfile.ZIP_DEFLATED) as archive:
        diff = subprocess.check_output(['git', '-C', str(ROOT), 'diff', '--binary'])
        archive.writestr('git-diff.patch', diff)
        archive.writestr('README.txt',
                         'Source changes only; no ROM, OTR archive, or NRO is included.\n'
                         'Apply git-diff.patch at the pinned Switch baseline, then copy the included files.\n')
        trees = [
            ('scripts/chinese', 'scripts/chinese'),
            ('packaging', 'packaging'),
            ('soh/soh/localization', 'soh/soh/localization'),
            ('soh/soh/z_message_CHI.cpp', 'soh/soh/z_message_CHI.cpp'),
            ('soh/src/code/z_kanfont_chinese_tbl.inc', 'soh/src/code/z_kanfont_chinese_tbl.inc'),
            ('soh/assets/custom', 'soh/assets/custom'),
            ('soh/assets/xml/N64_NTSC_12', 'soh/assets/xml/N64_NTSC_12'),
            ('soh/assets/extractor', 'soh/assets/extractor'),
        ]
        files = [
            ('OTRExporter/OTRExporter/CutsceneExporter.cpp', 'OTRExporter/OTRExporter/CutsceneExporter.cpp'),
            ('OTRExporter/OTRExporter/DisplayListExporter.cpp', 'OTRExporter/OTRExporter/DisplayListExporter.cpp'),
            ('OTRExporter/OTRExporter/Main.cpp', 'OTRExporter/OTRExporter/Main.cpp'),
            ('OTRExporter/OTRExporter/Ntsc12Aliases.inc', 'OTRExporter/OTRExporter/Ntsc12Aliases.inc'),
            ('OTRExporter/CFG/Config_N64_NTSC_12.xml', 'OTRExporter/CFG/Config_N64_NTSC_12.xml'),
            ('OTRExporter/CFG/filelists/ntsc_12_oot.txt', 'OTRExporter/CFG/filelists/ntsc_12_oot.txt'),
            ('ZAPDTR/ZAPD/ZRom.cpp', 'ZAPDTR/ZAPD/ZRom.cpp'),
            ('libultraship/extern/ImGui/imgui.cpp', 'libultraship/extern/ImGui/imgui.cpp'),
            ('libultraship/extern/ImGui/imgui.h', 'libultraship/extern/ImGui/imgui.h'),
            ('libultraship/extern/ImGui/imgui_widgets.cpp', 'libultraship/extern/ImGui/imgui_widgets.cpp'),
            ('libultraship/extern/ImGui/backends/imgui_impl_sdl2.cpp',
             'libultraship/extern/ImGui/backends/imgui_impl_sdl2.cpp'),
            ('libultraship/src/controller/controldeck/ControlDeck.cpp',
             'libultraship/src/controller/controldeck/ControlDeck.cpp'),
            ('libultraship/src/controller/controldevice/controller/mapping/factories/ButtonMappingFactory.cpp',
             'libultraship/src/controller/controldevice/controller/mapping/factories/ButtonMappingFactory.cpp'),
            ('libultraship/src/window/gui/Gui.h', 'libultraship/src/window/gui/Gui.h'),
            ('libultraship/src/window/gui/Gui.cpp', 'libultraship/src/window/gui/Gui.cpp'),
            ('soh/soh/OTRGlobals.cpp', 'soh/soh/OTRGlobals.cpp'),
            ('soh/src/code/z_kanfont.c', 'soh/src/code/z_kanfont.c'),
        ]
        for relative, arcroot in trees:
            path = ROOT / relative
            if path.is_file():
                archive.write(path, arcroot)
            elif path.is_dir():
                for item in sorted(path.rglob('*')):
                    if item.is_file(): archive.write(item, Path(arcroot) / item.relative_to(path))
        for relative, arcname in files:
            path = ROOT / relative
            if path.exists(): archive.write(path, arcname)
    print(f'Package: {sd}\nChecks: {logs}', flush=True)


if __name__ == '__main__':
    main()
