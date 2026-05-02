# Blueprint — Stitch UI Pipeline

## Architecture
OpenCV -> JSON SSOT (ui-spec.json) -> JUCE (GeneratedLayout / ImageKnob) -> Screenshot Diff

## Principles
- SSOT is ui-spec.json, not PluginEditor.cpp
- No magic numbers in UI code
- Circle doctrine for all knobs (square bounds, center-locked rotation)
- Deterministic measurement over visual estimation

## Notes
- Initial commit provides scaffolding for tools and components.
- Next iterations will add OpenCV extraction and codegen mapping.
