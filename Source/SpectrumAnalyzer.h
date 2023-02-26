/*

 * ==============================================================================


 * SpectrumAnalyzer.h
    Created: 25 Feb 2023 4:51:26pm
    Author:  oiu85


 * ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include "AnalyzerPathGenerator.h"
#include "FFTDataGenerator.h"
#include "PathProducer.h"
#include "PluginProcessor.h"
#include "SingleChannelSampleFifo.h"

struct SpectrumAnalyzer : juce::Component,
                          juce::AudioProcessorParameter::Listener,
                          juce::Timer {
  SpectrumAnalyzer(ThreeBandCompressorOiuAudioProcessor&);
  ~SpectrumAnalyzer();

  void parameterValueChanged(int parameterIndex, float newValue) override;

  void parameterGestureChanged(int parameterIndex,
                               bool gestureIsStarting) override {}

  void timerCallback() override;

  void paint(juce::Graphics& g) override;
  void resized() override;

  void toggleAnalysisEnablement(bool enabled) {
    shouldShowFFTAnalysis = enabled;
  }

 private:
  ThreeBandCompressorOiuAudioProcessor& audioProcessor;

  bool shouldShowFFTAnalysis = true;

  juce::Atomic<bool> parametersChanged{false};

  void drawBackgroundGrid(juce::Graphics& g,
                          juce::Rectangle<int> backgroundBounds);
  void drawTextLabels(juce::Graphics& g, juce::Rectangle<int> backgroundBounds);

  std::vector<float> getFrequencies();
  std::vector<float> getGains();
  std::vector<float> getXs(const std::vector<float>& freqs, float left,
                           float width);

  juce::Rectangle<int> getRenderArea(juce::Rectangle<int> backgroundBounds);

  juce::Rectangle<int> getAnalysisArea(juce::Rectangle<int> backgroundBounds);

  PathProducer leftPathProducer, rightPathProducer;
};
