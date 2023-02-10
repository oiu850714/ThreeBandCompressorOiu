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
  using namespace Param;
  attack =
      static_cast<juce::AudioParameterFloat *>(apvts.getParameter(AttackID));
  release =
      static_cast<juce::AudioParameterFloat *>(apvts.getParameter(ReleaseID));
  threshold =
      static_cast<juce::AudioParameterFloat *>(apvts.getParameter(ThresholdID));
  ratio =
      static_cast<juce::AudioParameterChoice *>(apvts.getParameter(RatioID));
  bypassed =
      static_cast<juce::AudioParameterBool *>(apvts.getParameter(BypassID));
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
