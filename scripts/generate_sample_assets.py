#!/usr/bin/env python3
"""
Generates the placeholder art and sound effects used by the demo scene.

The assets are deliberately simple procedural pixel art so the repository stays
free of third-party content. Re-run after editing to regenerate everything:

    python scripts/generate_sample_assets.py

Requires Pillow (pip install pillow).
"""
import math
import os
import struct
import wave

from PIL import Image, ImageDraw

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
TEXTURES = os.path.join(ROOT, "assets", "textures")
AUDIO = os.path.join(ROOT, "assets", "audio")


def save(img, name):
    os.makedirs(TEXTURES, exist_ok=True)
    img.save(os.path.join(TEXTURES, name))
    print("wrote", name)


def player():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    # Antenna
    d.line((32, 4, 32, 12), fill=(200, 200, 210, 255), width=3)
    d.ellipse((28, 0, 36, 8), fill=(255, 90, 90, 255))
    # Body
    d.rounded_rectangle((12, 12, 52, 54), radius=10, fill=(70, 160, 230, 255), outline=(30, 80, 140, 255), width=3)
    # Visor
    d.rounded_rectangle((18, 20, 46, 34), radius=6, fill=(20, 30, 50, 255))
    d.ellipse((22, 23, 30, 31), fill=(120, 240, 255, 255))
    d.ellipse((34, 23, 42, 31), fill=(120, 240, 255, 255))
    # Mouth / panel
    d.rectangle((24, 40, 40, 44), fill=(30, 80, 140, 255))
    d.rectangle((26, 41, 38, 43), fill=(255, 220, 120, 255))
    # Feet
    d.rounded_rectangle((16, 52, 28, 62), radius=4, fill=(40, 60, 90, 255))
    d.rounded_rectangle((36, 52, 48, 62), radius=4, fill=(40, 60, 90, 255))
    save(img, "player.png")


def crate():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rectangle((0, 0, 63, 63), fill=(176, 120, 60, 255), outline=(100, 65, 30, 255), width=4)
    for y in range(8, 64, 12):
        d.line((4, y, 59, y), fill=(150, 100, 45, 255), width=2)
    d.line((4, 4, 59, 59), fill=(110, 72, 35, 255), width=5)
    d.line((59, 4, 4, 59), fill=(110, 72, 35, 255), width=5)
    for x, y in ((8, 8), (52, 8), (8, 52), (52, 52)):
        d.ellipse((x - 3, y - 3, x + 3, y + 3), fill=(70, 70, 75, 255))
    save(img, "crate.png")


def ground():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rectangle((0, 0, 63, 63), fill=(120, 82, 50, 255))
    # Dirt speckles
    for i in range(40):
        x = (i * 37) % 64
        y = 20 + (i * 53) % 44
        d.point((x, y), fill=(95, 62, 36, 255))
    # Grass
    d.rectangle((0, 0, 63, 14), fill=(84, 170, 70, 255))
    d.rectangle((0, 12, 63, 16), fill=(60, 130, 52, 255))
    for i in range(0, 64, 8):
        d.polygon([(i, 14), (i + 4, 6), (i + 8, 14)], fill=(100, 190, 80, 255))
    save(img, "ground.png")


def platform():
    img = Image.new("RGBA", (64, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle((0, 0, 63, 31), radius=6, fill=(110, 115, 125, 255), outline=(70, 72, 80, 255), width=3)
    for x in range(8, 64, 16):
        d.line((x, 6, x, 25), fill=(90, 94, 104, 255), width=2)
    d.rectangle((3, 3, 60, 7), fill=(140, 145, 155, 255))
    save(img, "platform.png")


def coin():
    img = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.ellipse((4, 4, 59, 59), fill=(250, 200, 50, 255), outline=(200, 140, 20, 255), width=4)
    d.ellipse((16, 16, 47, 47), outline=(255, 235, 150, 255), width=3)
    d.rectangle((29, 20, 34, 44), fill=(200, 140, 20, 255))
    save(img, "coin.png")


def cloud():
    img = Image.new("RGBA", (128, 64), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    for (x, y, r) in ((30, 36, 22), (56, 26, 28), (86, 32, 24), (104, 40, 16), (60, 44, 20)):
        d.ellipse((x - r, y - r, x + r, y + r), fill=(255, 255, 255, 235))
    save(img, "cloud.png")


def sky():
    img = Image.new("RGBA", (16, 256), (0, 0, 0, 255))
    top = (28, 40, 90)
    bottom = (120, 170, 230)
    for y in range(256):
        t = y / 255.0
        c = tuple(int(top[i] + (bottom[i] - top[i]) * t) for i in range(3)) + (255,)
        for x in range(16):
            img.putpixel((x, 255 - y), c)
    save(img, "sky.png")


def blade():
    img = Image.new("RGBA", (32, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    d.rounded_rectangle((4, 0, 27, 127), radius=10, fill=(235, 235, 240, 255), outline=(150, 150, 160, 255), width=3)
    for y in range(16, 120, 24):
        d.line((8, y, 23, y), fill=(190, 190, 200, 255), width=2)
    save(img, "blade.png")


def write_wav(name, samples, rate=44100):
    os.makedirs(AUDIO, exist_ok=True)
    path = os.path.join(AUDIO, name)
    with wave.open(path, "w") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(rate)
        frames = b"".join(struct.pack("<h", int(max(-1.0, min(1.0, s)) * 32767)) for s in samples)
        w.writeframes(frames)
    print("wrote", name)


def jump_sound():
    rate = 44100
    length = int(rate * 0.18)
    out = []
    for i in range(length):
        t = i / rate
        freq = 320 + 900 * (t / 0.18)
        env = math.exp(-t * 18)
        out.append(0.6 * env * math.sin(2 * math.pi * freq * t))
    write_wav("jump.wav", out, rate)


def coin_sound():
    rate = 44100
    out = []
    for freq, start in ((988, 0.0), (1319, 0.08)):
        for i in range(int(rate * 0.12)):
            t = i / rate
            env = math.exp(-t * 20)
            idx = int((start + t) * rate)
            while len(out) <= idx:
                out.append(0.0)
            out[idx] += 0.5 * env * math.sin(2 * math.pi * freq * t)
    write_wav("coin.wav", out, rate)


def ambient_loop():
    rate = 22050
    length = rate * 4
    out = []
    for i in range(length):
        t = i / rate
        value = 0.12 * math.sin(2 * math.pi * 55 * t) + 0.08 * math.sin(2 * math.pi * 82.5 * t + 0.5) + 0.05 * math.sin(2 * math.pi * 110 * t)
        value *= 0.7 + 0.3 * math.sin(2 * math.pi * 0.25 * t)
        out.append(value)
    write_wav("ambient_loop.wav", out, rate)


if __name__ == "__main__":
    player(); crate(); ground(); platform(); coin(); cloud(); sky(); blade()
    jump_sound(); coin_sound(); ambient_loop()
