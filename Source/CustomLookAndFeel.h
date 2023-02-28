/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 28 Feb 2023 3:14:26pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct CustomLookAndFeel : juce::LookAndFeel_V4 {
  void drawRotarySlider(juce::Graphics &, int x, int y, int width, int height,
                        float sliderPosProportional, float rotaryStartAngle,
                        float rotaryEndAngle, juce::Slider &) override;

  void drawToggleButton(juce::Graphics &g, juce::ToggleButton &toggleButton,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
};

