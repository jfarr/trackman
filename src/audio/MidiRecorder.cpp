#include "MidiRecorder.h"
#include "model/Project.h"

MidiRecorder::MidiRecorder(Project &project, juce::AudioDeviceManager &deviceManager)
    : project(project), deviceManager(deviceManager) {
    keyboardState.addListener(this);
}

MidiRecorder::~MidiRecorder() { keyboardState.removeListener(this); }
//
//void MidiRecorder::setAudioSource(juce::AudioSource *newSource) {
//    source = newSource;
//    source->prepareToPlay(deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
//}

void MidiRecorder::startRecording() {
    const juce::ScopedLock lock(mutex);
    recording = true;
}

void MidiRecorder::stopRecording() {
    const juce::ScopedLock lock(mutex);
    recording = false;
    printEvents();
}

bool MidiRecorder::isRecording() const {
    const juce::ScopedLock lock(mutex);
    return recording;
}

// juce::MidiBuffer &MidiRecorder::getBufferAtTime(double time) { return getBufferAtSampleNumber(getSampleNumber(time));
// }
//
// juce::MidiBuffer &MidiRecorder::getBufferAtSampleNumber(juce::int64 sampleNumber) {
//     int index = sampleNumber / INT_MAX;
//     DBG("accessing buffer " << index);
//     if (index >= midiEvents.size()) {
//         DBG("expanding buffer to size " << index + 1);
//         midiEvents.resize(index + 1);
//     }
//     return midiEvents.getReference(index);
// }

juce::int64 MidiRecorder::getSampleNumber(double time) const {
    return (juce::int64)time * (juce::int64)deviceManager.getAudioDeviceSetup().sampleRate;
}

// void MidiRecorder::processNextMidiBuffer(juce::MidiBuffer &buffer, double time, int startSample, int numSamples) {
//     int sampleNumber = getSampleNumber(time) + startSample;
//     int startIndex = sampleNumber / INT_MAX;
//     int endIndex = (sampleNumber + numSamples) / INT_MAX;
//     int j = startSample;
//     for (int i = startIndex; i <= endIndex; i++) {
//         auto &bufferToCopy = getBufferAtSampleNumber(i * INT_MAX);
//         for (auto event : bufferToCopy) {
//             if (event.samplePosition >= startSample && event.samplePosition < startSample + numSamples) {
//                 buffer.addEvent(event.getMessage(), j++);
//             }
//         }
//     }
// }

//==============================================================================
void MidiRecorder::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (!recording) {
        return;
    }
    auto t = juce::Time::getMillisecondCounterHiRes();
    //    DBG("handleNoteOn: " << t);
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    postMessage(m, t);
}

void MidiRecorder::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (!recording) {
        return;
    }
    auto t = juce::Time::getMillisecondCounterHiRes();
    //    DBG("handleNoteOff: " << t);
    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    postMessage(m, t);
}

void MidiRecorder::postMessage(juce::MidiMessage &message, double time) {
    (new MidiMessageCallback(this, message, time))->post();
}

void MidiRecorder::handleMessage(juce::MidiMessage &message, double time) {
    auto t = juce::Time::getMillisecondCounterHiRes();
    auto offset = (time - t) * .001;
    //    DBG("handleMessage: " << t);
    //    DBG("offset: " << offset);
    auto timestamp = project.getMixer().getTransportSource().getCurrentPosition();
    message.setTimeStamp(timestamp + offset);
    //    auto buffer = getBufferAtTime(timestamp);
    auto sampleNumber = timestamp * deviceManager.getAudioDeviceSetup().sampleRate;
    //    buffer.addEvent(message, sampleNumber);
    lastSampleNumber = sampleNumber;
    midiEvents.addEvent(message);
    //    DBG("added note on event at time: " << (timestamp + offset) << " offset: " << offset << " sample number: " <<
    //    sampleNumber);
}

//==============================================================================
void MidiRecorder::setNextReadPosition(juce::int64 position) { nextReadPosition = position; }
juce::int64 MidiRecorder::getNextReadPosition() const { return nextReadPosition; }

juce::int64 MidiRecorder::getTotalLength() const {
    return recording ? std::max(nextReadPosition, lastSampleNumber) : lastSampleNumber;
}

//==============================================================================
void MidiRecorder::prepareToPlay(int blockSize, double sampleRate) {
//    if (source != nullptr) {
//        source->prepareToPlay(blockSize, sampleRate);
//    }
}

void MidiRecorder::releaseResources() {
//    if (source != nullptr) {
//        source->releaseResources();
//    }
}

void MidiRecorder::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    nextReadPosition += bufferToFill.numSamples;
//    if (source != nullptr) {
//        source->getNextAudioBlock(bufferToFill);
//    }
}

void MidiRecorder::printEvents() {
    for (auto i = midiEvents.begin(); i != midiEvents.end(); i++) {
        auto m = (*i)->message;
        DBG(juce::String("note ") + (m.isNoteOn() ? "on" : "off") + " event at time "
            << m.getTimeStamp() << ": noteNumber=" << m.getNoteNumber() << " velocity=" << m.getVelocity());
    }
}
