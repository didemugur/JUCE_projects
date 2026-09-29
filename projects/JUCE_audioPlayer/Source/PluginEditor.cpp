/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//AudioPlayerAudioProcessorEditor::AudioPlayerAudioProcessorEditor (AudioPlayerAudioProcessor& p)
  //  : AudioProcessorEditor (&p), audioProcessor (p)
//AudioPlayerEditor::AudioPlayerEditor(AudioPlayerEditor& p)
  //  : AudioPlayerEditor(&p), audioProcessor(p)
AudioPlayerEditor::AudioPlayerEditor(AudioPlayerProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    addAndMakeVisible(open);
    addAndMakeVisible(play);
    addAndMakeVisible(stop);
    addAndMakeVisible(pos);

    open.onClick = [this]
        {
            chooser.launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this](const juce::FileChooser& fc)
                {
                    auto f = fc.getResult();
                    if (f.existsAsFile())
                        audioProcessor.loadFile(f); 
                });
        };

    play.onClick = [this] { audioProcessor.start(); };
    stop.onClick = [this] { audioProcessor.stop(); };

    setSize(400, 300);
    startTimerHz(30); // Update at 30 FPS
}
    

AudioPlayerEditor::~AudioPlayerEditor()
{
}

//==============================================================================
void AudioPlayerEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPlayerEditor::resized()
{
    auto r = getLocalBounds().reduced(10);
    open.setBounds(r.removeFromTop(30).removeFromLeft(100));
    play.setBounds(r.removeFromTop(30).removeFromLeft(100));
    stop.setBounds(r.removeFromTop(30).removeFromLeft(100));
	pos.setBounds(r.removeFromTop(30).removeFromLeft(r.getWidth()));
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}



void AudioPlayerEditor::timerCallback()
{
    if (audioProcessor.isPlaying())
    {
        auto position = audioProcessor.getCurrentPosition();
        auto length = audioProcessor.getLengthInSeconds();
        if (length > 0)
            pos.setValue(position / length, juce::dontSendNotification);
    }
}

//  {
//  if (audioProcessor.isPlaying())
  //      pos.setValue(audioProcessor.getPosition() / audioProcessor.getLength(),
    //        juce::dontSendNotification);
//}