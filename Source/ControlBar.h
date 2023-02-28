/*
  ==============================================================================

    ControlBar.h
    Created: 28 Feb 2023 2:01:51pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "CustomButton.h"

class ControlBar : public juce::Component {
 public:
  ControlBar();
  void resized() override;

 public:
  AnalyzerButton analyzerButton;
  PowerButton globalBypassButton;
};
