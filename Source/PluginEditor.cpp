/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
	thresholdSlider.setSliderStyle(juce::Slider::Rotary);
	thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
	addAndMakeVisible(thresholdSlider);
	thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.getAPVTS(), "Threshold", thresholdSlider);
    
	ratioSlider.setSliderStyle(juce::Slider::Rotary);
	ratioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
	addAndMakeVisible(ratioSlider);
	ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.getAPVTS(), "Ratio", ratioSlider);

	attackSlider.setSliderStyle(juce::Slider::Rotary);
	attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
	addAndMakeVisible(attackSlider);
	attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.getAPVTS(), "Attack", attackSlider);

	releaseSlider.setSliderStyle(juce::Slider::Rotary);
	releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
	addAndMakeVisible(releaseSlider);
	releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		audioProcessor.getAPVTS(), "Release", releaseSlider);

    peakFreqSlider.setSliderStyle(juce::Slider::Rotary);
    peakFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(peakFreqSlider);

    peakGainSlider.setSliderStyle(juce::Slider::Rotary);
    peakGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(peakGainSlider);

    peakQualitySlider.setSliderStyle(juce::Slider::Rotary);
    peakQualitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(peakQualitySlider);

    peakFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "Peak Freq", peakFreqSlider);

    peakGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "Peak Gain", peakGainSlider);

    peakQualityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "Peak Quality", peakQualitySlider);
  

    lowCutFreqSlider.setSliderStyle(juce::Slider::Rotary);
    lowCutFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(lowCutFreqSlider);

    lowCutFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "LowCut Freq", lowCutFreqSlider);

   

   
    // LowCut Slope combo box
    lowCutSlopeComboBox.addItem("12 dB/oct", 1);
    lowCutSlopeComboBox.addItem("24 dB/oct", 2);
    lowCutSlopeComboBox.addItem("36 dB/oct", 3);
    lowCutSlopeComboBox.addItem("48 dB/oct", 4);
    addAndMakeVisible(lowCutSlopeComboBox);
    lowCutSlopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "LowCut Slope", lowCutSlopeComboBox);

     int lowSlopeValue = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("LowCut Slope")->load());
    lowCutSlopeComboBox.setSelectedItemIndex(lowSlopeValue + 1);

    highCutFreqSlider.setSliderStyle(juce::Slider::Rotary);
    highCutFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(highCutFreqSlider);

    highCutFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "HighCut Freq", highCutFreqSlider);

    highCutSlopeComboBox.addItem("12 dB/oct", 1);
    highCutSlopeComboBox.addItem("24 dB/oct", 2);
    highCutSlopeComboBox.addItem("36 dB/oct", 3);
    highCutSlopeComboBox.addItem("48 dB/oct", 4);
    addAndMakeVisible(highCutSlopeComboBox);

    highCutSlopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "HighCut Slope", highCutSlopeComboBox);

    int highSlopeValue = static_cast<int>(audioProcessor.getAPVTS().getRawParameterValue("HighCut Slope")->load());
    highCutSlopeComboBox.setSelectedItemIndex(highSlopeValue + 1);  

    setSize(1000, 600); 
}
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
}

//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleEQAudioProcessorEditor::resized()
{
    

	auto area = getLocalBounds().reduced(20);
    auto topArea = area.removeFromTop(100);
    auto bottomArea = area.removeFromBottom(100);
    thresholdSlider.setBounds(topArea.removeFromLeft(100).reduced(10));
    ratioSlider.setBounds(topArea.removeFromLeft(100).reduced(10));
    attackSlider.setBounds(topArea.removeFromLeft(100).reduced(10));
    releaseSlider.setBounds(topArea.removeFromLeft(100).reduced(10));
    peakFreqSlider.setBounds(bottomArea.removeFromLeft(100).reduced(10));
    peakGainSlider.setBounds(bottomArea.removeFromLeft(100).reduced(10));
    peakQualitySlider.setBounds(bottomArea.removeFromLeft(100).reduced(10));
    lowCutFreqSlider.setBounds(bottomArea.removeFromLeft(100).reduced(10));
    lowCutSlopeComboBox.setBounds(lowCutFreqSlider.getRight() + 10, lowCutFreqSlider.getY(), 100, 30);
    highCutFreqSlider.setBounds(bottomArea.removeFromTop(80).removeFromLeft(150).reduced(10));
	highCutSlopeComboBox.setBounds(highCutFreqSlider.getRight() + 10, highCutFreqSlider.getY(), 100, 30);
}
