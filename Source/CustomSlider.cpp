/*
 * ==============================================================================


 * CustomSlider.cpp
    Created: 20 Feb 2023 4:44:31pm
    Author:  oiu85


 * ==============================================================================
*/

#include "CustomSlider.h"

#include "CustomButton.h"

//==============================================================================
void RotarySliderWithLabels::paint(juce::Graphics& g) {
  using namespace juce;

  auto startAng = degreesToRadians(180.f + 45.f);
  auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

  auto range = getRange();

  auto sliderBounds = getSliderBounds();

  g.setColour(Colours::blueviolet);
  g.drawFittedText(getName(),
                   getLocalBounds().removeFromTop(getTextHeight() + 2),
                   Justification::centredBottom, 1);

  //    g.setColour(Colours::red);
  //    g.drawRect(getLocalBounds());
  //    g.setColour(Colours::yellow);
  //    g.drawRect(sliderBounds);

  getLookAndFeel().drawRotarySlider(
      g, sliderBounds.getX(), sliderBounds.getY(), sliderBounds.getWidth(),
      sliderBounds.getHeight(),
      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), startAng,
      endAng, *this);

  auto center = sliderBounds.toFloat().getCentre();
  auto radius = sliderBounds.getWidth() * 0.5f;

  g.setColour(Colour(0u, 172u, 1u));
  g.setFont(getTextHeight());

  auto numChoices = labels.size();
  for (int i = 0; i < numChoices; ++i) {
    auto pos = labels[i].pos;
    jassert(0.f <= pos);
    jassert(pos <= 1.f);

    auto ang = jmap(pos, 0.f, 1.f, startAng, endAng);

    auto c = center.getPointOnCircumference(radius + getTextHeight() * 0.5f + 1,
                                            ang);

    Rectangle<float> r;
    auto str = labels[i].label;
    r.setSize(g.getCurrentFont().getStringWidth(str), getTextHeight());
    r.setCentre(c);
    r.setY(r.getY() + getTextHeight());

    g.drawFittedText(str, r.toNearestInt(), juce::Justification::centred, 1);
  }
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const {
  auto bounds = getLocalBounds();
  bounds.removeFromTop(getTextHeight() * 1.5);  // For displaying title.

  auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

  size -= getTextHeight() * 1.5;
  juce::Rectangle<int> r;
  r.setSize(size, size);
  r.setCentre(bounds.getCentreX(), 0);
  r.setY(bounds.getY());  // For displaying title.

  return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const {
  if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param))
    return choiceParam->getCurrentChoiceName();

  if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
    return unitValueTruncatedOver1K(getValue(), suffix);
  } else {
    jassertfalse;  // this shouldn't happen!
  }
}

void RotarySliderWithLabels::changeParam(juce::RangedAudioParameter* newParam) {
  param = newParam;
  if (param) {
    setMinMaxLabels();
  }
  repaint();
}

juce::String RotaryRatioSlider::getDisplayString() const {
  auto* choiceParam =
      dynamic_cast<const juce::AudioParameterChoice*>(getParam());
  jassert(choiceParam != nullptr);

  auto currentChoice = choiceParam->getCurrentChoiceName();
  return juce::String(currentChoice.getIntValue()) + ":1";
}

void RotaryRatioSlider::setMinMaxLabels() {
  labels.clear();
  labels.add({0.f, "1:1"});
  auto ratioParam = static_cast<const juce::AudioParameterChoice*>(getParam());
  auto maxRatio = (ratioParam->choices.end() - 1)->getIntValue();
  labels.add({1.f, juce::String(maxRatio) + ":1"});
}
