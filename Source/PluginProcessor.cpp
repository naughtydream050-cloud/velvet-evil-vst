#include "PluginProcessor.h"
#include "PluginEditor.h"

const std::array<VelvetEvilProcessor::Preset, 4> VelvetEvilProcessor::kPresets = {{
    { "SANCTUARY", 0.20f, 0.10f, 0.20f, 12000.0f,  0.0f },
    { "BLOOD",     0.50f, 0.45f, 0.55f,  8000.0f, -1.0f },
    { "CRIMSON",   0.75f, 0.85f, 0.70f, 15000.0f,  1.5f },
    { "WHISPER",   0.15f, 0.05f, 0.15f,  4000.0f, -3.0f }
}};

juce::AudioProcessorValueTreeState::ParameterLayout
VelvetEvilProcessor::createParameterLayout()
{
    using P = juce::AudioParameterFloat;
    using I = juce::ParameterID;
    using R = juce::NormalisableRange<float>;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<P>(I("drive",1),      "Dark Melody", R(0.0f,1.0f),0.5f));
    layout.add(std::make_unique<P>(I("grit",1),       "Grit",        R(0.0f,1.0f),0.0f));
    layout.add(std::make_unique<P>(I("saturation",1), "Saturation",  R(0.0f,1.0f),0.5f));
    layout.add(std::make_unique<P>(I("lpf",1),        "LPF",         R(800.0f,20000.0f,0.0f,0.4f),8000.0f));
    layout.add(std::make_unique<P>(I("gain",1),       "Gain",        R(-12.0f,12.0f),0.0f));
    return layout;
}

VelvetEvilProcessor::VelvetEvilProcessor()
    : AudioProcessor(BusesProperties()
          .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{}

bool VelvetEvilProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    return l.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
        && l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void VelvetEvilProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate;
    driveSmooth.reset(sampleRate, 0.05);
    gritSmooth .reset(sampleRate, 0.05);
    satSmooth  .reset(sampleRate, 0.05);
    lpfSmooth  .reset(sampleRate, 0.05);
    gainSmooth .reset(sampleRate, 0.05);
    driveSmooth.setCurrentAndTargetValue(*apvts.getRawParameterValue("drive"));
    gritSmooth .setCurrentAndTargetValue(*apvts.getRawParameterValue("grit"));
    satSmooth  .setCurrentAndTargetValue(*apvts.getRawParameterValue("saturation"));
    lpfSmooth  .setCurrentAndTargetValue(*apvts.getRawParameterValue("lpf"));
    gainSmooth .setCurrentAndTargetValue(*apvts.getRawParameterValue("gain"));
    lpfZ[0] = lpfZ[1] = 0.0f;
}

void VelvetEvilProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    driveSmooth.setTargetValue(*apvts.getRawParameterValue("drive"));
    gritSmooth .setTargetValue(*apvts.getRawParameterValue("grit"));
    satSmooth  .setTargetValue(*apvts.getRawParameterValue("saturation"));
    lpfSmooth  .setTargetValue(*apvts.getRawParameterValue("lpf"));
    gainSmooth .setTargetValue(*apvts.getRawParameterValue("gain"));

    const int numCh = buffer.getNumChannels();
    const int N     = buffer.getNumSamples();
    const float fs  = (float)currentSampleRate;

    for (int i = 0; i < N; ++i)
    {
        const float drive  = driveSmooth.getNextValue();
        const float grit   = gritSmooth .getNextValue();
        const float sat    = satSmooth  .getNextValue();
        const float fc     = lpfSmooth  .getNextValue();
        const float gainDB = gainSmooth .getNextValue();

        const float preGain  = 1.0f + drive * 19.0f;
        const float alpha    = 1.0f - std::exp(-juce::MathConstants<float>::twoPi * fc / fs);
        const float outGain  = std::pow(10.0f, gainDB * 0.05f);

        for (int ch = 0; ch < juce::jmin(numCh, 2); ++ch)
        {
            float x      = buffer.getSample(ch, i) * preGain;
            float soft   = x / (1.0f + std::abs(x));
            float hard   = juce::jlimit(-1.0f, 1.0f, x);
            float clipped = (1.0f - grit) * soft + grit * hard;
            float dry    = buffer.getSample(ch, i);
            float y      = (1.0f - sat) * dry + sat * clipped;
            lpfZ[ch] += alpha * (y - lpfZ[ch]);
            buffer.setSample(ch, i, lpfZ[ch] * outGain);
        }
    }
}

void VelvetEvilProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}

void VelvetEvilProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* VelvetEvilProcessor::createEditor()
{
    return new VelvetEvilEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VelvetEvilProcessor();
}
