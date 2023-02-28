/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "CompressorBandControlsComponent.h"
#include "ControlBar.h"
#include "CustomLookAndFeel.h"
#include "CustomSlider.h"
#include "GlobalControlsComponent.h"
#include "PluginProcessor.h"
#include "SpectrumAnalyzer.h"

//==============================================================================
/**
 */
class ThreeBandCompressorOiuAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::Timer {
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

  CustomLookAndFeel lnf;

  GlobalControls globalControls{audioProcessor.apvts};
  CompressorBandControls bandControls{audioProcessor.apvts};
  SpectrumAnalyzer analyzer{audioProcessor};
  ControlBar controlBar;

  // Used to update input/output RMS level for each compressor band.
  void timerCallback() override;

  void toggleGlobalBypassState();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      ThreeBandCompressorOiuAudioProcessorEditor)
};
