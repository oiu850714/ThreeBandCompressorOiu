/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "ParamIDConst.h"

#include "PluginProcessor.h"

//==============================================================================
ThreeBandCompressorOiuAudioProcessorEditor::
    ThreeBandCompressorOiuAudioProcessorEditor(
        ThreeBandCompressorOiuAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  setLookAndFeel(&lnf);
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(600, 500);

  controlBar.analyzerButton.onClick = [this]{
    analyzer.toggleAnalysisEnablement(controlBar.analyzerButton.getToggleState());
  };
  controlBar.globalBypassButton.onClick = [this] {
    toggleGlobalBypassState();
  };

  addAndMakeVisible(controlBar);
  addAndMakeVisible(analyzer);
  addAndMakeVisible(globalControls);
  addAndMakeVisible(bandControls);

  startTimerHz(60);
}

ThreeBandCompressorOiuAudioProcessorEditor::
    ~ThreeBandCompressorOiuAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

//==============================================================================
void ThreeBandCompressorOiuAudioProcessorEditor::paint(juce::Graphics&) {}

void ThreeBandCompressorOiuAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  controlBar.setBounds(bounds.removeFromTop(32));
  bandControls.setBounds(bounds.removeFromBottom(135));
  analyzer.setBounds(bounds.removeFromTop(225));
  globalControls.setBounds(bounds);
}

void ThreeBandCompressorOiuAudioProcessorEditor::timerCallback() {
  // clang-format off
  const auto[
    lowInputRMS, lowOutputRMS,
    midInputRMS, midOutputRMS,
    highInputRMS, highOutputRMS] = audioProcessor.getLowMidHighBandRMSLevels();
  analyzer.updateGainReductions(lowOutputRMS - lowInputRMS,
                                midOutputRMS - midInputRMS,
                                highOutputRMS - highInputRMS);
  // clang-format on
}

void ThreeBandCompressorOiuAudioProcessorEditor::toggleGlobalBypassState() {
  auto getParamAndCast = [&apvts = audioProcessor.apvts] (auto Name){
    return static_cast<juce::AudioParameterBool*>(Params::getParam(apvts, Name));
  };
  std::array bypassParams = {
    getParamAndCast(Params::Names::Bypassed_Low_Band),
    getParamAndCast(Params::Names::Bypassed_Mid_Band),
    getParamAndCast(Params::Names::Bypassed_High_Band),
  };

  auto shouldGlobalBypassed = !controlBar.globalBypassButton.getToggleState();
  for (auto *bypassParam : bypassParams) {
    // Refer to AudioProcessorParameter::setValueNotifyingHost
    bypassParam->beginChangeGesture();
    bypassParam->setValueNotifyingHost(shouldGlobalBypassed);
    bypassParam->endChangeGesture();
  }
}

ThreeBandCompressorOiuAudioProcessorEditor::Placeholder::Placeholder() {
  juce::Random r;
  customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}
