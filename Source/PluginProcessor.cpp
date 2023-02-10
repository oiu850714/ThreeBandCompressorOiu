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
  const NormalisableRange<float> attackRange{5.f, 500.f, 1.f, 1.f},
      releaseRange = attackRange;
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
          params.at(Names::Threshold_Low_Band),
          params.at(Names::Threshold_Low_Band),
          NormalisableRange<float>(-60.f, 12.f, 1.f, 1.f), 0.f),
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
  };
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
      compressor(apvts) {
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

  compressor.prepare(spec);
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

void ThreeBandCompressorOiuAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  compressor.updateCompressorSettings();
  compressor.process(buffer);
}

//==============================================================================
bool ThreeBandCompressorOiuAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor*
ThreeBandCompressorOiuAudioProcessor::createEditor() {
  return new juce::GenericAudioProcessorEditor(*this);
  // return new ThreeBandCompressorOiuAudioProcessorEditor (*this);
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
