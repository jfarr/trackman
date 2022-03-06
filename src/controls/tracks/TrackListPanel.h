#pragma once

#include "JuceHeader.h"
#include "TrackLaneControl.h"
#include "controls/common/PositionOverlay.h"
#include "controls/common/TimeMeter.h"
#include "model/Track.h"
#include "model/TrackList.h"

class DropBox : public juce::Component {
  public:
    DropBox() { setSize(200, 85); }
    ~DropBox() {}

    void setSource(std::unique_ptr<juce::AudioFormatReaderSource> &newSource) { source = std::move(newSource); }

    void paint(juce::Graphics &g) override {
        g.fillAll(juce::Colours::dimgrey);
        g.setColour(juce::Colours::grey);
        g.drawRect(0, 0, getWidth(), getHeight());
    }

  private:
    std::unique_ptr<juce::AudioFormatReaderSource> source;
};

class TrackListPanel : public juce::Component, public juce::DragAndDropContainer {
  public:
    TrackListPanel(TrackList &trackList, juce::Viewport &viewport, juce::AudioTransportSource &transport,
        juce::AudioFormatManager &formatManager);
    ~TrackListPanel();

    Track *getTrackAtPos(int x, int y);
    //    DropBox &getDropBox() { return dropBox; }

    void addLane(TrackLaneControl *lane) { lanes.push_back(lane); }
    void resize();
    void clear() { lanes.clear(); }

    void update();

    void fileDragEnter(const juce::StringArray &files, int x, int y);
    void fileDragMove(const juce::StringArray &files, int x, int y);
    void fileDragExit(const juce::StringArray &files);
    void filesDropped(const juce::StringArray &files, int x, int y);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    TrackList &trackList;
    juce::Viewport &viewport;
    juce::AudioFormatManager &formatManager;
    std::list<TrackLaneControl *> lanes;
    float scale = 75;

    TimeMeter timeMeter;
    DropBox dropBox;

    void createControls();
    int getTrackLaneWidth() const;
    int getTrackLaneHeight() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListPanel)
};
