/*
  ==============================================================================

    CompressorBand.cpp
    Created: 10 Feb 2023 7:19:12pm
    Author:  oiu85

  ==============================================================================
*/

#include "CompressorBand.h"

CompressorBand::CompressorBand(
    const juce::AudioProcessorValueTreeState &apvts) {
  using namespace Params;
  const auto &params = Params::getParams();

  auto setParam = [&apvts, &params](auto &paramPtr, const auto &paramName) {
    paramPtr = static_cast<std::remove_reference_t<decltype(paramPtr)>>(
        apvts.getParameter(params.at(paramName)));
  };

  setParam(attack, Names::Attack_Low_Band);
  setParam(release, Names::Release_Low_Band);
  setParam(threshold, Names::Threshold_Low_Band);
  setParam(ratio, Names::Ratio_Low_Band);
  setParam(bypassed, Names::Bypassed_Low_Band);
}

void CompressorBand::updateCompressorSettings() {
  compressor.setAttack(attack->get());
  compressor.setRelease(release->get());
  compressor.setThreshold(threshold->get());
  compressor.setRatio(ratio->getCurrentChoiceName().getFloatValue());
}

void CompressorBand::process(juce::AudioBuffer<float> &buffer) {
  auto dspBlock = juce::dsp::AudioBlock<float>(buffer);
  auto dspContext = juce::dsp::ProcessContextReplacing<float>(dspBlock);
  dspContext.isBypassed = bypassed->get();
  compressor.process(dspContext);
}
