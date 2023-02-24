/*
  ==============================================================================

    CompressorBandControlsComponent.h
    Created: 24 Feb 2023 10:22:45am
    Author:  oiu85

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include "CustomSlider.h"

class CompressorBandControls : public juce::Component {
 public:
  CompressorBandControls(juce::AudioProcessorValueTreeState &apvts);
  void resized() override;
  void paint(juce::Graphics &g) override;
 private:
  RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider, ratioSlider;
  juce::ToggleButton bypassBtn, soloBtn, muteBtn;

  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  // Dynamic allocating attachments for the sake of constructor clarity...
  std::unique_ptr<Attachment> attackSliderAttachment,
                              releaseSliderAttachment,
                              thresholdSliderAttachment,
                              ratioSliderAttachment;
  using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
  std::unique_ptr<BtnAttachment> bypassBtnAttachment,
                                 muteBtnAttachment,
                                 soloBtnAttachment;
  void setRatioMinMaxLabels();
};
