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
  CompressorBandControls(juce::AudioProcessorValueTreeState &apv);
  void resized() override;
  void paint(juce::Graphics &g) override;
 private:
  juce::AudioProcessorValueTreeState &apvts;
  RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;
  RotaryRatioSlider ratioSlider;
  juce::ToggleButton bypassBtn, soloBtn, muteBtn;
  juce::ToggleButton lowBand, midBand, highBand;


  // Dynamic allocating attachments for the sake of constructor clarity...
  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<Attachment> attackSliderAttachment,
                              releaseSliderAttachment,
                              thresholdSliderAttachment,
                              ratioSliderAttachment;
  using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
  std::unique_ptr<BtnAttachment> bypassBtnAttachment,
                                 muteBtnAttachment,
                                 soloBtnAttachment;
  void setRatioMinMaxLabels();

  // TBH I have no idea when this component may be invalid.
  juce::Component::SafePointer<CompressorBandControls> safePtr {this};

  void updateParamSelection();
};
