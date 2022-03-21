#pragma once

#include <JuceHeader.h>

using namespace juce;

namespace trackman {

class FileDragDropTarget : public FileDragAndDropTarget {
  public:
    bool isInterestedInFileDrag(const StringArray &files) override { return true; }
};

} // namespace trackman
