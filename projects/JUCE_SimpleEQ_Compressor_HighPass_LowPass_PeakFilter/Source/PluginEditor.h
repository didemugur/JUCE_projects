
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SimpleEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SimpleEQAudioProcessorEditor(SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:

    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;

    juce::Slider peakFreqSlider;
    juce::Slider peakGainSlider;
    juce::Slider peakQualitySlider;

    juce::Slider lowCutFreqSlider;
    juce::Slider highCutFreqSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakQualityAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowCutFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highCutFreqAttachment;


    juce::ComboBox lowCutSlopeComboBox;
    juce::ComboBox highCutSlopeComboBox;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lowCutSlopeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> highCutSlopeAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& audioProcessor;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleEQAudioProcessorEditor)
};

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
//*/
//
//#pragma once
//
//#include <JuceHeader.h>
//#include "PluginProcessor.h"
//
////==============================================================================
///**
//*/
//class SimpleEQAudioProcessorEditor  : public juce::AudioProcessorEditor
//{
//public:
//    SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor&);
//    ~SimpleEQAudioProcessorEditor() override;
//
//    //==============================================================================
//    void paint (juce::Graphics&) override;
//    void resized() override;
//
//private:
//    // This reference is provided as a quick way for your editor to
//    // access the processor object that created it.
//    SimpleEQAudioProcessor& audioProcessor;
//
//    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
//};
