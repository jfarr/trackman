#pragma once

#include <JuceHeader.h>

template <typename ProcessorType> class ProcessingAudioSource : public juce::PositionableAudioSource {
  public:
    ProcessingAudioSource(juce::PositionableAudioSource *source, juce::dsp::ProcessorWrapper<ProcessorType> *processor,
        const bool deleteWhenRemoved);
    ~ProcessingAudioSource() override;

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    juce::PositionableAudioSource *source;
    juce::dsp::ProcessorWrapper<ProcessorType> *processor;
    juce::CriticalSection audioCallbackLock;
    const bool deleteWhenRemoved;
};

template <typename ProcessorType>
ProcessingAudioSource<ProcessorType>::ProcessingAudioSource(juce::PositionableAudioSource *source,
    juce::dsp::ProcessorWrapper<ProcessorType> *processor, const bool deleteWhenRemoved)
    : source(source), processor(processor), deleteWhenRemoved(deleteWhenRemoved) {}

template <typename ProcessorType> ProcessingAudioSource<ProcessorType>::~ProcessingAudioSource() {
    if (deleteWhenRemoved && source != nullptr) {
        std::unique_ptr<AudioSource> toDelete;
        toDelete.reset(source);
    }
}

//==============================================================================
template <typename ProcessorType>
void ProcessingAudioSource<ProcessorType>::prepareToPlay(int blockSize, double sampleRate) {
    source->prepareToPlay(blockSize, sampleRate);
    processor->prepare({sampleRate, (juce::uint32)blockSize, 2});
}

template <typename ProcessorType> void ProcessingAudioSource<ProcessorType>::releaseResources() {
    source->releaseResources();
}

template <typename ProcessorType>
void ProcessingAudioSource<ProcessorType>::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    source->getNextAudioBlock(bufferToFill);

    juce::dsp::AudioBlock<float> block(*bufferToFill.buffer, (size_t)bufferToFill.startSample);

    juce::ScopedLock audioLock(audioCallbackLock);
    processor->process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
template <typename ProcessorType> void ProcessingAudioSource<ProcessorType>::setNextReadPosition(juce::int64 position) {
    source->setNextReadPosition(position);
}

template <typename ProcessorType> juce::int64 ProcessingAudioSource<ProcessorType>::getNextReadPosition() const {
    return source->getNextReadPosition();
}

template <typename ProcessorType> juce::int64 ProcessingAudioSource<ProcessorType>::getTotalLength() const {
    return source->getTotalLength();
}

template <typename ProcessorType> bool ProcessingAudioSource<ProcessorType>::isLooping() const {
    return source->isLooping();
}

template <typename ProcessorType> void ProcessingAudioSource<ProcessorType>::setLooping(bool shouldLoop) {
    source->setLooping(shouldLoop);
}
