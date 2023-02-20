/*
  ==============================================================================

    GlobalControlsComponent.cpp
    Created: 20 Feb 2023 4:48:46pm
    Author:  oiu85

  ==============================================================================
*/

#include "GlobalControlsComponent.h"

void GlobalControls::paint(juce::Graphics& g) {
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
