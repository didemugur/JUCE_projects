/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SpectrogramAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    SpectrogramAudioProcessorEditor (SpectrogramAudioProcessor&);
    ~SpectrogramAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpectrogramAudioProcessor& audioProcessor;

	juce::Image spectrogramImage; // Image to display the spectrogram

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrogramAudioProcessorEditor)
};
