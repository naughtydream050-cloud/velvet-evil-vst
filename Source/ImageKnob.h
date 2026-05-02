#pragma once
#include <JuceHeader.h>

// ImageKnob: center-locked, square-preserving rotary image knob
class ImageKnob : public juce::Slider
{
public:
    ImageKnob()
        : juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox)
    {}

    void setKnobImage(const juce::Image& img) { knobImage = img; }

    void setAngleRangeDegrees(float startDeg, float endDeg)
    {
        startAngle = juce::degreesToRadians(startDeg);
        endAngle   = juce::degreesToRadians(endDeg);
    }

    void paint(juce::Graphics& g) override
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto side = juce::jmin(bounds.getWidth(), bounds.getHeight());
        const auto square = bounds.withSizeKeepingCentre(side, side);

        if (! knobImage.isValid())
            return;

        const auto norm = (float) ((getValue() - getMinimum()) / (getMaximum() - getMinimum()));
        const auto angle = startAngle + norm * (endAngle - startAngle);

        juce::Graphics::ScopedSaveState state(g);
        g.addTransform(juce::AffineTransform::rotation(angle, square.getCentreX(), square.getCentreY()));

        // center-crop to square to avoid transparent padding affecting rotation
        const int srcSide = juce::jmin(knobImage.getWidth(), knobImage.getHeight());
        const int srcX = (knobImage.getWidth()  - srcSide) / 2;
        const int srcY = (knobImage.getHeight() - srcSide) / 2;

        g.drawImage(knobImage,
                    square.getX(), square.getY(), square.getWidth(), square.getHeight(),
                    (float) srcX, (float) srcY, (float) srcSide, (float) srcSide);
    }

private:
    juce::Image knobImage;
    float startAngle { juce::degreesToRadians(-135.0f) };
    float endAngle   { juce::degreesToRadians(135.0f) };
};
