/*
  ==============================================================================

    GlobalControlsComponent.cpp
    Created: 20 Feb 2023 4:48:46pm
    Author:  oiu85

  ==============================================================================
*/

#include "GlobalControlsComponent.h"

#include "ParamIDConst.h"

static juce::RangedAudioParameter *getParams(
    juce::AudioProcessorValueTreeState &apvts, Params::Names name) {
  auto &params = Params::getParams();
  return apvts.getParameter(params.at(name));
}

GlobalControls::GlobalControls(juce::AudioProcessorValueTreeState &apvts)
    // clang-format off
    : inGainSlider      {*getParams(apvts, Params::Names::Gain_In), "dB"},
      lowMidXoverSlider {*getParams(apvts, Params::Names::Low_Mid_Crossover_Freq), "Hz"},
      midHighXoverSlider{*getParams(apvts, Params::Names::Mid_High_Crossover_Freq), "Hz"},
      outGainSlider     {*getParams(apvts, Params::Names::Gain_In), "dB"} {
  // clang-format on
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

  addSlidersMinMaxLabels(apvts);

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

void GlobalControls::addSlidersMinMaxLabels(juce::AudioProcessorValueTreeState &apvts) {
  auto addMinMaxLabel = [&params = Params::getParams(), &apvts = apvts] (auto &slider, auto name) {
    auto paramName = params.at(name);
    auto param = apvts.getParameter(paramName);
    auto minVal = param->getNormalisableRange().start, maxVal = param->getNormalisableRange().end;
    slider.labels.add({0.f, unitValueTruncatedOver1K(minVal, slider.getSuffix())});
    slider.labels.add({1.f, unitValueTruncatedOver1K(maxVal, slider.getSuffix())});
  };

  using namespace Params;
  addMinMaxLabel(inGainSlider, Names::Gain_In);
  addMinMaxLabel(lowMidXoverSlider, Params::Names::Low_Mid_Crossover_Freq);
  addMinMaxLabel(midHighXoverSlider, Params::Names::Mid_High_Crossover_Freq);
  addMinMaxLabel(outGainSlider, Params::Names::Gain_Out);
}
