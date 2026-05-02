#!/usr/bin/env python3
"""Generate JUCE layout headers from ui-spec.json.

Initial scaffold: emits placeholders. Subsequent passes will map controls to JUCE components.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate JUCE layout from ui-spec.json")
    parser.add_argument("spec", type=Path, help="ui-spec.json path")
    parser.add_argument("--out", type=Path, default=Path("Source/GeneratedLayout.h"))
    args = parser.parse_args()

    data = json.loads(args.spec.read_text(encoding="utf-8"))

    content = """#pragma once
// GeneratedLayout.h (scaffold)\n\n// TODO: map ui-spec.json to JUCE setBounds and ImageKnob bindings.\n"""

    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(content, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
