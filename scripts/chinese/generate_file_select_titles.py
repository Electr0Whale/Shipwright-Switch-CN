"""Generate Simplified Chinese IA8 title textures used by custom file-select screens."""

from argparse import ArgumentParser
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[2]
OUTPUT = ROOT / "soh/assets/custom/textures/title_static"
TITLES = {
    "gFileSelPleaseChooseAQuestCHITex.ia8.png": "请选择游戏模式",
    "gFileSelBossRushSettingsCHIText.ia8.png": "首领连战设置",
}


def render(text: str, font: ImageFont.FreeTypeFont) -> Image.Image:
    mask = Image.new("L", (128, 16), 0)
    draw = ImageDraw.Draw(mask)
    left, top, right, bottom = draw.textbbox((0, 0), text, font=font)
    width = right - left
    height = bottom - top
    x = (mask.width - width) // 2 - left
    y = (mask.height - height) // 2 - top
    draw.text((x, y), text, font=font, fill=255)
    # The source title textures are IA8. Quantizing the coverage to four bits
    # before conversion makes the checked-in PNG deterministic and matches the
    # antialiasing precision of the packed resource.
    mask = mask.point(lambda value: min(255, ((value + 8) // 17) * 17))
    return Image.merge("RGBA", (mask, mask, mask, mask))


def main() -> None:
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("--font", type=Path, required=True)
    args = parser.parse_args()
    font = ImageFont.truetype(str(args.font), 13)
    OUTPUT.mkdir(parents=True, exist_ok=True)
    for filename, text in TITLES.items():
        path = OUTPUT / filename
        render(text, font).save(path)
        print(f"{path.relative_to(ROOT)}: {text}")


if __name__ == "__main__":
    main()
