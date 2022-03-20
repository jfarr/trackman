#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MasterTrackListener.h"
#include "TrackControl.h"
#include "model/Mixer.h"
#include "model/TrackList.h"
#include "ui/common/TransportControl.h"

class DesktopController;

class TempoTextEditor : public juce::Label {
  public:
    TempoTextEditor() { setEditable(true); }
    ~TempoTextEditor() override = default;

  private:
    juce::TextEditor *createEditorComponent() override {
        auto editor = juce::Label::createEditorComponent();
        editor->setInputRestrictions(5, "0123456789.");
        return editor;
    }
};

// class TempoTextEditor : public juce::Label {
//   public:
//     TempoTextEditor() { setEditable(true); }
//     ~TempoTextEditor() override = default;
//
//   private:
//     juce::TextEditor *createEditorComponent() override {
//         auto editor = juce::Label::createEditorComponent();
//         editor->setInputRestrictions(5, "0123456789.");
//         return editor;
//     }
// };

class DenominatorLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    DenominatorLookAndFeel() = default;
    ~DenominatorLookAndFeel() = default;

    void drawComboBox (juce::Graphics& g, int width, int height, bool,
        int, int, int, int, juce::ComboBox& box) override
    {
        auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        juce::Rectangle<int> boxBounds (0, 0, width, height);

        g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

        g.setColour (box.findColour (juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

        juce::Rectangle<int> arrowZone (width - 22, 0, 12, height);
        juce::Path path;
        path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
        path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
        path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

        g.setColour (box.findColour (juce::ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath (path, juce::PathStrokeType (2.0f));
    }

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DenominatorLookAndFeel)

};

class TimeSignatureDenominatorTextEditor : public juce::Label {
  public:
    TimeSignatureDenominatorTextEditor() { setEditable(true); }
    ~TimeSignatureDenominatorTextEditor() override = default;

  private:
    juce::TextEditor *createEditorComponent() override {
        auto editor = juce::Label::createEditorComponent();
        editor->setInputRestrictions(2, "1486");
        return editor;
    }
};

class MixerPanel : public juce::Component {
  public:
    MixerPanel(DesktopController &desktopController, foleys::LevelMeterSource &meterSource);
    ~MixerPanel() override;

    int getPreferredHeight() const { return std::max(getHeight(), 280); }

    TransportControl &getTransportControl() { return transportControl; }
    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }

    void update();

    void addListener(MasterTrackListener *listener);
    void removeListener(MasterTrackListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    const int preferredWidth = 800;
    const int preferredHeight = 280;
    const int transportHeight = 35;
    const int transportWidth = 550;
    const int transportMargin = 5;

    DesktopController &desktopController;
    juce::Viewport &mixerViewport;

    TransportControl transportControl;
    MasterTrackControl masterTrackControl;

    juce::Label tempoLabel;
    TempoTextEditor tempoText;
    float previousTempo;

//    TimeSignatureDenominatorTextEditor denominatorText;
    juce::ComboBox denominatorSelect;
    juce::Label timeSignatureDivider;
    DenominatorLookAndFeel denominatorLF;

    std::list<MasterTrackListener *> listeners;

    void createControls();
    void tempoChanged();

    void notifyTempoChanged(float previousTempo, float newTempo);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};
