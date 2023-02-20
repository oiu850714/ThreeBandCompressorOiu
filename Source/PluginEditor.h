/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "GlobalControlsComponent.h"

//==============================================================================
/**
 */
class ThreeBandCompressorOiuAudioProcessorEditor
    : public juce::AudioProcessorEditor {
 public:
  ThreeBandCompressorOiuAudioProcessorEditor(
      ThreeBandCompressorOiuAudioProcessor&);
  ~ThreeBandCompressorOiuAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;

 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ThreeBandCompressorOiuAudioProcessor& audioProcessor;

  struct Placeholder : public juce::Component {
    Placeholder();
    void paint(juce::Graphics& g) override { g.fillAll(customColor); }

    juce::Colour customColor;
  };

  GlobalControls globalControls {audioProcessor.apvts};
  Placeholder controlBar, analyzer, bandControls;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      ThreeBandCompressorOiuAudioProcessorEditor)
};
