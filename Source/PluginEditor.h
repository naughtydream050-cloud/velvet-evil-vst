#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ImageKnob.h"

class VelvetEvilEditor : public juce::AudioProcessorEditor {
public:
    explicit VelvetEvilEditor(VelvetEvilProcessor&);
    ~VelvetEvilEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    VelvetEvilProcessor& proc;
    ImageKnob wobbleKnob;
    ImageKnob reverbKnob;
    juce::AudioProcessorValueTreeState::SliderAttachment wobbleAttach, reverbAttach;
    juce::TextButton selectButton { "SELECT" };
    juce::String currentPresetName { "SANCTUARY" };

    struct ClipTimer : juce::Timer {
        VelvetEvilEditor& owner;
        explicit ClipTimer(VelvetEvilEditor& o) : owner(o) {}
        void timerCallback() override { owner.repaint(); }
    };
    std::unique_ptr<ClipTimer> clipPollTimer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelvetEvilEditor)
};
