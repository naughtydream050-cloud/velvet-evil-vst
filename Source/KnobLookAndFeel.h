#pragma once
#include <JuceHeader.h>

class KnobLookAndFeel : public juce::LookAndFeel_V4 {
public:
    KnobLookAndFeel() = default;
    ~KnobLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
                          float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobLookAndFeel)
};
