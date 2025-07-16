/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class AudioPlayerProcessor  : public juce::AudioProcessor
{
public:
   // AudioPlayerAudioProcessor();    
   // ~AudioPlayerAudioProcessor() override;

    //==============================================================================
    AudioPlayerProcessor();
    ~AudioPlayerProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

    void processBlock(juce::AudioBuffer<float>& buf, juce::MidiBuffer&) override;

    bool isPlaying() const;

    double getCurrentPosition() const;

    double getLengthInSeconds() const;

    void loadFile(juce::File f);
    //{
   //     std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(f));
        //
     //   if (reader.get() != nullptr)
   //     {
 //           auto sampleRate = reader->sampleRate;
            //
     //       std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader.release(), true));
   //         transportSource.setSource(newSource.get(), 0, nullptr, sampleRate);
 //           readerSource.reset(newSource.release());
        //}
       // auto r = std::unique_ptr<juce::AudioFormatReader>
		//	(formatManager.createReaderFor(f));
    //    if (r != nullptr)
      //  {
        //    readerSource.reset(new juce::AudioFormatReaderSource(r.release(), true));
          //  transportSource.setSource(readerSource.get(), 0, readerSource->sampleRate());
        //}
	//}

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void start() { transportSource.start(); }
	void stop() { transportSource.stop(); }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayerProcessor)
};
