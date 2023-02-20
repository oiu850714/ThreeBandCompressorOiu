/*
  ==============================================================================

    GlobalControlsComponent.h
    Created: 20 Feb 2023 4:48:46pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class GlobalControls : public juce::Component {
 public:
  GlobalControls(juce::AudioProcessorValueTreeState &apvts);
  void paint(juce::Graphics &g) override;
  void resized() override;

 private:
  class RotarySlider : public juce::Slider {
   public:
    RotarySlider()
        : juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag,
                       juce::Slider::NoTextBox) {}
  };

  // clang-format off
  RotarySlider inGainSlider,
               lowMidXoverSlider,
               midHighXoverSlider,
               outGainSlider;

  using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<Attachment> inGainSliderAttachment,
                              lowMidXoverSliderAttachment,
                              midHighXoverSliderAttachment,
                              outGainSliderAttachment;
  // clang-format on
};
