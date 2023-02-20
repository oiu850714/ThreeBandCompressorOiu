/*
  ==============================================================================

    CustomSlider.h
    Created: 20 Feb 2023 4:44:31pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct LookAndFeel : juce::LookAndFeel_V4 {
  void drawRotarySlider(juce::Graphics &, int x, int y, int width, int height,
                        float sliderPosProportional, float rotaryStartAngle,
                        float rotaryEndAngle, juce::Slider &) override;

  void drawToggleButton(juce::Graphics &g, juce::ToggleButton &toggleButton,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
};

template <typename T>
juce::String unitValueTruncatedOver1K(T val, const juce::String &suffix) {
  juce::String Ret;
  if (val > static_cast<T>(999)) {
    return Ret << juce::String(val / static_cast<T>(1000), 2) << "K" << suffix;
  } else {
    return Ret << juce::String(val) << suffix;
  }
}

struct RotarySliderWithLabels : juce::Slider {
  RotarySliderWithLabels(juce::RangedAudioParameter &rap,
                         const juce::String &unitSuffix)
      : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                     juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(&rap),
        suffix(unitSuffix) {
    setLookAndFeel(&lnf);
  }

  ~RotarySliderWithLabels() { setLookAndFeel(nullptr); }

  struct LabelPos {
    float pos;
    juce::String label;
  };

  juce::Array<LabelPos> labels;

  void paint(juce::Graphics &g) override;
  juce::Rectangle<int> getSliderBounds() const;
  int getTextHeight() const { return 14; }
  juce::String getSuffix() const { return suffix; }
  juce::String getDisplayString() const;

 private:
  LookAndFeel lnf;

  juce::RangedAudioParameter *param;
  juce::String suffix;
};

struct PowerButton : juce::ToggleButton {};

struct AnalyzerButton : juce::ToggleButton {
  void resized() override {
    auto bounds = getLocalBounds();
    auto insetRect = bounds.reduced(4);

    randomPath.clear();

    juce::Random r;

    randomPath.startNewSubPath(
        insetRect.getX(),
        insetRect.getY() + insetRect.getHeight() * r.nextFloat());

    for (auto x = insetRect.getX() + 1; x < insetRect.getRight(); x += 2) {
      randomPath.lineTo(
          x, insetRect.getY() + insetRect.getHeight() * r.nextFloat());
    }
  }

  juce::Path randomPath;
};
