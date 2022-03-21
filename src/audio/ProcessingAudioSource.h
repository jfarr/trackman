#pragma once

#include <JuceHeader.h>

using namespace std;
using namespace juce;
using namespace juce::dsp;

namespace trackman {

template <typename ProcessorType> class ProcessingAudioSource : public PositionableAudioSource {
  public:
    ProcessingAudioSource(
        PositionableAudioSource *source, ProcessorWrapper<ProcessorType> *processor, const bool deleteWhenRemoved);
    ~ProcessingAudioSource() override;

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override;
    int64 getNextReadPosition() const override;
    int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    PositionableAudioSource *source;
    ProcessorWrapper<ProcessorType> *processor;
    CriticalSection audioCallbackLock;
    const bool deleteWhenRemoved;
};

template <typename ProcessorType>
ProcessingAudioSource<ProcessorType>::ProcessingAudioSource(
    PositionableAudioSource *source, ProcessorWrapper<ProcessorType> *processor, const bool deleteWhenRemoved)
    : source(source), processor(processor), deleteWhenRemoved(deleteWhenRemoved) {}

template <typename ProcessorType> ProcessingAudioSource<ProcessorType>::~ProcessingAudioSource() {
    if (deleteWhenRemoved && source != nullptr) {
        unique_ptr<AudioSource> toDelete;
        toDelete.reset(source);
    }
}

//==============================================================================
template <typename ProcessorType>
void ProcessingAudioSource<ProcessorType>::prepareToPlay(int blockSize, double sampleRate) {
    source->prepareToPlay(blockSize, sampleRate);
    processor->prepare({sampleRate, (uint32)blockSize, 2});
}

template <typename ProcessorType> void ProcessingAudioSource<ProcessorType>::releaseResources() {
    source->releaseResources();
}

template <typename ProcessorType>
void ProcessingAudioSource<ProcessorType>::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    source->getNextAudioBlock(bufferToFill);

    AudioBlock<float> block(*bufferToFill.buffer, (size_t)bufferToFill.startSample);

    ScopedLock audioLock(audioCallbackLock);
    processor->process(ProcessContextReplacing<float>(block));
}

//==============================================================================
template <typename ProcessorType> void ProcessingAudioSource<ProcessorType>::setNextReadPosition(int64 position) {
    source->setNextReadPosition(position);
}

template <typename ProcessorType> int64 ProcessingAudioSource<ProcessorType>::getNextReadPosition() const {
    return source->getNextReadPosition();
}

template <typename ProcessorType> int64 ProcessingAudioSource<ProcessorType>::getTotalLength() const {
    return source->getTotalLength();
}

template <typename ProcessorType> bool ProcessingAudioSource<ProcessorType>::isLooping() const {
    return source->isLooping();
}

template <typename ProcessorType> void ProcessingAudioSource<ProcessorType>::setLooping(bool shouldLoop) {
    source->setLooping(shouldLoop);
}

} // namespace trackman
