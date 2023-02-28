/*
 * ==============================================================================


 * CustomSlider.cpp
    Created: 20 Feb 2023 4:44:31pm
    Author:  oiu85


 * ==============================================================================
*/

#include "CustomSlider.h"
#include "CustomButton.h"

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y,
                                         int width, int height,
                                         float sliderPosProportional,
                                         float rotaryStartAngle,
                                         float rotaryEndAngle,
                                         juce::Slider& slider) {
  using namespace juce;

  auto bounds = Rectangle<float>(x, y, width, height);

  auto enabled = slider.isEnabled();

  g.setColour(enabled ? Colour(97u, 18u, 167u) : Colours::darkgrey);
  g.fillEllipse(bounds);

  g.setColour(enabled ? Colour(255u, 154u, 1u) : Colours::grey);
  g.drawEllipse(bounds, 1.f);

  if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider)) {
    auto center = bounds.getCentre();
    Path p;

    Rectangle<float> r;
    r.setLeft(center.getX() - 2);
    r.setRight(center.getX() + 2);
    r.setTop(bounds.getY());
    r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);

    p.addRoundedRectangle(r, 2.f);

    jassert(rotaryStartAngle < rotaryEndAngle);

    auto sliderAngRad =
        jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

    p.applyTransform(
        AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

    g.fillPath(p);

    g.setFont(rswl->getTextHeight());
    auto text = rswl->getDisplayString();
    auto strWidth = g.getCurrentFont().getStringWidth(text);

    r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
    r.setCentre(bounds.getCentre());

    g.setColour(enabled ? Colours::black : Colours::darkgrey);
    g.fillRect(r);

    g.setColour(enabled ? Colours::white : Colours::lightgrey);
    g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
  }
}

void CustomLookAndFeel::drawToggleButton(juce::Graphics& g,
                                         juce::ToggleButton& toggleButton,
                                         bool shouldDrawButtonAsHighlighted,
                                         bool shouldDrawButtonAsDown) {
  using namespace juce;

  if (auto* pb = dynamic_cast<PowerButton*>(&toggleButton)) {
    Path powerButton;

    auto bounds = toggleButton.getLocalBounds();

    auto size = jmin(bounds.getWidth(), bounds.getHeight()) - 6;
    auto r = bounds.withSizeKeepingCentre(size, size).toFloat();

    float ang = 30.f;  // 30.f;

    size -= 6;

    powerButton.addCentredArc(r.getCentreX(), r.getCentreY(), size * 0.5,
                              size * 0.5, 0.f, degreesToRadians(ang),
                              degreesToRadians(360.f - ang), true);

    powerButton.startNewSubPath(r.getCentreX(), r.getY());
    powerButton.lineTo(r.getCentre());

    PathStrokeType pst(2.f, PathStrokeType::JointStyle::curved);

    auto color =
        toggleButton.getToggleState() ? Colours::dimgrey : Colour(0u, 172u, 1u);

    g.setColour(color);
    g.strokePath(powerButton, pst);
    g.drawEllipse(r, 2);
  } else if (auto* analyzerButton =
                 dynamic_cast<AnalyzerButton*>(&toggleButton)) {
    auto color = !toggleButton.getToggleState() ? Colours::dimgrey
                                                : Colour(0u, 172u, 1u);

    g.setColour(color);

    auto bounds = toggleButton.getLocalBounds();
    g.drawRect(bounds);

    g.strokePath(analyzerButton->randomPath, PathStrokeType(1.f));
  } else {
    auto bounds = toggleButton.getLocalBounds().reduced(2);

    auto isOn = toggleButton.getToggleState();

    const int cornerSize = 4;

    g.setColour(isOn ? Colours::white : Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), cornerSize);
    g.setColour(isOn ? Colours::black : Colours::white);
    g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1);

    g.drawFittedText(toggleButton.getName(), bounds, Justification::centred, 1);
  }
}
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
