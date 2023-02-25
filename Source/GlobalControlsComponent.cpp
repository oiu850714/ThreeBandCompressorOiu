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
    : inGainSlider      {getParams(apvts, Names::Gain_In), "dB", "Input Trim"},
      lowMidXoverSlider {getParams(apvts, Names::Low_Mid_Crossover_Freq), "Hz", "Low-Mid X-over"},
      midHighXoverSlider{getParams(apvts, Names::Mid_High_Crossover_Freq), "Hz", "Mid-High X-over"},
      outGainSlider     {getParams(apvts, Names::Gain_In), "dB", "Output Trim"} {
  // clang-format on
  using namespace Params;
  auto &params = Params::getParams();

  inGainSlider.setMinMaxLabels();
  lowMidXoverSlider.setMinMaxLabels();
  midHighXoverSlider.setMinMaxLabels();
  outGainSlider.setMinMaxLabels();

  auto makeAttachment = [&params, &apvts](auto &slider, auto param) {
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
