#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>

class VelvetEvilProcessor : public juce::AudioProcessor {
public:
    struct Preset { juce::String name; float wobble, reverb; };
    static const std::array<Preset, 4> kPresets;
    int currentPreset { 0 };
    std::atomic<bool> leftClipping  { false };
    std::atomic<bool> rightClipping { false };

    VelvetEvilProcessor();
    ~VelvetEvilProcessor() override = default;

    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "VELVET EVIL"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    void selectNextPreset();
    bool isLeftClipping()  const noexcept { return leftClipping.load();  }
    bool isRightClipping() const noexcept { return rightClipping.load(); }
    void resetClipLEDs()   noexcept { leftClipping = false; rightClipping = false; }

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VelvetEvilProcessor)
};
