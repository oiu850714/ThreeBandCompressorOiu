/*
  ==============================================================================

    GlobalControlsComponent.cpp
    Created: 20 Feb 2023 4:48:46pm
    Author:  oiu85

  ==============================================================================
*/

#include "GlobalControlsComponent.h"

#include "ComponentDrawingUtils.h"
#include "ParamIDConst.h"

using namespace Params;

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState &apvts)
  // clang-format off
    : inGainSlider      {getParam(apvts, Names::Gain_In), "dB", "Input Trim"},
      lowMidXoverSlider {getParam(apvts, Names::Low_Mid_Crossover_Freq), "Hz", "Low-Mid X-over"},
      midHighXoverSlider{getParam(apvts, Names::Mid_High_Crossover_Freq), "Hz", "Mid-High X-over"},
      outGainSlider     {getParam(apvts, Names::Gain_In), "dB", "Output Trim"} {
  // clang-format on

  inGainSlider.setMinMaxLabels();
  lowMidXoverSlider.setMinMaxLabels();
  midHighXoverSlider.setMinMaxLabels();
  outGainSlider.setMinMaxLabels();

  auto makeAttachment = [&params = Params::getParams(), &apvts](auto &slider,
                                                                auto param) {
    return std::make_unique<Attachment>(apvts, params.at(param), slider);
  };

  inGainSliderAttachment = makeAttachment(inGainSlider, Names::Gain_In);
  lowMidXoverSliderAttachment =
      makeAttachment(lowMidXoverSlider, Names::Low_Mid_Crossover_Freq);
  midHighXoverSliderAttachment =
      makeAttachment(midHighXoverSlider, Names::Mid_High_Crossover_Freq);
  outGainSliderAttachment = makeAttachment(outGainSlider, Names::Gain_Out);

  addAndMakeVisible(inGainSlider);
  addAndMakeVisible(lowMidXoverSlider);
  addAndMakeVisible(midHighXoverSlider);
  addAndMakeVisible(outGainSlider);
}

void GlobalControls::paint(juce::Graphics &g) {
  drawModuleBackground(g, getLocalBounds());
}

void GlobalControls::resized() {
  using namespace juce;
  auto bounds = getLocalBounds().reduced(5);

  FlexBox flexBox;
  flexBox.flexDirection = FlexBox::Direction::row;
  flexBox.flexWrap = FlexBox::Wrap::noWrap;

  flexBox.items.add(FlexItem(inGainSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(lowMidXoverSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(midHighXoverSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(outGainSlider).withFlex(1.0f));

  flexBox.performLayout(bounds);
}
