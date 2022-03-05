#pragma once

#include <JuceHeader.h>

class FileDragDropTarget : public juce::FileDragAndDropTarget {
  public:
    bool isInterestedInFileDrag(const juce::StringArray &files) override { return true; }
};
