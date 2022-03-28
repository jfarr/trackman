#pragma once

#include <JuceHeader.h>

#include "ui/common/TransportControl.h"

using namespace juce;

namespace trackman {

class DesktopController;

class TempoTextEditor : public Label {
  public:
    TempoTextEditor() { setEditable(true); }
    ~TempoTextEditor() override = default;

  private:
    TextEditor *createEditorComponent() override {
        auto editor = Label::createEditorComponent();
        editor->setInputRestrictions(5, "0123456789.");
        return editor;
    }
};

class NumeratorTextEditor : public Label {
  public:
    NumeratorTextEditor() { setEditable(true); }
    ~NumeratorTextEditor() override = default;

  private:
    TextEditor *createEditorComponent() override {
        auto editor = Label::createEditorComponent();
        editor->setInputRestrictions(2, "0123456789");
        return editor;
    }
};

class DenominatorLookAndFeel : public LookAndFeel_V4 {
  public:
    DenominatorLookAndFeel() = default;
    ~DenominatorLookAndFeel() override = default;

    void drawComboBox(
        Graphics &g, int width, int height, bool, int, int, int, int, ComboBox &box) override {
        auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(box.findColour(ComboBox::backgroundColourId));
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

        g.setColour(box.findColour(ComboBox::outlineColourId));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

        Rectangle<int> arrowZone(width - 22, 0, 12, height);
        Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, PathStrokeType(2.0f));
    }

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DenominatorLookAndFeel)
};

class TransportPanel : public Component {
  public:
    TransportPanel(DesktopController &desktopController);
    ~TransportPanel() noexcept override;

    int getPreferredHeight() const { return preferredHeight; }

    void update();
    void selectionChanged(Track *track);

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

  private:
    const int preferredWidth = 800;
    const int preferredHeight = 35;
    const int transportWidth = 600;
    const int transportMargin = 5;

    DesktopController &desktopController;
    TransportControl transportControl;
    Label tempoLabel;
    TempoTextEditor tempoText;

    NumeratorTextEditor numeratorText;
    ComboBox denominatorSelect;
    Label timeSignatureDivider;
    DenominatorLookAndFeel denominatorLF;

    //==============================================================================

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportPanel)
};

} // namespace trackman
