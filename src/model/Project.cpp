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
