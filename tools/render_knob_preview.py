#!/usr/bin/env python3
from PIL import Image
import math, sys

img = Image.open(sys.argv[1]).convert("RGBA")
w,h = img.size
side = min(w,h)
img = img.crop(((w-side)//2,(h-side)//2,(w+side)//2,(h+side)//2))

for i,p in enumerate([0,0.25,0.5,0.75,1.0]):
    angle = -135 + p*270
    rotated = img.rotate(angle, resample=Image.BICUBIC)
    rotated.save(f"preview_{i}.png")
