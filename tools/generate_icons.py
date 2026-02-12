#!/usr/bin/env python3
"""
Icon generator for MusicWorld-Recreation.

Usage examples:
  python3 tools/generate_icons.py
  python3 tools/generate_icons.py --source assets/sprites/icons/icon_512x512.png
  python3 tools/generate_icons.py --no-android
  python3 tools/generate_icons.py --no-desktop --no-android
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("Error: Pillow is not installed. Install it with: pip install pillow")
    sys.exit(2)


# Размеры desktop-иконок для проекта (PNG).
DESKTOP_PNG_SIZES = [16, 32, 48, 64, 128, 256, 512]

# Размеры, которые будут упакованы в Windows ICO.
ICO_SIZES = [16, 24, 32, 48, 64, 128, 256]

# Соответствие Android density -> размер launcher-иконки.
ANDROID_MIPMAP_SIZES = {
    "mdpi": 48,
    "hdpi": 72,
    "xhdpi": 96,
    "xxhdpi": 144,
    "xxxhdpi": 192,
}

# Размеры macOS PNG-иконок и ICNS слотов.
APPLE_MACOS_SIZES = [16, 32, 64, 128, 256, 512, 1024]

# Набор iOS App Icon размеров (плоские PNG, пригодны для импорта в Xcode).
APPLE_IOS_ICON_SPECS = [
    ("ios_app_20@2x.png", 40),
    ("ios_app_20@3x.png", 60),
    ("ios_app_29@2x.png", 58),
    ("ios_app_29@3x.png", 87),
    ("ios_app_40@2x.png", 80),
    ("ios_app_40@3x.png", 120),
    ("ios_app_60@2x.png", 120),
    ("ios_app_60@3x.png", 180),
    ("ios_app_76@1x.png", 76),
    ("ios_app_76@2x.png", 152),
    ("ios_app_83.5@2x.png", 167),
    ("ios_app_1024.png", 1024),
]


def parse_args() -> argparse.Namespace:
    # Корень репозитория вычисляется относительно расположения скрипта.
    repo_root = Path(__file__).resolve().parents[1]
    default_source = repo_root / "assets" / "sprites" / "icons" / "icon_512x512.png"
    default_desktop_dir = repo_root / "assets" / "sprites" / "icons"
    default_android_res_dir = repo_root / "android" / "app" / "src" / "main" / "res"
    default_apple_dir = repo_root / "assets" / "sprites" / "icons" / "apple"

    parser = argparse.ArgumentParser(description="Generate app icons for desktop, Android, macOS and iOS.")
    parser.add_argument(
        "--source",
        type=Path,
        default=default_source,
        help=f"Source image path (default: {default_source})",
    )
    parser.add_argument(
        "--desktop-dir",
        type=Path,
        default=default_desktop_dir,
        help=f"Desktop icon output dir (default: {default_desktop_dir})",
    )
    parser.add_argument(
        "--android-res-dir",
        type=Path,
        default=default_android_res_dir,
        help=f"Android res output dir (default: {default_android_res_dir})",
    )
    parser.add_argument(
        "--apple-dir",
        type=Path,
        default=default_apple_dir,
        help=f"Apple icon output dir (default: {default_apple_dir})",
    )
    parser.add_argument(
        "--no-desktop",
        action="store_true",
        help="Skip desktop PNG/ICO generation.",
    )
    parser.add_argument(
        "--no-android",
        action="store_true",
        help="Skip Android mipmap icon generation.",
    )
    parser.add_argument(
        "--no-apple",
        action="store_true",
        help="Skip macOS/iOS icon generation.",
    )
    return parser.parse_args()


def ensure_parent(path: Path) -> None:
    # Создаем каталог назначения перед записью файла.
    path.parent.mkdir(parents=True, exist_ok=True)


def resize_pixel_perfect(img: Image.Image, size: int) -> Image.Image:
    # Используем nearest, чтобы не мылить пиксельную графику.
    return img.resize((size, size), resample=Image.Resampling.NEAREST)


def generate_desktop_icons(img: Image.Image, out_dir: Path) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    print(f"[desktop] Output directory: {out_dir}")

    for size in DESKTOP_PNG_SIZES:
        out_file = out_dir / f"icon_{size}x{size}.png"
        resized = resize_pixel_perfect(img, size)
        resized.save(out_file, format="PNG")
        print(f"[desktop] Wrote PNG: {out_file}")

    ico_file = out_dir / "icon.ico"
    ico_sizes = [(size, size) for size in ICO_SIZES]
    img.save(ico_file, format="ICO", sizes=ico_sizes)
    print(f"[desktop] Wrote ICO: {ico_file}")


def generate_android_icons(img: Image.Image, android_res_dir: Path) -> None:
    print(f"[android] Output directory: {android_res_dir}")
    for density, size in ANDROID_MIPMAP_SIZES.items():
        mipmap_dir = android_res_dir / f"mipmap-{density}"
        mipmap_dir.mkdir(parents=True, exist_ok=True)

        launcher = mipmap_dir / "ic_launcher.png"
        launcher_round = mipmap_dir / "ic_launcher_round.png"

        resized = resize_pixel_perfect(img, size)
        resized.save(launcher, format="PNG")
        resized.save(launcher_round, format="PNG")

        print(f"[android] Wrote: {launcher}")
        print(f"[android] Wrote: {launcher_round}")


def generate_apple_icons(img: Image.Image, apple_dir: Path) -> None:
    apple_dir.mkdir(parents=True, exist_ok=True)
    print(f"[apple] Output directory: {apple_dir}")

    for size in APPLE_MACOS_SIZES:
        out_file = apple_dir / f"macos_icon_{size}x{size}.png"
        resized = resize_pixel_perfect(img, size)
        resized.save(out_file, format="PNG")
        print(f"[apple] Wrote macOS PNG: {out_file}")

    icns_file = apple_dir / "icon.icns"
    icns_sizes = [(size, size) for size in APPLE_MACOS_SIZES]
    img.save(icns_file, format="ICNS", sizes=icns_sizes)
    print(f"[apple] Wrote macOS ICNS: {icns_file}")

    for filename, size in APPLE_IOS_ICON_SPECS:
        out_file = apple_dir / filename
        resized = resize_pixel_perfect(img, size)
        resized.save(out_file, format="PNG")
        print(f"[apple] Wrote iOS PNG: {out_file}")


def main() -> int:
    args = parse_args()
    source = args.source.resolve()

    if not source.exists():
        print(f"Error: Source icon not found: {source}")
        return 1

    try:
        img = Image.open(source).convert("RGBA")
    except Exception as exc:
        print(f"Error: Failed to open source image: {exc}")
        return 1

    print(f"Source image: {source}")
    print(f"Source size: {img.width}x{img.height}")

    if args.no_desktop and args.no_android and args.no_apple:
        print("Nothing to do: desktop, android and apple generation are all disabled.")
        return 0

    if not args.no_desktop:
        generate_desktop_icons(img, args.desktop_dir.resolve())
    else:
        print("[desktop] Skipped.")

    if not args.no_android:
        generate_android_icons(img, args.android_res_dir.resolve())
    else:
        print("[android] Skipped.")

    if not args.no_apple:
        generate_apple_icons(img, args.apple_dir.resolve())
    else:
        print("[apple] Skipped.")

    print("Done.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
