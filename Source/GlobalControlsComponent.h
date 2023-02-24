/*
  ==============================================================================

    GlobalControlsComponent.h
    Created: 20 Feb 2023 4:48:46pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomSlider.h"

class GlobalControls : public juce::Component {
 public:
  GlobalControls(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;
  void resized() override;

 private:
  // clang-format off
  using RSWL = RotarySliderWithLabels;
  RSWL inGainSlider, lowMidXoverSlider, midHighXoverSlider, outGainSlider;

  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  // Dynamic allocating attachments for the sake of constructor clarity...
  std::unique_ptr<Attachment> inGainSliderAttachment,
                              lowMidXoverSliderAttachment,
                              midHighXoverSliderAttachment,
                              outGainSliderAttachment;
  // clang-format on
};
