
#include "PluginProcessor.h"
#include "PluginEditor.h" 
juce::AudioProcessorValueTreeState::ParameterLayout
SimpleEQAudioProcessor::createParameterLayout()
{
    using namespace juce;

    AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<AudioParameterFloat>("Threshold", "Threshold", -60.0f, 0.0f, -20.0f));
    layout.add(std::make_unique<AudioParameterFloat>("Ratio", "Ratio", 1.0f, 20.0f, 2.0f));
    layout.add(std::make_unique<AudioParameterFloat>("Attack", "Attack", 1.0f, 200.0f, 20.0f));
    layout.add(std::make_unique<AudioParameterFloat>("Release", "Release", 10.0f, 1000.0f, 250.0f));

    layout.add(std::make_unique<AudioParameterFloat>("LowCut Freq", "LowCut Freq",
        NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 20.f));

    layout.add(std::make_unique<AudioParameterFloat>("HighCut Freq", "HighCut Freq",
        NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 20000.f));

    layout.add(std::make_unique<AudioParameterFloat>("Peak Freq", "Peak Freq",
        NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 750.f));

    layout.add(std::make_unique<AudioParameterFloat>("Peak Gain", "Peak Gain",
        NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), 0.f));

    layout.add(std::make_unique<AudioParameterFloat>("Peak Quality", "Peak Quality",
        NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 1.f));

    StringArray slopeChoices;
    for (int i = 0; i < 4; ++i)
        slopeChoices.add(String(12 + i * 12) + " dB/oct");

    layout.add(std::make_unique<AudioParameterChoice>("LowCut Slope", "LowCut Slope", slopeChoices, 0));
    layout.add(std::make_unique<AudioParameterChoice>("HighCut Slope", "HighCut Slope", slopeChoices, 0));

    return layout;
}

static void updateCutFilter(
    CutFilter& chain,
    const Coefficients& replacements,
    Slope slope)
{
    // bypass all first
    chain.setBypassed<0>(true);
    chain.setBypassed<1>(true);
    chain.setBypassed<2>(true);
    chain.setBypassed<3>(true);

    if (slope >= Slope_48)
    {
        updateCoefficients(chain.get<Slope_48>().coefficients, replacements);
        chain.setBypassed<Slope_48>(false);
    }

    if (slope >= Slope_36)
    {
        updateCoefficients(chain.get<Slope_36>().coefficients, replacements);
        chain.setBypassed<Slope_36>(false);
    }

    if (slope >= Slope_24)
    {
        updateCoefficients(chain.get<Slope_24>().coefficients, replacements);
        chain.setBypassed<Slope_24>(false);
    }

    if (slope >= Slope_12)
    {
        updateCoefficients(chain.get<Slope_12>().coefficients, replacements);
        chain.setBypassed<Slope_12>(false);
    }

}

//==============================================================================
//  Fetch GUI values into a struct ---------------------------------------------

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings s;
	s.threshold = apvts.getRawParameterValue("Threshold")->load();
	s.ratio = apvts.getRawParameterValue("Ratio")->load();
	s.attack = apvts.getRawParameterValue("Attack")->load();
	s.release = apvts.getRawParameterValue("Release")->load();

    s.lowCutFreq = apvts.getRawParameterValue("LowCut Freq")->load();
    s.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load();
    s.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    s.peakGainInDecibels = apvts.getRawParameterValue("Peak Gain")->load();
    s.peakQuality = apvts.getRawParameterValue("Peak Quality")->load();

    s.lowCutSlope = static_cast<Slope> (static_cast<int> (apvts.getRawParameterValue("LowCut Slope")->load()));
    s.highCutSlope = static_cast<Slope> (static_cast<int> (apvts.getRawParameterValue("HighCut Slope")->load()));

    return s;
}

//==============================================================================
//  Coefficient factories -------------------------------------------------------

inline auto makeLowCutFilter(const ChainSettings& cs, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
        cs.lowCutFreq, sampleRate, 2 * (cs.lowCutSlope + 1));
}

inline auto makeHighCutFilter(const ChainSettings& cs, double sampleRate)
{
    return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
        cs.highCutFreq, sampleRate, 2 * (cs.highCutSlope + 1));
}

//==============================================================================
//  Construction / destruction --------------------------------------------------

SimpleEQAudioProcessor::SimpleEQAudioProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect && !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // register all parameters for callbacks
    const juce::StringArray ids{ "LowCut Freq", "LowCut Slope",
                                  "HighCut Freq", "HighCut Slope",
                                  "Peak Freq", "Peak Gain", "Peak Quality" };

    for (auto& id : ids)
        apvts.addParameterListener(id, this);
}

SimpleEQAudioProcessor::~SimpleEQAudioProcessor() = default;

//==============================================================================
//  Prepare / layout -----------------------------------------------------------

bool SimpleEQAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // only mono or stereo in/out
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    return true;
}

void SimpleEQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = 1;          

    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
	updateCompressor(getChainSettings(apvts));
    updateFilters();

    
    
}

//==============================================================================
//  Processing -----------------------------------------------------------------

void SimpleEQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // clear any non-used output channels
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getNumChannels() > 1 ? block.getSingleChannelBlock(1)
        : juce::dsp::AudioBlock<float>();

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    leftChain.process(leftContext);

    if (block.getNumChannels() > 1)
    {
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
        rightChain.process(rightContext);
    }
}

//==============================================================================
//  Filter refresh helpers -----------------------------------------------------

void SimpleEQAudioProcessor::updateCompressor(const ChainSettings& cs)
{
 

    auto& leftCompressor = leftChain.get<0>();
    auto& rightCompressor = rightChain.get<0>();

    leftCompressor.setThreshold(cs.threshold);
    leftCompressor.setRatio(cs.ratio);
    leftCompressor.setAttack(cs.attack);
    leftCompressor.setRelease(cs.release);

	rightCompressor.setThreshold(cs.threshold);
	rightCompressor.setRatio(cs.ratio);
	rightCompressor.setAttack(cs.attack);
	rightCompressor.setRelease(cs.release);

    
	
}

void SimpleEQAudioProcessor::updatePeakFilter(const ChainSettings& cs)
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        getSampleRate(),
        cs.peakFreq,
        cs.peakQuality,
        juce::Decibels::decibelsToGain(cs.peakGainInDecibels));

    updateCoefficients(leftChain.get<Peak>().coefficients, *coeffs);
    updateCoefficients(rightChain.get<Peak>().coefficients, *coeffs);
}

void SimpleEQAudioProcessor::updateLowCutFilters(const ChainSettings& cs)
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), cs.lowCutFreq);

    auto& left = leftChain.get<LowCut>();
    auto& right = rightChain.get<LowCut>();

    updateCutFilter(left, *coeffs, cs.lowCutSlope);
    updateCutFilter(right, *coeffs, cs.lowCutSlope);
}

void SimpleEQAudioProcessor::updateHighCutFilters(const ChainSettings& cs)
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), cs.highCutFreq);

    auto& left = leftChain.get<HighCut>();
    auto& right = rightChain.get<HighCut>();

    updateCutFilter(left, *coeffs, cs.highCutSlope);
    updateCutFilter(right, *coeffs, cs.highCutSlope);
}

void SimpleEQAudioProcessor::updateFilters()
{
    const auto cs = getChainSettings(apvts);

	updateCompressor(cs);
    updateLowCutFilters(cs);
    updatePeakFilter(cs);
    updateHighCutFilters(cs);


}

//==============================================================================
//  Parameter listener ---------------------------------------------------------

void SimpleEQAudioProcessor::parameterChanged(const juce::String&, float)
{
    // defer filter rebuild to message thread – keeps audio thread real-time safe
    juce::MessageManager::callAsync([this] { updateFilters(); });

    
}

//==============================================================================
//  GUI ------------------------------------------------------------------------

juce::AudioProcessorEditor* SimpleEQAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void SimpleEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void SimpleEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
        updateFilters();
    }
}

//==============================================================================
//  Factory --------------------------------------------------------------------

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleEQAudioProcessor();
}
