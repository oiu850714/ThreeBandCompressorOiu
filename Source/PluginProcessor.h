/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "CompressorBand.h"

//==============================================================================
/**
 */
class ThreeBandCompressorOiuAudioProcessor
    : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    ,
      public juce::AudioProcessorARAExtension
#endif
{
 public:
  //==============================================================================
  ThreeBandCompressorOiuAudioProcessor();
  ~ThreeBandCompressorOiuAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  //==============================================================================
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState apvts;

 private:
  //==============================================================================
  std::array<CompressorBand, 3> compressors;
  // Create alias for getting and setting individual compressor's parameters.
  CompressorBand &lowBandCompressor = compressors[0],
                 &midBandCompressor = compressors[1],
                 highBandCompressor = compressors[2];

  //==============================================================================
  // See: https://youtu.be/Mo0Oco3Vimo?t=9033
  // Filter details for each band.
  // Make all three bands experience same "delay".
  using Filter = juce::dsp::LinkwitzRileyFilter<float>;
  // clang-format off
  //        fc0    fc1
  Filter    LP1,   AP2;
  Filter    HP1,   LP2;
  Filter           HP2;
  // clang-format on

  juce::AudioParameterFloat* lowMidCrossover{nullptr};
  juce::AudioParameterFloat* midHighCrossover{nullptr};

  std::array<juce::AudioBuffer<float>, 3> filterBuffers;

  juce::dsp::Gain<float> inputGain, outputGain;
  juce::AudioParameterFloat *inputGainParam{nullptr};
  juce::AudioParameterFloat *outputGainParam{nullptr};

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      ThreeBandCompressorOiuAudioProcessor)
};
