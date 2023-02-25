/*
  ==============================================================================

    PathProducer.h
    Created: 25 Feb 2023 6:13:49pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AnalyzerPathGenerator.h"
#include "FFTDataGenerator.h"
#include "PluginProcessor.h"
#include "SingleChannelSampleFifo.h"

struct PathProducer {
  PathProducer(
      SingleChannelSampleFifo<ThreeBandCompressorOiuAudioProcessor::BlockType>&
          scsf)
      : leftChannelFifo(&scsf) {
    leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
    monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
  }
  void process(juce::Rectangle<float> fftBounds, double sampleRate);
  juce::Path getPath() { return leftChannelFFTPath; }

 private:
  SingleChannelSampleFifo<ThreeBandCompressorOiuAudioProcessor::BlockType>*
      leftChannelFifo;

  juce::AudioBuffer<float> monoBuffer;

  FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;

  AnalyzerPathGenerator<juce::Path> pathProducer;

  juce::Path leftChannelFFTPath;
};
