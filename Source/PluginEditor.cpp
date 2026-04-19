#include "PluginEditor.h"

VelvetEvilEditor::VelvetEvilEditor(VelvetEvilProcessor& p)
    : AudioProcessorEditor(&p), proc(p),
      wobbleAttach(p.apvts, "wobble", wobbleKnob),
      reverbAttach(p.apvts, "reverb", reverbKnob)
{
    setSize(420, 420);
    wobbleKnob.setLookAndFeel(&knobLAF);
    reverbKnob.setLookAndFeel(&knobLAF);
    addAndMakeVisible(wobbleKnob);
    addAndMakeVisible(reverbKnob);
    addAndMakeVisible(selectButton);

    selectButton.onClick = [this] {
        proc.selectNextPreset();
        currentPresetName = proc.kPresets[proc.currentPreset].name;
        repaint();
    };

    clipPollTimer = std::make_unique<ClipTimer>(*this);
    clipPollTimer->startTimer(80);
}

VelvetEvilEditor::~VelvetEvilEditor() {
    wobbleKnob.setLookAndFeel(nullptr);
    reverbKnob.setLookAndFeel(nullptr);
    clipPollTimer->stopTimer();
}

void VelvetEvilEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xFF080808));

    // Title
    g.setColour(juce::Colour(0xFFCC0000));
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("VELVET EVIL", 0, 8, 420, 32, juce::Justification::centred);

    // Knob labels
    g.setColour(juce::Colour(0xFFCCAA88));
    g.setFont(juce::Font(11.0f, juce::Font::plain));
    g.drawText("TAPE WOBBLE", 58,  278, 140, 16, juce::Justification::centred);
    g.drawText("DARK SPACE",  222, 278, 140, 16, juce::Justification::centred);

    // LCD
    g.setColour(juce::Colour(0xFF0A0A0A));
    g.fillRoundedRectangle(10.0f, 308.0f, 400.0f, 35.0f, 4.0f);
    g.setColour(juce::Colour(0xFF00FF55));
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText(currentPresetName, 10, 308, 400, 35, juce::Justification::centred);

    // Clip LED
    const bool lc = proc.isLeftClipping();
    const bool rc = proc.isRightClipping();
    g.setColour((lc || rc) ? juce::Colour(0xFFFF2200) : juce::Colour(0xFF222222));
    g.fillEllipse(396.0f, 358.0f, 14.0f, 14.0f);
    if (lc || rc) proc.resetClipLEDs();
}

void VelvetEvilEditor::resized() {
    wobbleKnob.setBounds(58,  130, 140, 140);
    reverbKnob.setBounds(222, 130, 140, 140);
    selectButton.setBounds(12, 352, 80, 26);
}
