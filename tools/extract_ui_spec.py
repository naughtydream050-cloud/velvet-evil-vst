#!/usr/bin/env python3
"""Extract a starter ui-spec.json from a reference image.

This file is intentionally minimal in the first QUAD-BOOT pipeline commit.
Future passes should add OpenCV-based palette, bounds, and knob-center extraction.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(description="Extract a starter UI spec from a reference image")
    parser.add_argument("reference", type=Path, help="Reference UI image")
    parser.add_argument("--out", type=Path, default=Path("ui/spec/ui-spec.json"))
    args = parser.parse_args()

    spec = {
        "canvas": {"width": 420, "height": 420},
        "reference": str(args.reference),
        "controls": {},
        "palette": {},
        "notes": "TODO: add OpenCV measurement pass for bounds, colors, and knob centers."
    }

    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(spec, indent=2) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
