/*
  ==============================================================================

    CompressorBand.h
    Created: 10 Feb 2023 7:19:12pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "ParamIDConst.h"

class CompressorBand {
 public:
  enum CompressorBandType {
    Low,
    Mid,
    High,
  };
  CompressorBand(const juce::AudioProcessorValueTreeState &apvts,
                 CompressorBandType);
  void prepare(const juce::dsp::ProcessSpec &spec) { compressor.prepare(spec); }

  void updateCompressorSettings();

  void process(juce::AudioBuffer<float> &buffer);

  bool isSolo() const { return solo->get(); }
  bool isMute() const { return mute->get(); }
  bool isBypassed() const { return bypassed->get(); }

  float getInputRMSLevelDb() const { return inputRMSLevelDb.load(); }
  float getOutputRMSLevelDb() const { return outputRMSLevelDb.load(); }

 private:
  // Cached pointers for the parameters of apvts.
  juce::AudioParameterFloat *attack{nullptr};
  juce::AudioParameterFloat *release{nullptr};
  juce::AudioParameterFloat *threshold{nullptr};
  juce::AudioParameterChoice *ratio{nullptr};
  juce::AudioParameterBool *bypassed{nullptr};
  juce::AudioParameterBool *mute{nullptr};
  juce::AudioParameterBool *solo{nullptr};

  //==============================================================================
  juce::dsp::Compressor<float> compressor;

  std::atomic<float> inputRMSLevelDb;
  std::atomic<float> outputRMSLevelDb;
};
