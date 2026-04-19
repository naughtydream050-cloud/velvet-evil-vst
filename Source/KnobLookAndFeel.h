#pragma once
#include <JuceHeader.h>

class VelvetEvilKnobLAF : public juce::LookAndFeel_V4
{
public:
    VelvetEvilKnobLAF();
    ~VelvetEvilKnobLAF() override = default;

    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h,
                           float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

private:
    juce::Image knobImage;

    void drawLEDRing (juce::Graphics&, float cx, float cy,
                      float radius, float startAngle, float currentAngle,
                      float sliderPos) const;

    void drawKnobImage (juce::Graphics&, float cx, float cy,
                        float knobRadius, float angle) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VelvetEvilKnobLAF)
};
