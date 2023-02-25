/*
  ==============================================================================

    ComponentDrawingUtils.h
    Created: 24 Feb 2023 10:41:34am
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

inline void drawModuleBackground(juce::Graphics &g,
                                 juce::Rectangle<int> bounds) {
  using namespace juce;

  // border
  g.setColour(Colours::blueviolet);
  g.fillAll();

  bounds.reduce(3, 3);
  g.setColour(Colours::black);
  g.fillRoundedRectangle(bounds.toFloat(), 3);
}
