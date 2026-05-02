#!/usr/bin/env python3
"""Compare plugin screenshot against reference image.

Scaffold: computes a simple mean absolute difference if both images are same size.
"""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageChops


def main() -> int:
    parser = argparse.ArgumentParser(description="Compare screenshot with reference")
    parser.add_argument("reference", type=Path)
    parser.add_argument("screenshot", type=Path)
    args = parser.parse_args()

    ref = Image.open(args.reference).convert("RGB")
    scr = Image.open(args.screenshot).convert("RGB")

    if ref.size != scr.size:
        print(f"SIZE_MISMATCH ref={ref.size} scr={scr.size}")
        return 2

    diff = ImageChops.difference(ref, scr)
    hist = diff.histogram()
    # mean absolute difference
    pixels = ref.size[0] * ref.size[1] * 3
    mad = sum(i * h for i, h in enumerate(hist)) / pixels / 255.0

    print(f"UI_DIFF_SCORE={mad:.6f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
