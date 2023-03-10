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
  virtual juce::String getDisplayString() const;

  void changeParam(juce::RangedAudioParameter *newParam);
  const juce::RangedAudioParameter *getParam() const noexcept { return param; }

  virtual void setMinMaxLabels() {
    labels.clear();
    auto minVal = param->getNormalisableRange().start,
         maxVal = param->getNormalisableRange().end;
    labels.add({0.f, unitValueTruncatedOver1K(minVal, getSuffix())});
    labels.add({1.f, unitValueTruncatedOver1K(maxVal, getSuffix())});
  }

 private:
  juce::RangedAudioParameter *param{nullptr};
  juce::String suffix;
};

class RotaryRatioSlider : public RotarySliderWithLabels {
 public:
  RotaryRatioSlider(juce::RangedAudioParameter *rap)
      : RotarySliderWithLabels(rap, "", "Ratio") {}
  juce::String getDisplayString() const override;

 private:
  void setMinMaxLabels() override;
};
