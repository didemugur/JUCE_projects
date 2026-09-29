/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>

//==============================================================================
SpectrogramAudioProcessor::SpectrogramAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    forwardFFT(fftOrder),
    window(fftSize, juce::dsp::WindowingFunction<float>::hann)
#endif
{
}

SpectrogramAudioProcessor::~SpectrogramAudioProcessor()
{
}

//==============================================================================
const juce::String SpectrogramAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpectrogramAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpectrogramAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpectrogramAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpectrogramAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpectrogramAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpectrogramAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpectrogramAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpectrogramAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpectrogramAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SpectrogramAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    forwardFFT = juce::dsp::FFT(fftOrder);
   
    fifoIndex = 0;
    nextFFTBlockReady = false;
}

void SpectrogramAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpectrogramAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SpectrogramAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //auto numInputChannels = getTotalNumInputChannels();
    //auto numSamples = buffer.getNumSamples();

    //for (int i = 0; i < numSamples; ++i)
    //{
    //    float sample = 0.0f;

    //    
    //    if (numInputChannels >= 2)
    //    {
    //        auto left = buffer.getReadPointer(0)[i];
    //        auto right = buffer.getReadPointer(1)[i];
    //        sample = 0.5f * (left + right);
    //    }
    //    else if (numInputChannels == 1)
    //    {
    //        sample = buffer.getReadPointer(0)[i];
    //    }

    //    pushNextSampleIntoFifo(sample);
    //}
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 0.0f;

        if (totalNumInputChannels >= 2)
        {
            auto left = buffer.getReadPointer(0)[i];
            auto right = buffer.getReadPointer(1)[i];
            sample = 0.5f * (left + right);
        }
        else if (totalNumInputChannels == 1)
        {
            sample = buffer.getReadPointer(0)[i];
        }

        pushNextSampleIntoFifo(sample);
    }
    /*if (totalNumInputChannels >= 2)
    {
        auto left = buffer.getReadPointer(0)[i];
        auto right = buffer.getReadPointer(1)[i];
        sample = 0.5f * (left + right);
    }
    else if (totalNumInputChannels == 1)
    {
        sample = buffer.getReadPointer(0)[i];
    }

    pushNextSampleIntoFifo(sample);*/
}

    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);

    //    // ..do something to the data...
    //}

void SpectrogramAudioProcessor::pushNextSampleIntoFifo (float sample)
{
    if (fifoIndex == fftSize)
        return;

    fifo[fifoIndex++] = sample;

    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            juce::zeromem(fftData, sizeof(fftData));
            memcpy(fftData, fifo, sizeof(fifo));

            window.multiplyWithWindowingTable(fftData, fftSize);
            forwardFFT.performFrequencyOnlyForwardTransform(fftData);

            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }
    //// Push the sample into the FIFO buffer
    //fifo[fifoIndex++] = sample;
    //// If the FIFO buffer is full, prepare for FFT processing
    //if (fifoIndex == fftSize)
    //{
    //    fifoIndex = 0;
    //    nextFFTBlockReady = true;
    //    // Apply windowing function to the FIFO buffer
    //    for (int i = 0; i < fftSize; ++i)
    //    {
    //        fftData[i * 2] = fifo[i] * window[i]; // Real part
    //        fftData[i * 2 + 1] = 0.0f;           // Imaginary part
    //    }
    //}
}

//==============================================================================
bool SpectrogramAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpectrogramAudioProcessor::createEditor()
{
    return new SpectrogramAudioProcessorEditor (*this);
}

//==============================================================================
void SpectrogramAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SpectrogramAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpectrogramAudioProcessor();
}
