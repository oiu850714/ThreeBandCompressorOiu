/*
  ==============================================================================

    GlobalControlsComponent.cpp
    Created: 20 Feb 2023 4:48:46pm
    Author:  oiu85

  ==============================================================================
*/

#include "GlobalControlsComponent.h"

#include "ParamIDConst.h"

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState &apvts) {
  using namespace Params;
  auto &params = Params::getParams();

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
  using namespace juce;

  // border
  auto bounds = getLocalBounds();
  const auto localbounds = bounds;
  g.setColour(Colours::blueviolet);
  g.fillAll();

  bounds.reduce(3, 3);
  g.setColour(Colours::black);
  g.fillRoundedRectangle(bounds.toFloat(), 3);
}

void GlobalControls::resized() {
  using namespace juce;
  auto bounds = getLocalBounds();

  FlexBox flexBox;
  flexBox.flexDirection = FlexBox::Direction::row;
  flexBox.flexWrap = FlexBox::Wrap::noWrap;

  flexBox.items.add(FlexItem(inGainSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(lowMidXoverSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(midHighXoverSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(outGainSlider).withFlex(1.0f));

  flexBox.performLayout(bounds);
}
