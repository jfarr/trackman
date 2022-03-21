#pragma once

#include <JuceHeader.h>

using namespace std;
using namespace juce;

namespace trackman {

class PositionableAudioSourceAdapter : public PositionableAudioSource {
  public:
    PositionableAudioSourceAdapter(PositionableAudioSource *source, const bool deleteWhenRemoved)
        : source(source), deleteWhenRemoved(deleteWhenRemoved) {}
    ~PositionableAudioSourceAdapter() override {
        if (deleteWhenRemoved && source != nullptr) {
            source->releaseResources();
            unique_ptr<AudioSource> toDelete;
            toDelete.reset(source);
        }
    }

    void setNextReadPosition(int64 position) override { source->setNextReadPosition(position); }
    int64 getNextReadPosition() const override { return source->getNextReadPosition(); }
    int64 getTotalLength() const override { return source->getTotalLength(); }
    bool isLooping() const override { return source->isLooping(); }
    void setLooping(bool shouldLoop) override { source->setLooping(shouldLoop); }

  protected:
    PositionableAudioSource *source;
    const bool deleteWhenRemoved;
};

} // namespace trackman
