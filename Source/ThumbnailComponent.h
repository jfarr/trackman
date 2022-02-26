#pragma once

#include <JuceHeader.h>


class ThumbnailComponent : public juce::Component, public juce::ChangeListener
{
public:
    ThumbnailComponent(juce::AudioFormatManager& formatManager);
    ~ThumbnailComponent();

    void ThumbnailComponent::setSource(const juce::File& file);

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;

    //==============================================================================
    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    void paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void thumbnailChanged();
};


class PositionOverlay : public juce::Component,
    private juce::Timer
{
public:
    PositionOverlay(juce::AudioTransportSource& transportSource);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;

private:
    void timerCallback() override;

    juce::AudioTransportSource& transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlay)
};
