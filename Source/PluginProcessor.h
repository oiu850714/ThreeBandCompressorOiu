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
  CompressorBand compressor;

  //==============================================================================
  // filter details for each band.
  using Filter = juce::dsp::LinkwitzRileyFilter<float>;
  Filter lowPass, highPass;
  juce::AudioParameterFloat* lowCrossover{nullptr};
  juce::AudioParameterFloat* highCrossover{nullptr};
  std::array<juce::AudioBuffer<float>, 2> filterBuffers;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      ThreeBandCompressorOiuAudioProcessor)
};
