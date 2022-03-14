#pragma once

#include "JuceHeader.h"

class PositionableAudioSourceAdapter : public juce::PositionableAudioSource {
  public:
    PositionableAudioSourceAdapter(PositionableAudioSource *source, const bool deleteWhenRemoved)
        : source(source), deleteWhenRemoved(deleteWhenRemoved) {}
    ~PositionableAudioSourceAdapter() override {
        if (deleteWhenRemoved && source != nullptr) {
            source->releaseResources();
            std::unique_ptr<AudioSource> toDelete;
            toDelete.reset(source);
        }
    }

    virtual void setNextReadPosition(juce::int64 position) override { source->setNextReadPosition(position); }
    virtual juce::int64 getNextReadPosition() const override { return source->getNextReadPosition(); }
    virtual juce::int64 getTotalLength() const override { return source->getTotalLength(); }
    virtual bool isLooping() const override { return source->isLooping(); }
    virtual void setLooping(bool shouldLoop) override { source->setLooping(shouldLoop); }

  protected:
    PositionableAudioSource *source;
    const bool deleteWhenRemoved;
};
