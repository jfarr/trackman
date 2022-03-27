#pragma once

#include <JuceHeader.h>

#include "NoteCanvas.h"
#include "SampleThumbnail.h"
#include "model/Track.h"

using namespace std;
using namespace juce;

namespace trackman {

class TrackLaneControl : public Component {
  public:
    TrackLaneControl(Project &project, Track &track, AudioTransportSource &transport);
    ~TrackLaneControl();

    Track &getTrack() { return track; }
    int getPreferredHeight() const { return preferredHeight; }

    void addSampleThumbnail(SampleThumbnail *thumbnail);
    void addNoteCanvas(NoteCanvas *canvas);
    void clear();
    void update();

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

  private:
    const int preferredWidth = 800;
    const int preferredHeight = 75;

    Project &project;
    Track &track;
    AudioTransportSource &transport;

    Label trackLabel;
    list<SampleThumbnail *> thumbnails;
    list<NoteCanvas *> canvases;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneControl)
};

} // namespace trackman
