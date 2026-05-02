#include "PluginEditor.h"
#include <BinaryData.h>

VelvetEvilEditor::VelvetEvilEditor(VelvetEvilProcessor& p)
    : AudioProcessorEditor(&p), proc(p),
      wobbleAttach(p.apvts, "wobble", wobbleKnob),
      reverbAttach(p.apvts, "reverb", reverbKnob)
{
    setSize(420, 420);

    auto img = juce::ImageCache::getFromMemory(BinaryData::image_0_png, BinaryData::image_0_pngSize);
    wobbleKnob.setKnobImage(img);
    reverbKnob.setKnobImage(img);

    addAndMakeVisible(wobbleKnob);
    addAndMakeVisible(reverbKnob);
    addAndMakeVisible(selectButton);

    selectButton.setColour(juce::TextButton::buttonColourId,  juce::Colour(0xFF1A1008));
    selectButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xFFAA8866));

    selectButton.onClick = [this] {
        proc.selectNextPreset();
        currentPresetName = proc.kPresets[proc.currentPreset].name;
        repaint();
    };

    clipPollTimer = std::make_unique<ClipTimer>(*this);
    clipPollTimer->startTimer(80);
}

VelvetEvilEditor::~VelvetEvilEditor() {
    clipPollTimer->stopTimer();
}

void VelvetEvilEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF080505));
}

void VelvetEvilEditor::resized()
{
    wobbleKnob.setBounds(48,118,150,150);
    reverbKnob.setBounds(222,118,150,150);
    selectButton.setBounds(12,360,85,26);
}
