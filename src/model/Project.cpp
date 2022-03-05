#include <fstream>
#include <nlohmann/json.hpp>

#include "Project.h"

using json = nlohmann::json;

std::string Project::to_json() {
    json project_json = {{"mixer", {{"gain", mixer.getMasterLevelGain()}, {"muted", mixer.isMasterMuted()}}}};
    project_json["tracks"] = json::array();
    trackList.eachTrack([&project_json](Track &track) {
        json track_json = {{"name", track.getName().toStdString()},
            {"file", track.getFile().getFullPathName().toStdString()}, {"gain", track.getLevelGain()},
            {"muted", track.isMuted()}};
        track.eachSample([&track_json](Sample &sample) {
            json sample_json = {{"file", sample.getFile().getFullPathName().toStdString()},
                {"startPos", sample.getStartPos()}, {"endPos", sample.getEndPos()},
                {"length", sample.getLength()}, {"sampleRate", sample.getSampleRate()}};
            track_json["samples"].push_back(sample_json);
        });
        project_json["tracks"].push_back(track_json);
    });
    return project_json.dump();
}

void Project::from_json(juce::AudioFormatManager &formatManager, std::string filename) {
    std::ifstream s(filename);
    json j;
    s >> j;
    mixer.setMasterLevelGain(j["mixer"]["gain"]);
    mixer.setMasterMute(j["mixer"]["muted"]);
    trackList.clear();
    for (auto t : j["tracks"]) {
        auto track = trackList.addTrack(t["name"]);
        track->setFile(t["file"]);
        track->loadFile(formatManager, t["file"]);
        track->setLevelGain(t["gain"]);
        track->setMute(t["muted"]);
    }
}
