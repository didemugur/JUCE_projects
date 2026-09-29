/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPlayerProcessor::AudioPlayerProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    formatManager.registerBasicFormats();
}

AudioPlayerProcessor::~AudioPlayerProcessor()
{
}

//==============================================================================
const juce::String AudioPlayerProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPlayerProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPlayerProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPlayerProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPlayerProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPlayerProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPlayerProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPlayerProcessor::setCurrentProgram (int index)
{
}

const juce::String AudioPlayerProcessor::getProgramName (int index)
{
    return {};
}

void AudioPlayerProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AudioPlayerProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    transportSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void AudioPlayerProcessor::releaseResources()
{
	transportSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioPlayerProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioPlayerProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    transportSource.getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));

   // {
    //    juce::AudioSourceChannelInfo info(&buf, 0, buf.getNumSamples());
      //  if (readerSource.get() == nullptr)
        //    info.clearActiveBufferRegion();
 //       else
   //         transportSource.getNextAudioBlock(info);
 //   }
//    juce::ScopedNoDenormals noDenormals;
  //  auto totalNumInputChannels  = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();
//
  //
//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
  //      buffer.clear (i, 0, buffer.getNumSamples());
    //
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
  //  {
  //      auto* channelData = buffer.getWritePointer (channel);
    //
        // ..do something to the data...
  //  }
}

void AudioPlayerProcessor::loadFile(juce::File f)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(f));
    if (reader.get() != nullptr)
    {
        auto sampleRate = reader->sampleRate;
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader.release(), true));
        transportSource.setSource(newSource.get(), 0, nullptr, sampleRate);
        readerSource.reset(newSource.release());
    }
}

bool AudioPlayerProcessor::isPlaying() const
{
    return transportSource.isPlaying();
}
//==============================================================================
bool AudioPlayerProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPlayerProcessor::createEditor()
{
    return new AudioPlayerEditor(*this);
}
//==============================================================================
void AudioPlayerProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AudioPlayerProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

double AudioPlayerProcessor::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

double AudioPlayerProcessor::getLengthInSeconds() const
{
    return transportSource.getLengthInSeconds();
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPlayerProcessor();
}
