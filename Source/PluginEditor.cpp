#include "PluginEditor.h"

namespace
{
struct VelvetEditorLayout
{
    static constexpr int baseW = 420;
    static constexpr int baseH = 420;
    static constexpr int clipTimerMs = 80;

    static int x(int w, float r) { return juce::roundToInt((float) w * r); }
    static int y(int h, float r) { return juce::roundToInt((float) h * r); }
    static int s(int w, int h, float r) { return juce::roundToInt((float) juce::jmin(w, h) * r); }

    static juce::Rectangle<int> wobbleKnob(int w, int h)
    {
        const int side = s(w, h, 150.0f / (float) baseW);
        return { x(w, 48.0f / (float) baseW), y(h, 118.0f / (float) baseH), side, side };
    }

    static juce::Rectangle<int> reverbKnob(int w, int h)
    {
        const int side = s(w, h, 150.0f / (float) baseW);
        return { x(w, 222.0f / (float) baseW), y(h, 118.0f / (float) baseH), side, side };
    }

    static juce::Rectangle<int> selectButton(int w, int h)
    {
        return { x(w, 12.0f / (float) baseW), y(h, 360.0f / (float) baseH),
                 x(w, 85.0f / (float) baseW), y(h, 26.0f / (float) baseH) };
    }
};

struct VelvetEditorLook
{
    static constexpr juce::uint32 backgroundBase = 0xFF080505;
    static constexpr juce::uint32 radialInner    = 0xFF1A0A08;
    static constexpr juce::uint32 radialOuter    = 0xFF040202;
    static constexpr juce::uint32 scratchA       = 0x228B0000;
    static constexpr juce::uint32 scratchB       = 0x15CC2200;
    static constexpr juce::uint32 cornerRust     = 0x40660000;
    static constexpr juce::uint32 panelRed       = 0xFF330A0A;
    static constexpr juce::uint32 dividerRed     = 0xFF550000;
    static constexpr juce::uint32 titleRed       = 0xFFCC1111;
    static constexpr juce::uint32 subtitleRed    = 0xFF663333;
    static constexpr juce::uint32 labelBone      = 0xFFCCBBAA;
    static constexpr juce::uint32 clipHot        = 0xFFFF3300;
    static constexpr juce::uint32 clipCold       = 0xFF1A0808;
    static constexpr juce::uint32 lcdFill        = 0xFF0D0804;
    static constexpr juce::uint32 lcdStroke      = 0xFF332211;
    static constexpr juce::uint32 lcdText        = 0xFFFFAA00;
    static constexpr juce::uint32 indicatorRust  = 0xFF663322;
    static constexpr juce::uint32 buttonFill     = 0xFF1A1008;
    static constexpr juce::uint32 buttonText     = 0xFFAA8866;
};

using Layout = VelvetEditorLayout;
using Look = VelvetEditorLook;
}

VelvetEvilEditor::VelvetEvilEditor(VelvetEvilProcessor& p)
    : AudioProcessorEditor(&p), proc(p),
      wobbleAttach(p.apvts, "wobble", wobbleKnob),
      reverbAttach(p.apvts, "reverb", reverbKnob)
{
    setSize(Layout::baseW, Layout::baseH);
    wobbleKnob.setLookAndFeel(&knobLAF);
    reverbKnob.setLookAndFeel(&knobLAF);
    addAndMakeVisible(wobbleKnob);
    addAndMakeVisible(reverbKnob);
    addAndMakeVisible(selectButton);
    selectButton.setColour(juce::TextButton::buttonColourId,  juce::Colour(Look::buttonFill));
    selectButton.setColour(juce::TextButton::textColourOffId, juce::Colour(Look::buttonText));
    selectButton.onClick = [this] {
        proc.selectNextPreset();
        currentPresetName = proc.kPresets[proc.currentPreset].name;
        repaint();
    };
    clipPollTimer = std::make_unique<ClipTimer>(*this);
    clipPollTimer->startTimer(Layout::clipTimerMs);
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

    g.fillAll(juce::Colour(Look::backgroundBase));
    juce::ColourGradient radGrad(juce::Colour(Look::radialInner), W*0.5f, H*0.5f,
                                  juce::Colour(Look::radialOuter), 0.0f, 0.0f, true);
    g.setGradientFill(radGrad);
    g.fillRect(0, 0, W, H);

    g.setColour(juce::Colour(Look::scratchA));
    for (int i = 0; i < 12; ++i)
        g.drawLine((float)(i*38-20), 0.f, (float)(i*38+60), (float)H, 0.6f);
    g.setColour(juce::Colour(Look::scratchB));
    for (int i = 0; i < 8; ++i)
        g.drawLine(0.f, (float)(i*55), (float)W, (float)(i*55+30), 0.4f);

    g.setColour(juce::Colour(Look::cornerRust));
    const int rust = Layout::s(W, H, 100.0f / 420.0f);
    g.fillEllipse(-rust * 3 / 10, -rust * 3 / 10, rust, rust);
    g.fillEllipse(W - rust * 7 / 10, -rust * 3 / 10, rust, rust);
    g.fillEllipse(-rust * 3 / 10, H - rust * 7 / 10, rust, rust);
    g.fillEllipse(W - rust * 7 / 10, H - rust * 7 / 10, rust, rust);

    g.setColour(juce::Colour(Look::panelRed));
    g.fillRect(0, 0, W, Layout::y(H, 58.0f / 420.0f));
    g.setColour(juce::Colour(Look::dividerRed));
    g.fillRect(0, Layout::y(H, 56.0f / 420.0f), W, juce::jmax(1, Layout::y(H, 2.0f / 420.0f)));

    g.setColour(juce::Colour(Look::titleRed));
    g.setFont(juce::Font((float) Layout::s(W, H, 24.0f / 420.0f), juce::Font::bold));
    g.drawText("VELVET EVIL", 0, Layout::y(H, 8.0f / 420.0f), W, Layout::y(H, 28.0f / 420.0f), juce::Justification::centred);
    g.setColour(juce::Colour(Look::subtitleRed));
    g.setFont(juce::Font((float) Layout::s(W, H, 10.0f / 420.0f), juce::Font::italic));
    g.drawText("~ FIND YOURSELVES ~", 0, Layout::y(H, 36.0f / 420.0f), W, Layout::y(H, 16.0f / 420.0f), juce::Justification::centred);

    g.setColour(juce::Colour(Look::labelBone));
    g.setFont(juce::Font((float) Layout::s(W, H, 11.0f / 420.0f), juce::Font::bold));
    g.drawText("DARK MELODY",  Layout::x(W, 48.0f / 420.0f), Layout::y(H, 278.0f / 420.0f), Layout::x(W, 160.0f / 420.0f), Layout::y(H, 16.0f / 420.0f), juce::Justification::centred);
    g.drawText("GRIT",        Layout::x(W, 212.0f / 420.0f), Layout::y(H, 278.0f / 420.0f), Layout::x(W, 160.0f / 420.0f), Layout::y(H, 16.0f / 420.0f), juce::Justification::centred);

    const bool lc = proc.isLeftClipping();
    const bool rc = proc.isRightClipping();
    const int led = Layout::s(W, H, 10.0f / 420.0f);
    g.setColour(lc ? juce::Colour(Look::clipHot) : juce::Colour(Look::clipCold));
    g.fillEllipse((float) Layout::x(W, 8.0f / 420.0f), (float) Layout::y(H, 8.0f / 420.0f), (float) led, (float) led);
    g.setColour(rc ? juce::Colour(Look::clipHot) : juce::Colour(Look::clipCold));
    g.fillEllipse((float)(W - Layout::x(W, 18.0f / 420.0f)), (float) Layout::y(H, 8.0f / 420.0f), (float) led, (float) led);
    if (lc || rc) proc.resetClipLEDs();

    g.setColour(juce::Colour(Look::lcdFill));
    g.fillRoundedRectangle((float) Layout::x(W, 60.0f / 420.0f), (float) Layout::y(H, 308.0f / 420.0f), (float) Layout::x(W, 300.0f / 420.0f), (float) Layout::y(H, 38.0f / 420.0f), (float) Layout::s(W, H, 4.0f / 420.0f));
    g.setColour(juce::Colour(Look::lcdStroke));
    g.drawRoundedRectangle((float) Layout::x(W, 60.0f / 420.0f), (float) Layout::y(H, 308.0f / 420.0f), (float) Layout::x(W, 300.0f / 420.0f), (float) Layout::y(H, 38.0f / 420.0f), (float) Layout::s(W, H, 4.0f / 420.0f), 1.f);
    g.setColour(juce::Colour(Look::lcdText));
    g.setFont(juce::Font((float) Layout::s(W, H, 16.0f / 420.0f), juce::Font::bold));
    g.drawText(currentPresetName, Layout::x(W, 60.0f / 420.0f), Layout::y(H, 308.0f / 420.0f), Layout::x(W, 300.0f / 420.0f), Layout::y(H, 38.0f / 420.0f), juce::Justification::centred);

    g.setColour(juce::Colour(Look::panelRed));
    g.fillRect(0, Layout::y(H, 356.0f / 420.0f), W, H - Layout::y(H, 356.0f / 420.0f));
    g.setColour(juce::Colour(Look::dividerRed));
    g.fillRect(0, Layout::y(H, 356.0f / 420.0f), W, 1);

    g.setColour(juce::Colour(Look::indicatorRust));
    const int dot = Layout::s(W, H, 12.0f / 420.0f);
    g.fillEllipse((float)(W - Layout::x(W, 22.0f / 420.0f)), (float) Layout::y(H, 362.0f / 420.0f), (float) dot, (float) dot);
}

void VelvetEvilEditor::resized()
{
    const int W = getWidth();
    const int H = getHeight();
    wobbleKnob.setBounds(Layout::wobbleKnob(W, H));
    reverbKnob.setBounds(Layout::reverbKnob(W, H));
    selectButton.setBounds(Layout::selectButton(W, H));
}
