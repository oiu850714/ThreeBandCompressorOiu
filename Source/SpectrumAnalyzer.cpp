/*

 * ==============================================================================


 * SpectrumAnalyzer.cpp
    Created: 25 Feb 2023 4:51:26pm
    Author:  oiu85


 * ==============================================================================
*/

#include "SpectrumAnalyzer.h"

#include "ComponentDrawingUtils.h"

SpectrumAnalyzer::SpectrumAnalyzer(ThreeBandCompressorOiuAudioProcessor& p)
    : audioProcessor(p),
      leftPathProducer(audioProcessor.leftChannelFifo),
      rightPathProducer(audioProcessor.rightChannelFifo) {
  const auto& params = audioProcessor.getParameters();
  for (auto param : params) {
    param->addListener(this);
  }

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

  drawTextLabels(g, backgroundBounds);
}

std::vector<float> SpectrumAnalyzer::getFrequencies() {
  return std::vector<float>{
      20,   /*30, 40,*/ 50,       100,   200,  /*300, 400,*/ 500, 1000,
      2000, /*3000, 4000,*/ 5000, 10000, 20000};
}

std::vector<float> SpectrumAnalyzer::getGains() {
  std::vector<float> gains;
  for (auto gain = DECIBAL_NEGATIVE_INFINITY; gain <= DECIBAL_MAX; gain += DECIBAL_INCREMENT) {
    gains.push_back(gain);
  }
  return gains;
}

std::vector<float> SpectrumAnalyzer::getXs(const std::vector<float>& freqs,
                                           float left, float width) {
  std::vector<float> xs;
  for (auto f : freqs) {
    auto normX = juce::mapFromLog10(f, 20.f, 20000.f);
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
    auto y = jmap(gDb, DECIBAL_NEGATIVE_INFINITY, DECIBAL_MAX, float(bottom), float(top));

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
    auto y = jmap(gDb, DECIBAL_NEGATIVE_INFINITY, DECIBAL_MAX, float(bottom), float(top));

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
  auto negInf = juce::jmap(bounds.toFloat().getBottom(), fftBounds.getBottom(),
                           fftBounds.getY(), DECIBAL_NEGATIVE_INFINITY, DECIBAL_MAX);
  leftPathProducer.setNegativeInfinity(negInf);
  rightPathProducer.setNegativeInfinity(negInf);
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
