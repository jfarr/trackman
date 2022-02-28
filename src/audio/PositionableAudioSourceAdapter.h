#pragma once

#include "JuceHeader.h"

class PositionableAudioSourceAdapter : public juce::PositionableAudioSource {
  public:
    PositionableAudioSourceAdapter(PositionableAudioSource *source, const bool deleteWhenRemoved)
        : source(source), deleteWhenRemoved(deleteWhenRemoved) {}
    ~PositionableAudioSourceAdapter() override {
        if (deleteWhenRemoved && source != nullptr) {
            std::unique_ptr<AudioSource> toDelete;
            toDelete.reset(source);
        }
    }

    void setNextReadPosition(juce::int64 position) override {
        source->setNextReadPosition(position);
    }

    juce::int64 getNextReadPosition() const override {
        return source->getNextReadPosition();
    }
    juce::int64 getTotalLength() const override {
        return source->getTotalLength();
    }
    bool isLooping() const override { return source->isLooping(); }
    void setLooping(bool shouldLoop) override { source->setLooping(shouldLoop); }

  protected:
    PositionableAudioSource *source;
    const bool deleteWhenRemoved;
};
