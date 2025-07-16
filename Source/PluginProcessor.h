//==============================================================================
/*
    SimpleEQ — Processor
    ---------------------------------------------------
    Header-only declarations, helper templates and types
*/
//==============================================================================

#pragma once

#include <JuceHeader.h>

//==============================================================================
//--- General DSP helpers
//==============================================================================

enum Slope
{
    Slope_12 = 0,
    Slope_24,
    Slope_36,
    Slope_48
};

struct ChainSettings
{
	float threshold{ -20.f };
	float ratio{ 2.f };
	float attack{ 20.f };
	float release{ 250.f };

    float peakFreq{ 750.f };
    float peakGainInDecibels{ 0.f };
    float peakQuality{ 1.f };

    float lowCutFreq{ 20.f };
    float highCutFreq{ 20000.f };

    Slope lowCutSlope{ Slope_12 };
    Slope highCutSlope{ Slope_12 };
};

// Fetch GUI values -----------------------------------------------------------
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState&);

//==============================================================================
//  Filter type aliases
//==============================================================================

using Filter = juce::dsp::IIR::Filter<float>;
using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>; // up to 48 dB/oct
using MonoChain = juce::dsp::ProcessorChain<juce::dsp::Compressor<float>, CutFilter, Filter, CutFilter>;

enum ChainPositions
{
    Compressor = 0,
    LowCut,
    Peak,
    HighCut
};

using Coefficients = Filter::CoefficientsPtr;

//==============================================================================
//  Low-level helpers
//==============================================================================

// Replaces *old with *replacements
inline void updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;
}

// Assign the Nth stage in a CutFilter ----------------------------------------
//template <int Index, typename ChainType, typename CoeffArray>
//inline void update(ChainType& chain, const CoeffArray& coeffs)
//{
//    updateCoefficients(chain.template get<Index>().coefficients, coeffs);
//    chain.template setBypassed<Index>(false);
//}

// Enable as many biquads as needed for the chosen slope -----------------------


//==============================================================================
//  Forward declarations of coefficient factories
//==============================================================================

inline auto makeLowCutFilter(const ChainSettings&, double sampleRate);
inline auto makeHighCutFilter(const ChainSettings&, double sampleRate);

//==============================================================================
//  Plugin processor
//==============================================================================

class SimpleEQAudioProcessor : public juce::AudioProcessor,
    public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==========================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

    //==========================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout&) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==========================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==========================================================================
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==========================================================================
    int  getNumPrograms() override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    //==========================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void*, int) override;

    //==========================================================================
    //  Parameter callbacks
    void parameterChanged(const juce::String&, float) override;

    //==========================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateCompressor(const ChainSettings& cs);
    void updateFilters();
    void updatePeakFilter(const ChainSettings&);
    void updateLowCutFilters(const ChainSettings&);
    void updateHighCutFilters(const ChainSettings&);

    juce::dsp::Compressor<float> Compressor;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
private:
    // Helper -------------------------------------------------------------------


    // DSP chains ---------------------------------------------------------------
    MonoChain leftChain, rightChain;

    // Parameters ---------------------------------------------------------------
    juce::AudioProcessorValueTreeState apvts;

    
    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessor)
};

