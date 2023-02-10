/*
  ==============================================================================

    ParamIDConst.h
    Created: 10 Feb 2023 7:33:01pm
    Author:  oiu85

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
namespace Params {
enum Names {
  Low_Mid_Crossover_Freq,
  Mid_High_Crossover_Freq,

  Threshold_Low_Band,
  Threshold_Mid_Band,
  Threshold_High_Band,

  Attack_Low_Band,
  Attack_Mid_Band,
  Attack_High_Band,

  Release_Low_Band,
  Release_Mid_Band,
  Release_High_Band,

  Ratio_Low_Band,
  Ratio_Mid_Band,
  Ratio_High_Band,

  Bypassed_Low_Band,
  Bypassed_Mid_Band,
  Bypassed_High_Band,
};

inline std::map<Names, juce::String> &getParams() {
  static std::map<Names, juce::String> params{
      {Low_Mid_Crossover_Freq, "Low-Mid Crossover Freq"},
      {Mid_High_Crossover_Freq, "Mid-High Crossover Freq"},

      {Threshold_Low_Band, "Threshold Low Band"},
      {Threshold_Mid_Band, "Threshold Mid Band"},
      {Threshold_High_Band, "Threshold High Band"},

      {Attack_Low_Band, "Attack Low Band"},
      {Attack_Mid_Band, "Attack Mid Band"},
      {Attack_High_Band, "Attack High Band"},

      {Release_Low_Band, "Release Low Band"},
      {Release_Mid_Band, "Release Mid Band"},
      {Release_High_Band, "Release High Band"},

      {Ratio_Low_Band, "Ratio Low Band"},
      {Ratio_Mid_Band, "Ratio Mid Band"},
      {Ratio_High_Band, "Ratio High Band"},

      {Bypassed_Low_Band, "Bypassed Low Band"},
      {Bypassed_Mid_Band, "Bypassed Mid Band"},
      {Bypassed_High_Band, "Bypassed High Band"},
  };
  return params;
}
}  // namespace Params
