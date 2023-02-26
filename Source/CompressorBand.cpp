/*
  ==============================================================================

    CompressorBand.cpp
    Created: 10 Feb 2023 7:19:12pm
    Author:  oiu85

  ==============================================================================
*/

#include "CompressorBand.h"

CompressorBand::CompressorBand(const juce::AudioProcessorValueTreeState &apvts,
                               CompressorBandType bandType) {
  using namespace Params;
  const auto &params = Params::getParams();

  auto setParam = [&apvts, &params](auto &paramPtr, const auto &paramName) {
    paramPtr = static_cast<std::remove_reference_t<decltype(paramPtr)>>(
        apvts.getParameter(params.at(paramName)));
  };

  switch (bandType) {
    case CompressorBandType::Low: {
      setParam(attack, Names::Attack_Low_Band);
      setParam(release, Names::Release_Low_Band);
      setParam(threshold, Names::Threshold_Low_Band);
      setParam(ratio, Names::Ratio_Low_Band);
      setParam(bypassed, Names::Bypassed_Low_Band);
      setParam(mute, Names::Mute_Low_Band);
      setParam(solo, Names::Solo_Low_Band);
    } break;
    case CompressorBandType::Mid: {
      setParam(attack, Names::Attack_Mid_Band);
      setParam(release, Names::Release_Mid_Band);
      setParam(threshold, Names::Threshold_Mid_Band);
      setParam(ratio, Names::Ratio_Mid_Band);
      setParam(bypassed, Names::Bypassed_Mid_Band);
      setParam(mute, Names::Mute_Mid_Band);
      setParam(solo, Names::Solo_Mid_Band);
    } break;
    case CompressorBandType::High: {
      setParam(attack, Names::Attack_High_Band);
      setParam(release, Names::Release_High_Band);
      setParam(threshold, Names::Threshold_High_Band);
      setParam(ratio, Names::Ratio_High_Band);
      setParam(bypassed, Names::Bypassed_High_Band);
      setParam(mute, Names::Mute_High_Band);
      setParam(solo, Names::Solo_High_Band);
    } break;
  }
}

void CompressorBand::updateCompressorSettings() {
  compressor.setAttack(attack->get());
  compressor.setRelease(release->get());
  compressor.setThreshold(threshold->get());
  compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

static float computeRMSLevel(const juce::AudioBuffer<float> &buffer) noexcept {
  auto numChannels = buffer.getNumChannels();
  auto numSamples = buffer.getNumSamples();
  auto rms = 0.f;
  for (int channel = 0; channel < numChannels; channel++) {
    rms += buffer.getRMSLevel(channel, 0, numSamples);
  }
  return rms / (float) numChannels;
}

void CompressorBand::process(juce::AudioBuffer<float> &buffer) {
  inputRMSLevelDb.store(juce::Decibels::gainToDecibels(computeRMSLevel(buffer)));
  auto dspBlock = juce::dsp::AudioBlock<float>(buffer);
  auto dspContext = juce::dsp::ProcessContextReplacing<float>(dspBlock);
  dspContext.isBypassed = bypassed->get();
  compressor.process(dspContext);
  outputRMSLevelDb.store(juce::Decibels::gainToDecibels(computeRMSLevel(buffer)));
}
