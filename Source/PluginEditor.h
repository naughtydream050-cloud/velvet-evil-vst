#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "KnobLookAndFeel.h"

namespace Layout
{
    constexpr int kWidth   = 420;
    constexpr int kHeight  = 420;

    // Left knob  (Drive / "DARK MELODY")
    constexpr int kKnob1CX = 128;
    constexpr int kKnob1CY = 205;
    constexpr int kKnobD   = 160;

    // Right knob (Grit / "GRIT")
    constexpr int kKnob2CX = 292;
    constexpr int kKnob2CY = 205;
}

class VelvetEvilEditor : public juce::AudioProcessorEditor
{
public:
    explicit VelvetEvilEditor (VelvetEvilProcessor&);
    ~VelvetEvilEditor() override;

    void paint   (juce::Graphics&) override;
    void resized () override;

private:
    VelvetEvilProcessor& audioProcessor;
    juce::Image backgroundImage;

    VelvetEvilKnobLAF knobLAF;

    juce::Slider darkMelodyKnob { juce::Slider::RotaryVerticalDrag,
                                  juce::Slider::NoTextBox };
    juce::Slider gritKnob       { juce::Slider::RotaryVerticalDrag,
                                  juce::Slider::NoTextBox };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        darkMelodyAttach, gritAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VelvetEvilEditor)
};
