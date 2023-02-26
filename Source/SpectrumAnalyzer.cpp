/*

 * ==============================================================================


 * SpectrumAnalyzer.cpp
    Created: 25 Feb 2023 4:51:26pm
    Author:  oiu85


 * ==============================================================================
*/

#include "SpectrumAnalyzer.h"

#include "ComponentDrawingUtils.h"
#include "ParamIDConst.h"

SpectrumAnalyzer::SpectrumAnalyzer(ThreeBandCompressorOiuAudioProcessor& p)
    : audioProcessor(p),
      leftPathProducer(audioProcessor.leftChannelFifo),
      rightPathProducer(audioProcessor.rightChannelFifo) {
  const auto& params = audioProcessor.getParameters();
  for (auto param : params) {
    param->addListener(this);
  }

  auto setParam = [&apvts = audioProcessor.apvts,
                   &params = Params::getParams()](auto& paramPtr,
                                                  const auto& paramName) {
    paramPtr = static_cast<std::remove_reference_t<decltype(paramPtr)>>(
        apvts.getParameter(params.at(paramName)));
  };
  setParam(lowMidXoverParam, Params::Names::Low_Mid_Crossover_Freq);
  setParam(midHighXoverParam, Params::Names::Mid_High_Crossover_Freq);
  setParam(lowBandThredholdParam, Params::Names::Threshold_Low_Band);
  setParam(midBandThredholdParam, Params::Names::Threshold_Mid_Band);
  setParam(highBandThredholdParam, Params::Names::Threshold_High_Band);

  startTimerHz(60);
}

SpectrumAnalyzer::~SpectrumAnalyzer() {
  const auto& params = audioProcessor.getParameters();
  for (auto param : params) {
    param->removeListener(this);
  }
}

void SpectrumAnalyzer::paint(juce::Graphics& g) {
  using namespace juce;
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(Colours::black);

  auto backgroundBounds = drawModuleBackground(g, getLocalBounds());
  drawBackgroundGrid(g, backgroundBounds);
  drawFFTAnalysis(g, getAnalysisArea(backgroundBounds));

  g.setColour(Colours::black);

  drawBandParameterLinesAndGainReductions(g, backgroundBounds);
  drawTextLabels(g, backgroundBounds);
}

std::vector<float> SpectrumAnalyzer::getFrequencies() {
  return std::vector<float>{
      MIN_FREQ, /*30, 40,*/ 50,       100,   200,     /*300, 400,*/ 500, 1000,
      2000,     /*3000, 4000,*/ 5000, 10000, MAX_FREQ};
}

std::vector<float> SpectrumAnalyzer::getGains() {
  std::vector<float> gains;
  for (auto gain = DECIBAL_NEGATIVE_INFINITY; gain <= DECIBAL_MAX;
       gain += DECIBAL_INCREMENT) {
    gains.push_back(gain);
  }
  return gains;
}

std::vector<float> SpectrumAnalyzer::getXs(const std::vector<float>& freqs,
                                           float left, float width) {
  std::vector<float> xs;
  for (auto f : freqs) {
    auto normX = juce::mapFromLog10(f, MIN_FREQ, MAX_FREQ);
    xs.push_back(left + width * normX);
  }

  return xs;
}

void SpectrumAnalyzer::drawBackgroundGrid(
    juce::Graphics& g, juce::Rectangle<int> backgroundBounds) {
  using namespace juce;
  auto freqs = getFrequencies();

  auto renderArea = getAnalysisArea(backgroundBounds);
  auto left = renderArea.getX();
  auto right = renderArea.getRight();
  auto top = renderArea.getY();
  auto bottom = renderArea.getBottom();
  auto width = renderArea.getWidth();

  auto xs = getXs(freqs, left, width);

  g.setColour(Colours::dimgrey);
  for (auto x : xs) {
    g.drawVerticalLine(x, top, bottom);
  }

  auto gain = getGains();

  for (auto gDb : gain) {
    auto y = jmap(gDb, DECIBAL_NEGATIVE_INFINITY, DECIBAL_MAX, float(bottom),
                  float(top));

    g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey);
    g.drawHorizontalLine(y, left, right);
  }
}

void SpectrumAnalyzer::drawTextLabels(juce::Graphics& g,
                                      juce::Rectangle<int> backgroundBounds) {
  using namespace juce;
  g.setColour(Colours::lightgrey);
  const int fontHeight = 10;
  g.setFont(fontHeight);

  auto renderArea = getAnalysisArea(backgroundBounds);
  auto left = renderArea.getX();

  auto top = renderArea.getY();
  auto bottom = renderArea.getBottom();
  auto width = renderArea.getWidth();

  auto freqs = getFrequencies();
  auto xs = getXs(freqs, left, width);

  for (int i = 0; i < freqs.size(); ++i) {
    auto f = freqs[i];
    auto x = xs[i];

    bool addK = false;
    String str;
    if (f > 999.f) {
      addK = true;
      f /= 1000.f;
    }

    str << f;
    if (addK) str << "k";
    str << "Hz";

    auto textWidth = g.getCurrentFont().getStringWidth(str);

    Rectangle<int> r;

    r.setSize(textWidth, fontHeight);
    r.setCentre(x, 0);
    r.setY(backgroundBounds.getY());

    g.drawFittedText(str, r, juce::Justification::centred, 1);
  }

  auto gain = getGains();

  for (auto gDb : gain) {
    auto y = jmap(gDb, DECIBAL_NEGATIVE_INFINITY, DECIBAL_MAX, float(bottom),
                  float(top));

    String str;
    if (gDb > 0) str << "+";
    str << gDb;

    auto textWidth = g.getCurrentFont().getStringWidth(str);

    Rectangle<int> r;
    r.setSize(textWidth, fontHeight);
    r.setX(backgroundBounds.getRight() - textWidth);
    r.setCentre(r.getCentreX(), y);

    g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey);

    g.drawFittedText(str, r, juce::Justification::centredLeft, 1);

    r.setX(backgroundBounds.getX() + 1);
    g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
  }
}

void SpectrumAnalyzer::resized() {
  auto bounds = getLocalBounds();
  auto fftBounds = getAnalysisArea(bounds).toFloat();
  auto negInf =
      juce::jmap(bounds.toFloat().getBottom(), fftBounds.getBottom(),
                 fftBounds.getY(), DECIBAL_NEGATIVE_INFINITY, DECIBAL_MAX);
  leftPathProducer.setNegativeInfinity(negInf);
  rightPathProducer.setNegativeInfinity(negInf);
}

void SpectrumAnalyzer::updateGainReductions(float lowBandGR, float midBandGR,
                                            float highBandGR) {
  std::tie(lowBandGainReduction, midBandGainReduction, highBandGainReduction) =
      std::tuple{lowBandGR, midBandGR, highBandGR};

  repaint();
}

void SpectrumAnalyzer::parameterValueChanged(int parameterIndex,
                                             float newValue) {
  parametersChanged.set(true);
}

void SpectrumAnalyzer::timerCallback() {
  if (shouldShowFFTAnalysis) {
    auto fftBounds = getAnalysisArea(getLocalBounds()).toFloat();
    fftBounds.setBottom(getLocalBounds().getBottom());
    auto sampleRate = audioProcessor.getSampleRate();

    leftPathProducer.process(fftBounds, sampleRate);
    rightPathProducer.process(fftBounds, sampleRate);
  }

  if (parametersChanged.compareAndSetBool(false, true)) {
  }

  repaint();
}

juce::Rectangle<int> SpectrumAnalyzer::getRenderArea(
    juce::Rectangle<int> backgroundBounds) {
  backgroundBounds.removeFromTop(12);
  backgroundBounds.removeFromBottom(2);
  backgroundBounds.removeFromLeft(20);
  backgroundBounds.removeFromRight(20);

  return backgroundBounds;
}

juce::Rectangle<int> SpectrumAnalyzer::getAnalysisArea(
    juce::Rectangle<int> backgroundBounds) {
  auto renderBounds = getRenderArea(backgroundBounds);
  renderBounds.removeFromTop(4);
  renderBounds.removeFromBottom(4);
  return renderBounds;
}

void SpectrumAnalyzer::drawFFTAnalysis(juce::Graphics& g,
                                       juce::Rectangle<int> analysisArea) {
  using namespace juce;
  if (shouldShowFFTAnalysis) {
    auto leftChannelFFTPath = leftPathProducer.getPath();
    leftChannelFFTPath.applyTransform(AffineTransform().translation(
        analysisArea.getX(), analysisArea.getY()));

    g.setColour(Colour(97u, 18u, 167u));  // purple-
    g.strokePath(leftChannelFFTPath, PathStrokeType(1.f));

    auto rightChannelFFTPath = rightPathProducer.getPath();
    rightChannelFFTPath.applyTransform(AffineTransform().translation(
        analysisArea.getX(), analysisArea.getY()));

    g.setColour(Colour(215u, 201u, 134u));
    g.strokePath(rightChannelFFTPath, PathStrokeType(1.f));
  }
}

void SpectrumAnalyzer::drawBandParameterLinesAndGainReductions(
    juce::Graphics& g, juce::Rectangle<int> backgroundBounds) {
  using namespace juce;
  auto analysisArea = getAnalysisArea(backgroundBounds);
  const auto left = analysisArea.getX();
  const auto right = analysisArea.getRight();
  const auto top = analysisArea.getY();
  const auto bottom = analysisArea.getBottom();

  auto mapX = [left = left, width = analysisArea.getWidth()](float frequency) {
    auto normX = juce::mapFromLog10(frequency, MIN_FREQ, MAX_FREQ);
    return (left + width * normX);
  };
  auto mapY = [top = top, bottom = bottom](float gDb) {
    return juce::jmap(gDb, DECIBAL_NEGATIVE_INFINITY, DECIBAL_MAX,
                      (float)bottom, (float)top);
  };

  auto lowMidX = mapX(lowMidXoverParam->get());
  auto midHighX = mapX(midHighXoverParam->get());
  g.setColour(Colours::orange);
  g.drawVerticalLine(lowMidX, top, bottom);
  g.drawVerticalLine(midHighX, top, bottom);

  auto lowBandThresholdY = mapY(lowBandThredholdParam->get());
  auto midBandThresholdY = mapY(midBandThredholdParam->get());
  auto highBandThresholdY = mapY(highBandThredholdParam->get());
  g.setColour(Colours::yellow);
  g.drawHorizontalLine(lowBandThresholdY, left, lowMidX);
  g.drawHorizontalLine(midBandThresholdY, lowMidX, midHighX);
  g.drawHorizontalLine(highBandThresholdY, midHighX, right);

  auto zeroDbY = mapY(0.f);
  g.setColour(Colours::hotpink.withAlpha(0.3f));
  g.fillRect(Rectangle<float>::leftTopRightBottom(left, zeroDbY, lowMidX,
                                                  mapY(lowBandGainReduction)));
  g.fillRect(Rectangle<float>::leftTopRightBottom(lowMidX, zeroDbY, midHighX,
                                                  mapY(midBandGainReduction)));
  g.fillRect(Rectangle<float>::leftTopRightBottom(midHighX, zeroDbY, right,
                                                  mapY(highBandGainReduction)));
}
