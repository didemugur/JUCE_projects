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
class AudioPlayerProcessor; // Forward declaration of AudioPlayerProcessor  

class AudioPlayerEditor : public juce::AudioProcessorEditor,  
                          private juce::Timer  
{  
public:  
    AudioPlayerEditor(AudioPlayerProcessor&);  
    ~AudioPlayerEditor() override;  

    //==============================================================================  
    void paint(juce::Graphics&) override;  
    void resized() override;  
    void timerCallback() override;

private:  
    AudioPlayerProcessor& audioProcessor;  
    juce::FileChooser chooser{ "Select a file" };
    juce::TextButton open{ "Open" }, stop{ "Stop" }, play{ "Play" };  
    juce::Slider pos;  

    // This reference is provided as a quick way for your editor to  
    // access the processor object that created it.  
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayerEditor)  
};
