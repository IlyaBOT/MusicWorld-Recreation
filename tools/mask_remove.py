#!/usr/bin/env python3
from PIL import Image
import sys

def remove_background_color(input_path, output_path, target=(0x20, 0x90, 0x20), tolerance=10):
    """
    Заменяет заданный цвет и его близкие оттенки на прозрачный.
    
    target: цвет фона (RGB).
    tolerance: максимальное отклонение по каждому каналу, чтобы считать цвет "фоновым.
    """
    img = Image.open(input_path).convert("RGBA")
    pixels = img.load()
    for y in range(img.height):
        for x in range(img.width):
            r, g, b, a = pixels[x, y]
            if (abs(r - target[0]) <= tolerance and
                abs(g - target[1]) <= tolerance and
                abs(b - target[2]) <= tolerance):
                # делаем пиксель полностью прозрачным
                pixels[x, y] = (r, g, b, 0)
    img.save(output_path)
    print(f"Done: {output_path}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python mask_remove.py <input.png> <output.png> [tolerance]")
    else:
        tol = int(sys.argv[3]) if len(sys.argv) > 3 else 10
        remove_background_color(sys.argv[1], sys.argv[2], tolerance=tol)
