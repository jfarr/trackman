#include <fstream>
#include <nlohmann/json.hpp>

#include "Project.h"

using json = nlohmann::json;

std::string Project::to_json() {
    json j = {{"mixer", {{"gain", mixer.getMasterLevelGain()}, {"muted", mixer.isMasterMuted()}}}};
    j["tracks"] = json::array();
    trackList.eachTrack([&j](Track &track) {
        json t = {{"name", track.getName().toStdString()}, {"file", track.getFile().getFullPathName().toStdString()},
            {"gain", track.getLevelGain()}, {"muted", track.isMuted()}};
        j["tracks"].push_back(t);
    });
    return j.dump();
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
