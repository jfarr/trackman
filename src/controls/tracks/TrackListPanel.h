#pragma once

#include "JuceHeader.h"
#include "TrackLaneControl.h"
#include "model/Track.h"
#include "model/TrackList.h"

class DropBox : public juce::Component {
  public:
    DropBox() { setSize(200, 80); }
    ~DropBox() {}

    void paint(juce::Graphics &g) override {
        g.fillAll(juce::Colours::dimgrey);
        g.setColour(juce::Colours::grey);
        g.drawRect(0, 0, getWidth(), getHeight());
    }
};

class TrackListPanel : public juce::Component {
  public:
    TrackListPanel(TrackList &trackList, juce::Viewport &viewport);
    ~TrackListPanel();

    Track *getTrackAtPos(int x, int y);
    DropBox &getDropBox() { return dropBox; }

    void addLane(TrackLaneControl *lane) { lanes.push_back(lane); }
    void resize();
    void clear() { lanes.clear(); }

    void update();

    void fileDragEnter(const juce::StringArray &files, int x, int y);
    void fileDragMove(const juce::StringArray &files, int x, int y);
    void fileDragExit(const juce::StringArray &files);
    void filesDropped (const juce::StringArray& files, int x, int y);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    TrackList &trackList;
    juce::Viewport &viewport;
    std::list<TrackLaneControl *> lanes;
    float scale = 25;

    DropBox dropBox;

    int getTrackLaneWidth() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListPanel)
};
