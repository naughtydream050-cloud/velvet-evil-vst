#include "PluginEditor.h"

VelvetEvilEditor::VelvetEvilEditor(VelvetEvilProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    {
        int sz = 0;
        const char* data = BinaryData::getNamedResource("image_0_png", sz);
        if (data && sz > 0)
            backgroundImage = juce::ImageCache::getFromMemory(data, sz);
    }

    auto setupKnob = [&](juce::Slider& k)
    {
        k.setLookAndFeel(&knobLAF);
        k.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        k.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        k.setRange(0.0, 1.0, 0.001);
        k.setValue(0.5, juce::dontSendNotification);
        k.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                              juce::MathConstants<float>::pi * 2.75f, true);
        addAndMakeVisible(k);
    };
    setupKnob(darkMelodyKnob);
    setupKnob(gritKnob);

    darkMelodyAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "drive", darkMelodyKnob);
    gritAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "grit", gritKnob);

    setSize(Layout::kWidth, Layout::kHeight);
}

VelvetEvilEditor::~VelvetEvilEditor()
{
    darkMelodyKnob.setLookAndFeel(nullptr);
    gritKnob      .setLookAndFeel(nullptr);
}

void VelvetEvilEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF111111));
    if (backgroundImage.isValid())
        g.drawImage(backgroundImage,
                    0, 0, getWidth(), getHeight(),
                    0, 0, backgroundImage.getWidth(), backgroundImage.getHeight(), false);
}

void VelvetEvilEditor::resized()
{
    using namespace Layout;
    const int r = kKnobD / 2;
    darkMelodyKnob.setBounds(kKnob1CX - r, kKnob1CY - r, kKnobD, kKnobD);
    gritKnob      .setBounds(kKnob2CX - r, kKnob2CY - r, kKnobD, kKnobD);
}
