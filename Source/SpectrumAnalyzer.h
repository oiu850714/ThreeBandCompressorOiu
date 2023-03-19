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

struct SpectrumAnalyzer : juce::Component, juce::Timer {
  SpectrumAnalyzer(ThreeBandCompressorOiuAudioProcessor&);

  void timerCallback() override;

  void paint(juce::Graphics& g) override;
  void resized() override;

  void toggleAnalysisEnablement(bool enabled) {
    shouldShowFFTAnalysis = enabled;
  }

  void updateGainReductions(float lowBandGR, float midBandGR, float highBandGR);

 private:
  ThreeBandCompressorOiuAudioProcessor& audioProcessor;

  bool shouldShowFFTAnalysis = true;

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

  void drawFFTAnalysis(juce::Graphics& g, juce::Rectangle<int> analysisArea);

  // Used to draw band crossover and band thresholds
  juce::AudioParameterFloat* lowMidXoverParam{nullptr};
  juce::AudioParameterFloat* midHighXoverParam{nullptr};
  juce::AudioParameterFloat* lowBandThredholdParam{nullptr};
  juce::AudioParameterFloat* midBandThredholdParam{nullptr};
  juce::AudioParameterFloat* highBandThredholdParam{nullptr};
  // Used to draw gain reduction for each band compressor
  float lowBandGainReduction{0.f};
  float midBandGainReduction{0.f};
  float highBandGainReduction{0.f};
  void drawBandParameterLinesAndGainReductions(
      juce::Graphics& g, juce::Rectangle<int> backgroundBounds);
};
