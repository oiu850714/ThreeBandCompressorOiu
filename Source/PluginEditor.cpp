/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

//==============================================================================
ThreeBandCompressorOiuAudioProcessorEditor::
    ThreeBandCompressorOiuAudioProcessorEditor(
        ThreeBandCompressorOiuAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  setLookAndFeel(&lnf);
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(600, 500);

  addAndMakeVisible(controlBar);
  addAndMakeVisible(analyzer);
  addAndMakeVisible(globalControls);
  addAndMakeVisible(bandControls);
}

ThreeBandCompressorOiuAudioProcessorEditor::
    ~ThreeBandCompressorOiuAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

//==============================================================================
void ThreeBandCompressorOiuAudioProcessorEditor::paint(juce::Graphics& g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
  g.drawFittedText("Hello World!", getLocalBounds(),
                   juce::Justification::centred, 1);
}

void ThreeBandCompressorOiuAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  controlBar.setBounds(bounds.removeFromTop(32));
  bandControls.setBounds(bounds.removeFromBottom(135));
  analyzer.setBounds(bounds.removeFromTop(225));
  globalControls.setBounds(bounds);
}

ThreeBandCompressorOiuAudioProcessorEditor::Placeholder::Placeholder() {
  juce::Random r;
  customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}
