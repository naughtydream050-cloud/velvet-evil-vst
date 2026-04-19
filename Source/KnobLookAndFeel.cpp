#include "KnobLookAndFeel.h"

VelvetEvilKnobLAF::VelvetEvilKnobLAF()
{
    int sz = 0;
    const char* data = BinaryData::getNamedResource("image_2_png", sz);
    if (data && sz > 0)
        knobImage = juce::ImageCache::getFromMemory(data, sz);
}

void VelvetEvilKnobLAF::drawRotarySlider(
        juce::Graphics& g, int x, int y, int w, int h,
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
        juce::Slider&)
{
    const float cx    = x + w * 0.5f;
    const float cy    = y + h * 0.5f;
    const float r     = juce::jmin(w, h) * 0.5f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const float knobR = r * 0.88f;

    drawLEDRing(g, cx, cy, r, rotaryStartAngle, angle, sliderPos);
    drawKnobImage(g, cx, cy, knobR, angle);
}

// SANCTUARY(0-30%): dim red  /  BLOOD(30-65%): digital red  /  CRIMSON(65-100%): hot red
void VelvetEvilKnobLAF::drawLEDRing(
        juce::Graphics& g, float cx, float cy,
        float r, float startAngle, float currentAngle, float sliderPos) const
{
    const float arcR  = r * 0.96f;
    const float thick = r * 0.07f;
    const float sweep = juce::MathConstants<float>::twoPi * 0.833f;

    juce::Path track;
    track.addCentredArc(cx, cy, arcR, arcR, 0.0f, startAngle, startAngle + sweep, true);
    g.setColour(juce::Colour(0xFF0D0D0D));
    g.strokePath(track, juce::PathStrokeType(thick,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (currentAngle <= startAngle + 0.001f) return;

    juce::Colour led = (sliderPos < 0.30f) ? juce::Colour(0xFF440011) :
                       (sliderPos < 0.65f) ? juce::Colour(0xFFFF0033) :
                                             juce::Colour(0xFFFF3355);

    const float gw[] = { 4.5f, 2.8f, 1.6f };
    const float ga[] = { 0.05f, 0.13f, 0.28f };
    for (int i = 0; i < 3; ++i)
    {
        juce::Path p;
        p.addCentredArc(cx, cy, arcR, arcR, 0.0f, startAngle, currentAngle, true);
        g.setColour(led.withAlpha(ga[i]));
        g.strokePath(p, juce::PathStrokeType(thick * gw[i],
            juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    juce::Path solid;
    solid.addCentredArc(cx, cy, arcR, arcR, 0.0f, startAngle, currentAngle, true);
    g.setColour(led);
    g.strokePath(solid, juce::PathStrokeType(thick * 0.5f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const float tipX = cx + arcR * std::sin(currentAngle);
    const float tipY = cy - arcR * std::cos(currentAngle);
    const float tipR = thick * 0.6f;
    g.setColour(led.brighter(0.4f));
    g.fillEllipse(tipX - tipR, tipY - tipR, tipR * 2.0f, tipR * 2.0f);
}

void VelvetEvilKnobLAF::drawKnobImage(
        juce::Graphics& g, float cx, float cy,
        float knobRadius, float angle) const
{
    const float d = knobRadius * 2.0f;

    if (knobImage.isValid())
    {
        juce::Graphics::ScopedSaveState state(g);
        g.addTransform(juce::AffineTransform::rotation(angle, cx, cy));
        g.drawImage(knobImage,
                    (int)(cx - d * 0.5f), (int)(cy - d * 0.5f),
                    (int)d, (int)d,
                    0, 0, knobImage.getWidth(), knobImage.getHeight(), false);
    }
    else
    {
        g.setColour(juce::Colour(0xFF1C1C1C));
        g.fillEllipse(cx - knobRadius, cy - knobRadius, d, d);
        g.setColour(juce::Colour(0xFF333333));
        g.drawEllipse(cx - knobRadius, cy - knobRadius, d, d, 1.5f);
        const float ix = cx + knobRadius * 0.65f * std::sin(angle);
        const float iy = cy - knobRadius * 0.65f * std::cos(angle);
        g.setColour(juce::Colour(0xFFFF0033));
        g.drawLine(cx, cy, ix, iy, 2.0f);
    }
}
