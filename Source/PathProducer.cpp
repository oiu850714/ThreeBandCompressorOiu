/*
  ==============================================================================

    PathProducer.cpp
    Created: 25 Feb 2023 6:13:49pm
    Author:  oiu85

  ==============================================================================
*/

#include "PathProducer.h"

void PathProducer::process(juce::Rectangle<float> fftBounds,
                           double sampleRate) {
  juce::AudioBuffer<float> tempIncomingBuffer;
  while (channelFifo->getNumCompleteBuffersAvailable() > 0) {
    if (channelFifo->getAudioBuffer(tempIncomingBuffer)) {
      auto size = tempIncomingBuffer.getNumSamples();

      jassert(size <= monoBuffer.getNumSamples());
      size = juce::jmin(size, monoBuffer.getNumSamples());

      auto writePointer = monoBuffer.getWritePointer(0, 0);
      auto readPointer = monoBuffer.getReadPointer(0, size);

      std::copy(readPointer, readPointer + (monoBuffer.getNumSamples() - size),
                writePointer);

      juce::FloatVectorOperations::copy(
          monoBuffer.getWritePointer(0, monoBuffer.getNumSamples() - size),
          tempIncomingBuffer.getReadPointer(0, 0), size);

      channelFFTDataGenerator.produceFFTDataForRendering(monoBuffer,
                                                         negativeInfinity);
    }
  }

  const auto fftSize = channelFFTDataGenerator.getFFTSize();
  const auto binWidth = sampleRate / double(fftSize);

  while (channelFFTDataGenerator.getNumAvailableFFTDataBlocks() > 0) {
    std::vector<float> fftData;
    if (channelFFTDataGenerator.getFFTData(fftData)) {
      pathProducer.generatePath(fftData, fftBounds, fftSize, binWidth,
                                negativeInfinity);
    }
  }

  while (pathProducer.getNumPathsAvailable() > 0) {
    pathProducer.getPath(channelFFTPath);
  }
}