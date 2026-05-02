#!/usr/bin/env python3
from PIL import Image
import sys

img = Image.open(sys.argv[1]).convert("RGBA")
w, h = img.size

if w != h:
    print("ERROR: not square")
    sys.exit(1)

print(f"OK: square {w}x{h}")

# simple center mass check
pixels = img.getdata()
alpha = [p[3] for p in pixels]
print("Alpha avg:", sum(alpha)/len(alpha))
