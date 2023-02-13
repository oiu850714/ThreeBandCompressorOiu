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

 private:
  // Cached pointers for the parameters of apvts.
  juce::AudioParameterFloat *attack{nullptr};
  juce::AudioParameterFloat *release{nullptr};
  juce::AudioParameterFloat *threshold{nullptr};
  juce::AudioParameterChoice *ratio{nullptr};
  juce::AudioParameterBool *bypassed{nullptr};

  //==============================================================================
  juce::dsp::Compressor<float> compressor;
};
