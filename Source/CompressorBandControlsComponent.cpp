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
  auto makeAttachment = [&params, &apvts](auto &attachment, auto &slider,
                                          auto param) {
    attachment =
        std::make_unique<std::remove_reference_t<decltype(*attachment)>>(
            apvts, params.at(param), slider);
  };

  // clang-format off
  makeAttachment(attackSliderAttachment, attackSlider, Names::Attack_Mid_Band);
  makeAttachment(releaseSliderAttachment, releaseSlider, Names::Release_Mid_Band);
  makeAttachment(thresholdSliderAttachment, thresholdSlider, Names::Threshold_Mid_Band);
  makeAttachment(ratioSliderAttachment, ratioSlider, Names::Ratio_Mid_Band);

  makeAttachment(bypassBtnAttachment, bypassBtn, Names::Bypassed_Mid_Band);
  makeAttachment(muteBtnAttachment, muteBtn, Names::Mute_Mid_Band);
  makeAttachment(soloBtnAttachment, soloBtn, Names::Solo_Mid_Band);
  // clang-format on

  // XXX ratio slider's min and max unit need to use special values.
  // TODO: make ration slider a derived custom slider.
  setRatioMinMaxLabels();

  addAndMakeVisible(attackSlider);
  addAndMakeVisible(releaseSlider);
  addAndMakeVisible(thresholdSlider);
  addAndMakeVisible(ratioSlider);

  bypassBtn.setName("X");
  soloBtn.setName("S");
  muteBtn.setName("M");
  addAndMakeVisible(bypassBtn);
  addAndMakeVisible(muteBtn);
  addAndMakeVisible(soloBtn);
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

  auto createBandBtnCtrlBox = [](std::vector<Component *> comps) {
    FlexBox flexBox;
    flexBox.flexDirection = FlexBox::Direction::column;
    flexBox.flexWrap = FlexBox::Wrap::noWrap;

    const auto spacer = FlexItem().withHeight(2.f);
    for (auto comp : comps) {
      flexBox.items.add(spacer);
      flexBox.items.add(FlexItem(*comp).withFlex(1.0f));
    }
    flexBox.items.add(spacer);
    return flexBox;
  };

  auto boxForStateBtns = createBandBtnCtrlBox({&bypassBtn, &soloBtn, &muteBtn});
  FlexBox flexBox;
  flexBox.flexDirection = FlexBox::Direction::row;
  flexBox.flexWrap = FlexBox::Wrap::noWrap;

  flexBox.items.add(FlexItem(attackSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(releaseSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(ratioSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(boxForStateBtns).withFlex(0.3f));

  flexBox.performLayout(bounds);
}
