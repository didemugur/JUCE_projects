/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpectrogramAudioProcessorEditor::SpectrogramAudioProcessorEditor (SpectrogramAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 300);

    spectrogramImage = juce::Image(juce::Image::RGB, getWidth(), getHeight(), true);

    startTimerHz(60);
}

SpectrogramAudioProcessorEditor::~SpectrogramAudioProcessorEditor()
{
}

//==============================================================================
void SpectrogramAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.drawImage(spectrogramImage, getLocalBounds().toFloat());
   /* g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);*/
}

void SpectrogramAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void SpectrogramAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.nextFFTBlockReady)
    {
        auto rightHandEdge = spectrogramImage.getWidth() - 1;
        auto imageHeight = spectrogramImage.getHeight();

        spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);

        auto* fftData = audioProcessor.fftData;
        auto fftSize = audioProcessor.fftSize;

        for (int y = 0; y < imageHeight; ++y)
        {
            auto fftIndex = juce::jlimit(0, fftSize / 2, y * fftSize / (2 * imageHeight));
            auto level = juce::jlimit(0.0f, 1.0f, fftData[fftIndex] / 10.0f);

            spectrogramImage.setPixelAt(rightHandEdge, imageHeight - y - 1,
                juce::Colour::fromHSV(level, 1.0f, level, 1.0f));
        }

        audioProcessor.nextFFTBlockReady = false;

        repaint();
    }
}