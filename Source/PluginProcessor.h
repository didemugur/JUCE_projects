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
class SpectrogramAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SpectrogramAudioProcessor();
    ~SpectrogramAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

    enum
    {
		fftOrder = 11, // FFT order
		fftSize = 1 << fftOrder, // FFT size
    };

	

	//float fifo[fftSize]; // FIFO buffer for FFT input
	//float fftData[fftSize * 2]; // FFT data buffer (real and imaginary parts)
 //   int fifoIndex = 0; // Current index in the FIFO buffer
	//bool nextFFTBlockReady = false; // Flag to indicate if the next FFT block is ready

    bool nextFFTBlockReady = false;
    float fftData[2 * fftSize] = { 0 };


    void pushNextSampleIntoFifo (float sample);
    //==============================================================================
	/*juce::AudioProcessorValueTreeState parameters;*/ // Parameter tree for managing plugin parameters
private:
    /*static constexpr int fftOrder = 11;               
    static constexpr int fftSize = 1 << fftOrder;  */   

   /* juce::dsp::FFT forwardFFT{ fftOrder };            
    juce::dsp::WindowingFunction<float> window;      */

    juce::dsp::FFT forwardFFT; // Forward FFT object
	juce::dsp::WindowingFunction<float> window; // Windowing function for FFT
    
    float fifo[fftSize] = { 0 };                        
    int fifoIndex = 0;
   

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrogramAudioProcessor)
};
