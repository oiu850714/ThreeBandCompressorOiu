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
      : channelFifo(&scsf) {
    channelFFTDataGenerator.changeOrder(FFTOrder::order2048);
    monoBuffer.setSize(1, channelFFTDataGenerator.getFFTSize());
  }
  void process(juce::Rectangle<float> fftBounds, double sampleRate);
  juce::Path getPath() { return channelFFTPath; }

  void setNegativeInfinity(float val) noexcept { negativeInfinity = val; }

 private:
  SingleChannelSampleFifo<ThreeBandCompressorOiuAudioProcessor::BlockType>*
      channelFifo;

  juce::AudioBuffer<float> monoBuffer;

  FFTDataGenerator<std::vector<float>> channelFFTDataGenerator;

  AnalyzerPathGenerator<juce::Path> pathProducer;

  juce::Path channelFFTPath;

  float negativeInfinity{-48.f};
};
