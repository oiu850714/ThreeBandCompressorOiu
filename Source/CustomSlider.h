/*
  ==============================================================================

    CustomSlider.h
    Created: 20 Feb 2023 4:44:31pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// Dummy class for the sake of prototyping.
class RotarySlider : public juce::Slider {
 public:
  RotarySlider()
      : juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag,
                     juce::Slider::NoTextBox) {}
};

struct CustomLookAndFeel : juce::LookAndFeel_V4 {
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
  RotarySliderWithLabels(juce::RangedAudioParameter *rap,
                         const juce::String &unitSuffix,
                         const juce::String &title)
      : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                     juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(rap),
        suffix(unitSuffix) {
    setName(title);
  }

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

  void changeParam(juce::RangedAudioParameter *newParam);
  const juce::RangedAudioParameter *getParam() const noexcept { return param; }

  void setMinMaxLabels() {
    auto minVal = param->getNormalisableRange().start,
         maxVal = param->getNormalisableRange().end;
    labels.add({0.f, unitValueTruncatedOver1K(minVal, getSuffix())});
    labels.add({1.f, unitValueTruncatedOver1K(maxVal, getSuffix())});
  }
 private:
  juce::RangedAudioParameter *param{nullptr};
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
