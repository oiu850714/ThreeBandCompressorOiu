/*
  ==============================================================================

    CompressorBandControlsComponent.cpp
    Created: 24 Feb 2023 10:22:45am
    Author:  oiu85

  ==============================================================================
*/

#include "CompressorBandControlsComponent.h"

#include "ComponentDrawingUtils.h"
#include "ParamIDConst.h"

using namespace Params;

CompressorBandControls::CompressorBandControls(
    juce::AudioProcessorValueTreeState &apvts)
    // clang-format off
  : attackSlider    {getParams(apvts, Names::Attack_Mid_Band), "ms", "Attack"},
    releaseSlider   {getParams(apvts, Names::Release_Mid_Band), "ms", "Release"},
    thresholdSlider {getParams(apvts, Names::Threshold_Mid_Band), "dB", "Threshold"},
    ratioSlider     {getParams(apvts, Names::Ratio_Mid_Band), "", "Ratio"} {
  // clang-format on

  auto &params = getParams();
  auto makeAttachment = [&params, &apvts](auto &slider, auto param) {
    return std::make_unique<Attachment>(apvts, params.at(param), slider);
  };

  // clang-format off
  attackSliderAttachment = makeAttachment(attackSlider, Names::Attack_Mid_Band);
  releaseSliderAttachment = makeAttachment(releaseSlider, Names::Release_Mid_Band);
  thresholdSliderAttachment = makeAttachment(thresholdSlider, Names::Threshold_Mid_Band);
  ratioSliderAttachment = makeAttachment(ratioSlider, Names::Ratio_Mid_Band);
  // clang-format on

  // XXX ratio slider's min and max unit need to use special values.
  // TODO: make ration slider a derived custom slider.
  setRatioMinMaxLabels();

  addAndMakeVisible(attackSlider);
  addAndMakeVisible(releaseSlider);
  addAndMakeVisible(thresholdSlider);
  addAndMakeVisible(ratioSlider);
}

void CompressorBandControls::paint(juce::Graphics &g) {
  drawModuleBackground(g, getLocalBounds());
}

void CompressorBandControls::setRatioMinMaxLabels() {
  ratioSlider.labels.clear();
  ratioSlider.labels.add({0.f, "1:1"});
  auto ratioParam = static_cast<const juce::AudioParameterChoice*>(ratioSlider.getParam());
  auto maxRatio = (ratioParam->choices.end() - 1)->getIntValue();
  ratioSlider.labels.add({1.f, juce::String(maxRatio) + ":1"});
}

void CompressorBandControls::resized() {
  using namespace juce;
  auto bounds = getLocalBounds().reduced(5);

  FlexBox flexBox;
  flexBox.flexDirection = FlexBox::Direction::row;
  flexBox.flexWrap = FlexBox::Wrap::noWrap;

  flexBox.items.add(FlexItem(attackSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(releaseSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(ratioSlider).withFlex(1.0f));

  flexBox.performLayout(bounds);
}
