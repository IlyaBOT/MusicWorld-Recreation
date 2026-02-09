#!/usr/bin/env python3
import os
import sys
from PIL import Image

# Набор размеров для PNG-иконок и ICO (16...256 пикселей)
PNG_SIZES = [16, 32, 48, 64, 128, 256, 512]

def make_icons(input_path: str, output_dir: str):
    # Создаём каталог, если его нет
    os.makedirs(output_dir, exist_ok=True)

    # Загружаем исходное изображение и приводим к RGBA
    img = Image.open(input_path).convert('RGBA')

    # Генерируем PNG в указанных размерах
    for size in PNG_SIZES:
        resized = img.resize((size, size), resample=Image.Resampling.NEAREST)
        filename = f"icon_{size}x{size}.png"
        resized.save(os.path.join(output_dir, filename))
        print(f"Saved: {filename}")

    # Создаём ICO-файл (не более 256x256, большие размеры Windows игнорирует)
    ico_sizes = [(size, size) for size in PNG_SIZES if size <= 256]
    ico_path = os.path.join(output_dir, "icon.ico")
    img.save(ico_path, format='ICO', sizes=ico_sizes)
    print(f"ICO-file created: {ico_path}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python icon-gen.py input.png output_folder/")
        sys.exit(1)
    make_icons(sys.argv[1], sys.argv[2])
