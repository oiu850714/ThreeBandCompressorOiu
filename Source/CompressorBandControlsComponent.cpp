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
  : apvts(apvts),
    attackSlider    {nullptr, "ms", "Attack"},
    releaseSlider   {nullptr, "ms", "Release"},
    thresholdSlider {nullptr, "dB", "Threshold"},
    ratioSlider     {nullptr} {
  // clang-format on

  addAndMakeVisible(attackSlider);
  addAndMakeVisible(releaseSlider);
  addAndMakeVisible(thresholdSlider);
  addAndMakeVisible(ratioSlider);

  // Set Listener
  bypassBtn.addListener(this);
  soloBtn.addListener(this);
  muteBtn.addListener(this);

  bypassBtn.setName("X");
  soloBtn.setName("S");
  muteBtn.setName("M");
  addAndMakeVisible(bypassBtn);
  addAndMakeVisible(muteBtn);
  addAndMakeVisible(soloBtn);

  lowBand.setName("Low");
  midBand.setName("Mid");
  highBand.setName("High");
  lowBand.setRadioGroupId(1);
  midBand.setRadioGroupId(1);
  highBand.setRadioGroupId(1);

  auto btnSwitch = [safePtr = this->safePtr] {
    if (safePtr) {
      safePtr->updateParamSelection();
    }
  };
  lowBand.onClick = midBand.onClick = highBand.onClick = btnSwitch;

  // Select low band by default.
  lowBand.setToggleState(true, juce::NotificationType::dontSendNotification);
  updateParamSelection();

  addAndMakeVisible(lowBand);
  addAndMakeVisible(midBand);
  addAndMakeVisible(highBand);
}

CompressorBandControls::~CompressorBandControls() {
  bypassBtn.removeListener(this);
  muteBtn.removeListener(this);
  soloBtn.removeListener(this);
}

void CompressorBandControls::paint(juce::Graphics &g) {
  drawModuleBackground(g, getLocalBounds());
}

void CompressorBandControls::buttonClicked(juce::Button *button) {
  updateToggleStates(*button);
  updateSliderEnablements();
}
void CompressorBandControls::updateSliderEnablements() {
  auto disabled = muteBtn.getToggleState() || bypassBtn.getToggleState();
  attackSlider.setEnabled(!disabled);
  releaseSlider.setEnabled(!disabled);
  thresholdSlider.setEnabled(!disabled);
  ratioSlider.setEnabled(!disabled);
}

void CompressorBandControls::updateToggleStates(juce::Button &clickedButton) {
  // If you toggle a button off, don't do anything.
  if (!clickedButton.getToggleState()) {
    return;
  }
  using namespace juce;

  // If solo button is toggled, then toggle mute and bypass buttons off.
  if (&clickedButton == &soloBtn) {
    muteBtn.setToggleState(false, NotificationType::sendNotification);
    bypassBtn.setToggleState(false, NotificationType::sendNotification);
  }
  // If mute button is toggled, then toggle solo and bypass buttons off.
  if (&clickedButton == &muteBtn) {
    soloBtn.setToggleState(false, NotificationType::sendNotification);
    bypassBtn.setToggleState(false, NotificationType::sendNotification);
  }
  // If bypass button is toggled, then toggle solo and mute buttons off.
  if (&clickedButton == &bypassBtn) {
    muteBtn.setToggleState(false, NotificationType::sendNotification);
    soloBtn.setToggleState(false, NotificationType::sendNotification);
  }
}

void CompressorBandControls::setRatioMinMaxLabels() {
  ratioSlider.labels.clear();
  ratioSlider.labels.add({0.f, "1:1"});
  auto ratioParam =
      static_cast<const juce::AudioParameterChoice *>(ratioSlider.getParam());
  auto maxRatio = (ratioParam->choices.end() - 1)->getIntValue();
  ratioSlider.labels.add({1.f, juce::String(maxRatio) + ":1"});
}

void CompressorBandControls::updateParamSelection() {
  // TODO
  auto updateAttachment = [&params = getParams(), &apvts = this->apvts](
                              auto &attachment, auto &slider, auto param) {
    // XXX: in theory, we can skip the reset() step.
    //      But that does NOT work (if we really skip reset()) and will make the
    //      old attached parameter to be reset.
    // Refer to: https://youtu.be/Mo0Oco3Vimo?t=19085
    attachment.reset();
    attachment =
        std::make_unique<std::remove_reference_t<decltype(*attachment)>>(
            apvts, params.at(param), slider);
  };

  auto updateSlidersParam = [this](auto attack, auto release, auto threshold,
                                   auto ratio) {
    attackSlider.changeParam(getParam(apvts, attack));
    releaseSlider.changeParam(getParam(apvts, release));
    thresholdSlider.changeParam(getParam(apvts, threshold));
    ratioSlider.changeParam(getParam(apvts, ratio));
  };

  auto updateAttachments = [this, updateAttachment](auto attack, auto release,
                                                    auto threshold, auto ratio,
                                                    auto bypassed, auto mute,
                                                    auto solo) {
    // clang-format off
        updateAttachment(attackSliderAttachment, attackSlider, attack);
        updateAttachment(releaseSliderAttachment, releaseSlider, release);
        updateAttachment(thresholdSliderAttachment, thresholdSlider, threshold);
        updateAttachment(ratioSliderAttachment, ratioSlider, ratio);

        updateAttachment(bypassBtnAttachment, bypassBtn, bypassed);
        updateAttachment(muteBtnAttachment, muteBtn, mute);
        updateAttachment(soloBtnAttachment, soloBtn, solo);
    // clang-format on
  };

  enum class BandType { Low, Mid, High };

  auto bandType = lowBand.getToggleState()   ? BandType::Low
                  : midBand.getToggleState() ? BandType::Mid
                                             : BandType::High;
  // clang-format off
  switch (bandType) {
    case BandType::Low: {
      updateSlidersParam(Names::Attack_Low_Band,
                         Names::Release_Low_Band,
                         Names::Threshold_Low_Band,
                         Names::Ratio_Low_Band);
      updateAttachments(Names::Attack_Low_Band,
                        Names::Release_Low_Band,
                        Names::Threshold_Low_Band,
                        Names::Ratio_Low_Band,
                        Names::Bypassed_Low_Band,
                        Names::Mute_Low_Band,
                        Names::Solo_Low_Band);
      break;
    }
    case BandType::Mid: {
      updateSlidersParam(Names::Attack_Mid_Band,
                         Names::Release_Mid_Band,
                         Names::Threshold_Mid_Band,
                         Names::Ratio_Mid_Band);
      updateAttachments(Names::Attack_Mid_Band,
                        Names::Release_Mid_Band,
                        Names::Threshold_Mid_Band,
                        Names::Ratio_Mid_Band,
                        Names::Bypassed_Mid_Band,
                        Names::Mute_Mid_Band,
                        Names::Solo_Mid_Band);
      break;
    }
    case BandType::High: {
      updateSlidersParam(Names::Attack_High_Band,
                         Names::Release_High_Band,
                         Names::Threshold_High_Band,
                         Names::Ratio_High_Band);
      updateAttachments(Names::Attack_High_Band,
                        Names::Release_High_Band,
                        Names::Threshold_High_Band,
                        Names::Ratio_High_Band,
                        Names::Bypassed_High_Band,
                        Names::Mute_High_Band,
                        Names::Solo_High_Band);
      break;
    }
  }
  // clang-format on
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
  auto boxForBandSelectionBtns =
      createBandBtnCtrlBox({&highBand, &midBand, &lowBand});

  FlexBox flexBox;
  flexBox.flexDirection = FlexBox::Direction::row;
  flexBox.flexWrap = FlexBox::Wrap::noWrap;

  flexBox.items.add(FlexItem(boxForBandSelectionBtns).withFlex(0.3f));
  flexBox.items.add(FlexItem(attackSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(releaseSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(thresholdSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(ratioSlider).withFlex(1.0f));
  flexBox.items.add(FlexItem(boxForStateBtns).withFlex(0.3f));

  flexBox.performLayout(bounds);
}
