#include "KnobLookAndFeel.h"

void KnobLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float /*rotaryStartAngle*/, float /*rotaryEndAngle*/,
    juce::Slider&)
{
    const float cx = x + width  * 0.5f;
    const float cy = y + height * 0.5f;
    const float r  = juce::jmin(width, height) * 0.5f * 0.75f;

    // Knob body
    g.setColour(juce::Colour(0xFF1C1C1C));
    g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
    g.setColour(juce::Colour(0xFF444444));
    g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.5f);

    // Indicator dot
    const float startAng  = juce::MathConstants<float>::pi * 1.25f;
    const float endAng    = juce::MathConstants<float>::pi * 2.75f;
    const float knobAngle = startAng + sliderPos * (endAng - startAng);
    const float ix = cx + r * 0.65f * std::sin(knobAngle);
    const float iy = cy - r * 0.65f * std::cos(knobAngle);
    g.setColour(juce::Colour(0xFFFF2200));
    g.fillEllipse(ix - 3.0f, iy - 3.0f, 6.0f, 6.0f);

    // === LED ARC (Green→Yellow→Orange, 7 o'clock → 5 o'clock) ===
    const float outerR  = 68.0f;
    const float innerR  = 56.0f;
    const float curAng  = startAng + sliderPos * (endAng - startAng);
    const int   numLEDs = 30;
    for (int i = 0; i < numLEDs; ++i) {
        const float t     = (float)i / (float)(numLEDs - 1);
        const float angle = startAng + t * (endAng - startAng);
        const bool  lit   = angle <= curAng;
        // Green(0.33)→Yellow(0.17)→Orange(0.08)
        const float hue   = lit ? (0.33f - t * 0.25f) : 0.0f;
        const float sat   = lit ? 1.0f : 0.15f;
        const float bri   = lit ? 0.80f : 0.07f;
        const auto  col   = juce::Colour::fromHSL(hue, sat, bri, 1.0f);
        const float midR  = (innerR + outerR) * 0.5f;
        const float lx    = cx + std::cos(angle) * midR;
        const float ly    = cy + std::sin(angle) * midR;
        if (lit) {
            g.setColour(col.withAlpha(0.25f));
            g.fillEllipse(lx - 5.5f, ly - 5.5f, 11.0f, 11.0f);
            g.setColour(col.withAlpha(0.6f));
            g.fillEllipse(lx - 3.5f, ly - 3.5f, 7.0f, 7.0f);
        }
        g.setColour(col);
        g.fillEllipse(lx - 2.5f, ly - 2.5f, 5.0f, 5.0f);
    }
}
