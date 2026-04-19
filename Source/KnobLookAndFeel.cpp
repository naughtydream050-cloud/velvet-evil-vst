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
    const float startAng = juce::MathConstants<float>::pi * 1.25f;
    const float endAng   = juce::MathConstants<float>::pi * 2.75f;
    const float knobAngle = startAng + sliderPos * (endAng - startAng);
    const float ix = cx + r * 0.65f * std::sin(knobAngle);
    const float iy = cy - r * 0.65f * std::cos(knobAngle);
    g.setColour(juce::Colour(0xFFFF2200));
    g.fillEllipse(ix - 3.0f, iy - 3.0f, 6.0f, 6.0f);

    // LED arc — 7 o'clock to 5 o'clock, red→green gradient
    const float outerR = 72.0f;
    const float innerR = 60.0f;
    const float arcR   = (innerR + outerR) * 0.5f;
    const float currentAngle = startAng + sliderPos * (endAng - startAng);
    const int numLEDs = 28;

    for (int i = 0; i < numLEDs; ++i) {
        const float t        = (float)i / (numLEDs - 1);
        const float ledAngle = startAng + t * (endAng - startAng);
        const bool  lit      = ledAngle <= currentAngle;
        const float hue      = lit ? t * 0.33f : 0.0f;
        const float sat      = lit ? 1.0f : 0.1f;
        const float bri      = lit ? 0.85f : 0.08f;
        const juce::Colour ledCol = juce::Colour::fromHSL(hue, sat, bri, 1.0f);
        const float lx = x + width  * 0.5f + std::cos(ledAngle) * arcR;
        const float ly = y + height * 0.5f + std::sin(ledAngle) * arcR;
        g.setColour(ledCol);
        g.fillEllipse(lx - 3.5f, ly - 3.5f, 7.0f, 7.0f);
    }
}
