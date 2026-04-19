#include "PluginProcessor.h"
#include "PluginEditor.h"

const std::array<VelvetEvilProcessor::Preset, 4> VelvetEvilProcessor::kPresets = {{
    { "SANCTUARY", 0.20f, 0.10f },
    { "BLOOD",     0.50f, 0.45f },
    { "CRIMSON",   0.75f, 0.85f },
    { "WHISPER",   0.15f, 0.05f }
}};

juce::AudioProcessorValueTreeState::ParameterLayout VelvetEvilProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"wobble", 1}, "Tape Wobble",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.20f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"reverb", 1}, "Dark Space",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.10f));
    return { params.begin(), params.end() };
}

VelvetEvilProcessor::VelvetEvilProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "VELVETEVIL", createParameterLayout()) {}

void VelvetEvilProcessor::selectNextPreset() {
    currentPreset = (currentPreset + 1) % (int)kPresets.size();
    const auto& p = kPresets[currentPreset];
    if (auto* w = apvts.getParameter("wobble")) w->setValueNotifyingHost(p.wobble);
    if (auto* r = apvts.getParameter("reverb")) r->setValueNotifyingHost(p.reverb);
}

juce::AudioProcessorEditor* VelvetEvilProcessor::createEditor() {
    return new VelvetEvilEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new VelvetEvilProcessor();
}
