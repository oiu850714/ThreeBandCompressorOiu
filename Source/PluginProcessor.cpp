/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "ParamIDConst.h"
#include "PluginEditor.h"

using APVTS = juce::AudioProcessorValueTreeState;
static APVTS::ParameterLayout createParameterLayout() {
  using namespace juce;

  using namespace Params;
  const auto& params = Params::getParams();
  const NormalisableRange<float> gainRange{-24.f, 24.f, 0.5f, 1.f};
  const NormalisableRange<float> thresholdRange(COMPRESSOR_MIN_THRESHOLD,
                                                DECIBAL_MAX, 1.f, 1.f);
  const NormalisableRange<float> attackRange{5.f, 500.f, 1.f, 1.f},
      releaseRange = attackRange;
  const NormalisableRange<float> lowMidCutRange{MIN_FREQ, 999.f, 1.f, 0.2f};
  const NormalisableRange<float> MidHighCutRange{1000.f, MAX_FREQ, 1.f, 0.2f};
  const auto ratioChoices = [] {
    std::vector<double> choices{1, 1.5, 2,  3,  4,  5,  6,
                                7, 8,   10, 15, 20, 50, 100};
    StringArray choicesStr;
    for (auto choice : choices) {
      choicesStr.add(String{choice, 1});
    }
    return choicesStr;
  }();
  return {
      std::make_unique<AudioParameterFloat>(
          params.at(Names::Gain_In), params.at(Names::Gain_In), gainRange, 0.f),
      std::make_unique<AudioParameterFloat>(params.at(Names::Gain_Out),
                                            params.at(Names::Gain_Out),
                                            gainRange, 0.f),
      std::make_unique<AudioParameterFloat>(
          params.at(Names::Threshold_Low_Band),
          params.at(Names::Threshold_Low_Band), thresholdRange, 0.f),
      std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Low_Band),
                                            params.at(Names::Attack_Low_Band),
                                            attackRange, 50.f),
      std::make_unique<AudioParameterFloat>(params.at(Names::Release_Low_Band),
                                            params.at(Names::Release_Low_Band),
                                            releaseRange, 250.f),
      std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Low_Band),
                                             params.at(Names::Ratio_Low_Band),
                                             ratioChoices, 3),
      std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_Low_Band),
                                           params.at(Names::Bypassed_Low_Band),
                                           false),
      std::make_unique<AudioParameterBool>(params.at(Names::Mute_Low_Band),
                                           params.at(Names::Mute_Low_Band),
                                           false),
      std::make_unique<AudioParameterBool>(params.at(Names::Solo_Low_Band),
                                           params.at(Names::Solo_Low_Band),
                                           false),
      std::make_unique<AudioParameterFloat>(
          params.at(Names::Threshold_Mid_Band),
          params.at(Names::Threshold_Mid_Band), thresholdRange, 0.f),
      std::make_unique<AudioParameterFloat>(params.at(Names::Attack_Mid_Band),
                                            params.at(Names::Attack_Mid_Band),
                                            attackRange, 50.f),
      std::make_unique<AudioParameterFloat>(params.at(Names::Release_Mid_Band),
                                            params.at(Names::Release_Mid_Band),
                                            releaseRange, 250.f),
      std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_Mid_Band),
                                             params.at(Names::Ratio_Mid_Band),
                                             ratioChoices, 3),
      std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_Mid_Band),
                                           params.at(Names::Bypassed_Mid_Band),
                                           false),
      std::make_unique<AudioParameterBool>(params.at(Names::Mute_Mid_Band),
                                           params.at(Names::Mute_Mid_Band),
                                           false),
      std::make_unique<AudioParameterBool>(params.at(Names::Solo_Mid_Band),
                                           params.at(Names::Solo_Mid_Band),
                                           false),
      std::make_unique<AudioParameterFloat>(
          params.at(Names::Threshold_High_Band),
          params.at(Names::Threshold_High_Band), thresholdRange, 0.f),
      std::make_unique<AudioParameterFloat>(params.at(Names::Attack_High_Band),
                                            params.at(Names::Attack_High_Band),
                                            attackRange, 50.f),
      std::make_unique<AudioParameterFloat>(params.at(Names::Release_High_Band),
                                            params.at(Names::Release_High_Band),
                                            releaseRange, 250.f),
      std::make_unique<AudioParameterChoice>(params.at(Names::Ratio_High_Band),
                                             params.at(Names::Ratio_High_Band),
                                             ratioChoices, 3),
      std::make_unique<AudioParameterBool>(params.at(Names::Bypassed_High_Band),
                                           params.at(Names::Bypassed_High_Band),
                                           false),
      std::make_unique<AudioParameterBool>(params.at(Names::Mute_High_Band),
                                           params.at(Names::Mute_High_Band),
                                           false),
      std::make_unique<AudioParameterBool>(params.at(Names::Solo_High_Band),
                                           params.at(Names::Solo_High_Band),
                                           false),
      std::make_unique<AudioParameterFloat>(
          params.at(Names::Low_Mid_Crossover_Freq),
          params.at(Names::Low_Mid_Crossover_Freq), lowMidCutRange, 400.f),
      std::make_unique<AudioParameterFloat>(
          params.at(Names::Mid_High_Crossover_Freq),
          params.at(Names::Mid_High_Crossover_Freq), MidHighCutRange, 2000.f),
  };
}

static void applyGain(juce::AudioBuffer<float>& buffer,
                      juce::dsp::Gain<float>& gain) {
  auto block = juce::dsp::AudioBlock<float>(buffer);
  auto ctx = juce::dsp::ProcessContextReplacing(block);
  gain.process(ctx);
}

//==============================================================================
ThreeBandCompressorOiuAudioProcessor::ThreeBandCompressorOiuAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              ),
#endif
      apvts{*this, nullptr, "Parameters", createParameterLayout()},
      compressors{CompressorBand{apvts, CompressorBand::Low},
                  CompressorBand{apvts, CompressorBand::Mid},
                  CompressorBand{apvts, CompressorBand::High}} {
  using namespace Params;

  const auto params = Params::getParams();
  auto setParam = [&apvts = apvts, &params](auto& paramPtr,
                                            const auto& paramName) {
    paramPtr = static_cast<std::remove_reference_t<decltype(paramPtr)>>(
        apvts.getParameter(params.at(paramName)));
  };
  setParam(lowMidCrossover, Names::Low_Mid_Crossover_Freq);
  setParam(midHighCrossover, Names::Mid_High_Crossover_Freq);
  setParam(inputGainParam, Names::Gain_In);
  setParam(outputGainParam, Names::Gain_Out);

  LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
  HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
  AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
  LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
  HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

ThreeBandCompressorOiuAudioProcessor::~ThreeBandCompressorOiuAudioProcessor() {}

//==============================================================================
const juce::String ThreeBandCompressorOiuAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool ThreeBandCompressorOiuAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool ThreeBandCompressorOiuAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool ThreeBandCompressorOiuAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double ThreeBandCompressorOiuAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int ThreeBandCompressorOiuAudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int ThreeBandCompressorOiuAudioProcessor::getCurrentProgram() { return 0; }

void ThreeBandCompressorOiuAudioProcessor::setCurrentProgram(int index) {}

const juce::String ThreeBandCompressorOiuAudioProcessor::getProgramName(
    int index) {
  return {};
}

void ThreeBandCompressorOiuAudioProcessor::changeProgramName(
    int index, const juce::String& newName) {}

//==============================================================================
void ThreeBandCompressorOiuAudioProcessor::prepareToPlay(double sampleRate,
                                                         int samplesPerBlock) {
  juce::dsp::ProcessSpec spec;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = getTotalNumOutputChannels();
  spec.sampleRate = sampleRate;

  for (auto& compressor : compressors) {
    compressor.prepare(spec);
  }

  LP1.prepare(spec);
  HP1.prepare(spec);
  AP2.prepare(spec);
  LP2.prepare(spec);
  HP2.prepare(spec);
  inputGain.prepare(spec);
  outputGain.prepare(spec);

  for (auto& buffer : splittedBuffers) {
    buffer.setSize(spec.numChannels, samplesPerBlock);
  }
  inputGain.setRampDurationSeconds(0.05);
  outputGain.setRampDurationSeconds(0.05);

  leftChannelFifo.prepare(samplesPerBlock);
  rightChannelFifo.prepare(samplesPerBlock);
}

void ThreeBandCompressorOiuAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ThreeBandCompressorOiuAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void ThreeBandCompressorOiuAudioProcessor::updateDSPStates() {
  inputGain.setGainDecibels(inputGainParam->get());
  outputGain.setGainDecibels(outputGainParam->get());

  auto lowMidcutOffFreq = lowMidCrossover->get();
  auto midHighcutOffFreq = midHighCrossover->get();
  LP1.setCutoffFrequency(lowMidcutOffFreq);
  HP1.setCutoffFrequency(lowMidcutOffFreq);
  AP2.setCutoffFrequency(midHighcutOffFreq);
  LP2.setCutoffFrequency(midHighcutOffFreq);
  HP2.setCutoffFrequency(midHighcutOffFreq);

  for (size_t i = 0; i < 3; ++i) {
    compressors[i].updateCompressorSettings();
  }
}

void ThreeBandCompressorOiuAudioProcessor::splitBands(
    const juce::AudioBuffer<float>& buffer) {
  for (auto& splittedBuffer : splittedBuffers) {
    // Let LinkwitzRileyFilter operate on copied buffer and finally combine the
    // results back.
    splittedBuffer = buffer;
  }
  auto SB0 = juce::dsp::AudioBlock<float>{splittedBuffers[0]};
  auto SB1 = juce::dsp::AudioBlock<float>{splittedBuffers[1]};
  auto SB2 = juce::dsp::AudioBlock<float>{splittedBuffers[2]};
  auto SB0Ctx = juce::dsp::ProcessContextReplacing<float>(SB0);
  auto SB1Ctx = juce::dsp::ProcessContextReplacing<float>(SB1);
  auto SB2Ctx = juce::dsp::ProcessContextReplacing<float>(SB2);

  // See https://youtu.be/Mo0Oco3Vimo?t=9608

  // First band.
  LP1.process(SB0Ctx);
  AP2.process(SB0Ctx);

  // Second band.
  HP1.process(SB1Ctx);
  // See the video above.
  // We need to first copy the filtered result to the third buffer;
  // after that we continue to process the result in the second buffer.
  splittedBuffers[2] = splittedBuffers[1];
  LP2.process(SB1Ctx);

  // Third band.
  HP2.process(SB2Ctx);
}

void ThreeBandCompressorOiuAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  updateDSPStates();

  leftChannelFifo.update(buffer);
  rightChannelFifo.update(buffer);

  applyGain(buffer, inputGain);

  splitBands(buffer);

  // Compress each band after we split them.
  for (size_t i = 0; i < 3; ++i) {
    compressors[i].process(splittedBuffers[i]);
  }

  // Now add the filtered three band results to the output.
  auto numSamples = buffer.getNumSamples();
  auto numChannels = buffer.getNumChannels();
  buffer.clear();

  auto addBackSplittedBuffer = [ns = numSamples, nc = numChannels](
                                   auto& buffer, const auto& source) {
    for (auto i = 0; i < nc; ++i) {
      buffer.addFrom(i, 0, source, i, 0, ns);
    }
  };

  if (compressors[0].isSolo() || compressors[1].isSolo() ||
      compressors[2].isSolo()) {
    // If any of compressor is in solo, then we add the corresponding buffer
    // only. (But we prefer compressor with smaller index if multiple
    // compressors are in solo.)
    compressors[0].isSolo() ? addBackSplittedBuffer(buffer, splittedBuffers[0])
    : compressors[1].isSolo()
        ? addBackSplittedBuffer(buffer, splittedBuffers[1])
        : addBackSplittedBuffer(buffer, splittedBuffers[2]);
  } else {
    // Otherwise, we add buffers for compressors that are not muted.
    for (auto i = 0; i < 3; ++i) {
      if (!compressors[i].isMute()) {
        addBackSplittedBuffer(buffer, splittedBuffers[i]);
      }
    }
  }

  applyGain(buffer, outputGain);
}

//==============================================================================
bool ThreeBandCompressorOiuAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor*
ThreeBandCompressorOiuAudioProcessor::createEditor() {
  return new ThreeBandCompressorOiuAudioProcessorEditor(*this);
}

//==============================================================================
void ThreeBandCompressorOiuAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
  juce::MemoryOutputStream mos(destData, true);
  apvts.state.writeToStream(mos);
}

void ThreeBandCompressorOiuAudioProcessor::setStateInformation(
    const void* data, int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
  auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
  if (tree.isValid()) {
    apvts.replaceState(tree);
  }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new ThreeBandCompressorOiuAudioProcessor();
}
