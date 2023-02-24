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

class CompressorBandControls : public juce::Component,
                               private juce::Button::Listener {
 public:
  CompressorBandControls(juce::AudioProcessorValueTreeState &apv);
  ~CompressorBandControls() override;
  void resized() override;
  void paint(juce::Graphics &g) override;

  void buttonClicked(juce::Button *button) override;

 private:
  juce::AudioProcessorValueTreeState &apvts;
  RotarySliderWithLabels attackSlider, releaseSlider, thresholdSlider;
  RotaryRatioSlider ratioSlider;
  juce::ToggleButton bypassBtn, soloBtn, muteBtn;
  juce::ToggleButton lowBand, midBand, highBand;

  // Dynamic allocating attachments for the sake of constructor clarity...
  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<Attachment> attackSliderAttachment, releaseSliderAttachment,
      thresholdSliderAttachment, ratioSliderAttachment;
  using BtnAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
  std::unique_ptr<BtnAttachment> bypassBtnAttachment, muteBtnAttachment,
      soloBtnAttachment;
  void setRatioMinMaxLabels();

  // TBH I have no idea when this component may be invalid.
  juce::Component::SafePointer<CompressorBandControls> safePtr{this};

  void updateParamSelection();

  // Disable setting sliders if mute or bypass buttons are toggled.
  void updateSliderEnablements();
  // Update toggle states according to some rules described in source files.
  void updateToggleStates(juce::Button &clickedButton);
};
