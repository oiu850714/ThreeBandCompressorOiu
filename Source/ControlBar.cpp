/*
  ==============================================================================

    ControlBar.cpp
    Created: 28 Feb 2023 2:01:51pm
    Author:  oiu85

  ==============================================================================
*/

#include "ControlBar.h"

ControlBar::ControlBar() {
  analyzerButton.setToggleState(true,
                                juce::NotificationType::dontSendNotification);
  globalBypassButton.setToggleState(
      true, juce::NotificationType::dontSendNotification);
  addAndMakeVisible(analyzerButton);
  addAndMakeVisible(globalBypassButton);
}

void ControlBar::resized() {
  auto bounds = getLocalBounds();

  analyzerButton.setBounds(
      bounds.removeFromLeft(50).withTrimmedTop(4).withTrimmedBottom(4));

  globalBypassButton.setBounds(
      bounds.removeFromRight(60).withTrimmedTop(2).withTrimmedBottom(2));
}
