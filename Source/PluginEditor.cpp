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

    selectButton.setColour(juce::TextButton::buttonColourId,   juce::Colour(0xFF1A1008));
    selectButton.setColour(juce::TextButton::textColourOffId,  juce::Colour(0xFFAA8866));
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

void VelvetEvilEditor::paint(juce::Graphics& g)
{
    const int W = getWidth();
    const int H = getHeight();

    // === BACKGROUND: dark cracked metal ===
    g.fillAll(juce::Colour(0xFF080505));

    // Cracked texture simulation via layered gradients
    {
        juce::ColourGradient radGrad(
            juce::Colour(0xFF1A0A08), W * 0.5f, H * 0.5f,
            juce::Colour(0xFF040202), 0.0f, 0.0f,
            true);
        g.setGradientFill(radGrad);
        g.fillRect(0, 0, W, H);
    }

    // Blood vein accents — diagonal scratch lines
    g.setColour(juce::Colour(0x228B0000));
    for (int i = 0; i < 12; ++i) {
        const float x1 = (float)(i * 38 - 20);
        g.drawLine(x1, 0.0f, x1 + 80.0f, (float)H, 0.6f);
    }
    g.setColour(juce::Colour(0x15CC2200));
    for (int i = 0; i < 8; ++i) {
        const float y1 = (float)(i * 55);
        g.drawLine(0.0f, y1, (float)W, y1 + 30.0f, 0.4f);
    }

    // Corner rust stains
    g.setColour(juce::Colour(0x40660000));
    g.fillEllipse(-30, -30, 100, 100);
    g.fillEllipse(W - 70, -30, 100, 100);
    g.fillEllipse(-30, H - 70, 100, 100);
    g.fillEllipse(W - 70, H - 70, 100, 100);

    // === TITLE: VELVET EVIL ===
    // Decorative top bar
    g.setColour(juce::Colour(0xFF330A0A));
    g.fillRect(0, 0, W, 58);
    g.setColour(juce::Colour(0xFF550000));
    g.fillRect(0, 56, W, 2);

    // Title text
    g.setColour(juce::Colour(0xFFCC1111));
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("VELVET EVIL", 0, 8, W, 28, juce::Justification::centred);

    // Decorative subtitle
    g.setColour(juce::Colour(0xFF663333));
    g.setFont(juce::Font(10.0f, juce::Font::italic));
    g.drawText("~ FIND YOURSELVES ~", 0, 36, W, 16, juce::Justification::centred);

    // === KNOB LABELS ===
    g.setColour(juce::Colour(0xFFCCBBAA));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText("DARK MELODY", 48,  278, 160, 16, juce::Justification::centred);
    g.drawText("GRIT",        212, 278, 160, 16, juce::Justification::centred);

    // === CLIP LEDs ===
    const bool lc = proc.isLeftClipping();
    const bool rc = proc.isRightClipping();
    // Left indicator (near DARK MELODY knob)
    g.setColour(lc ? juce::Colour(0xFFFF3300) : juce::Colour(0xFF1A0808));
    g.fillEllipse(8.0f, 8.0f, 10.0f, 10.0f);
    // Right indicator
    g.setColour(rc ? juce::Colour(0xFFFF3300) : juce::Colour(0xFF1A0808));
    g.fillEllipse((float)(W - 18), 8.0f, 10.0f, 10.0f);
    if (lc || rc) proc.resetClipLEDs();

    // === LCD PANEL ===
    g.setColour(juce::Colour(0xFF0D0804));
    g.fillRoundedRectangle(60.0f, 308.0f, 300.0f, 38.0f, 4.0f);
    g.setColour(juce::Colour(0xFF332211));
    g.drawRoundedRectangle(60.0f, 308.0f, 300.0f, 38.0f, 4.0f, 1.0f);
    g.setColour(juce::Colour(0xFFFFAA00));
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText(currentPresetName, 60, 308, 300, 38, juce::Justification::centred);

    // Bottom separator
    g.setColour(juce::Colour(0xFF330A0A));
    g.fillRect(0, 356, W, H - 356);
    g.setColour(juce::Colour(0xFF550000));
    g.fillRect(0, 356, W, 1);

    // Bottom right indicator
    g.setColour(juce::Colour(0xFF663322));
    g.fillEllipse((float)(W - 22), 362.0f, 12.0f, 12.0f);
    g.setColour(juce::Colour(0xFF884433));
    g.drawEllipse((float)(W - 22), 362.0f, 12.0f, 12.0f, 1.0f);
}

void VelvetEvilEditor::resized()
{
    // SIDE-BY-SIDE horizontal layout
    wobbleKnob.setBounds(48,  118, 150, 150);   // left  knob: center(123, 193)
    reverbKnob.setBounds(222, 118, 150, 150);   // right knob: center(297, 193)
    selectButton.setBounds(12, 360, 85, 26);
}
