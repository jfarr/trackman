#pragma once

#include "ui/common/TransportControl.h"
#include <JuceHeader.h>

class DesktopController;

namespace trackman {

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

class NumeratorTextEditor : public juce::Label {
  public:
    NumeratorTextEditor() { setEditable(true); }
    ~NumeratorTextEditor() override = default;

  private:
    juce::TextEditor *createEditorComponent() override {
        auto editor = juce::Label::createEditorComponent();
        editor->setInputRestrictions(2, "0123456789");
        return editor;
    }
};

class DenominatorLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    DenominatorLookAndFeel() = default;
    ~DenominatorLookAndFeel() override = default;

    void drawComboBox(
        juce::Graphics &g, int width, int height, bool, int, int, int, int, juce::ComboBox &box) override {
        auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        juce::Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

        g.setColour(box.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

        juce::Rectangle<int> arrowZone(width - 22, 0, 12, height);
        juce::Path path;
        path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
        path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
        path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, juce::PathStrokeType(2.0f));
    }

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DenominatorLookAndFeel)
};

class TransportPanel : public juce::Component {
  public:
    TransportPanel(DesktopController &desktopController);
    ~TransportPanel() noexcept override;

    int getPreferredHeight() const { return preferredHeight; }

    void update();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    const int preferredWidth = 800;
    const int preferredHeight = 35;
    const int transportWidth = 550;
    const int transportMargin = 5;

    DesktopController &desktopController;
    TransportControl transportControl;
    juce::Label tempoLabel;
    TempoTextEditor tempoText;

    NumeratorTextEditor numeratorText;
    juce::ComboBox denominatorSelect;
    juce::Label timeSignatureDivider;
    DenominatorLookAndFeel denominatorLF;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportPanel)
};

} // namespace trackman
