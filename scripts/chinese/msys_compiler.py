"""Translate MSYS paths inside GCC response files for native devkitPro tools."""
import os
from pathlib import Path
import re
import subprocess
import sys


def native(text):
    text = text.replace('/opt/devkitpro/', os.environ.get('DEVKITPRO_NATIVE', 'C:/devkitPro').replace('\\', '/') + '/')
    text = re.sub(r'(-[IL])/([a-zA-Z])/', lambda m: m[1] + m[2].upper() + ':/', text)
    return re.sub(r'(?<![\w/])/([a-zA-Z])/', lambda m: m[1].upper() + ':/', text)


def main():
    args = []
    temporary = []
    try:
        for arg in sys.argv[1:]:
            if arg.startswith('@'):
                source = Path(native(arg[1:]))
                target = source.with_name(source.name + '.windows')
                target.write_text(native(source.read_text(encoding='utf-8')), encoding='utf-8')
                temporary.append(target)
                args.append('@' + str(target))
            else:
                args.append(native(arg))
        return subprocess.call(args)
    finally:
        for path in temporary:
            path.unlink(missing_ok=True)


if __name__ == '__main__':
    sys.exit(main())
