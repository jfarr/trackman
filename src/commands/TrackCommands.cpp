#include "commands/TrackCommands.h"

AddTrackCommand::AddTrackCommand(TrackListController &controller) : Command("add track"), controller(controller) {}

AddTrackCommand::~AddTrackCommand() {}

ChangeMasterVolumeCommand::ChangeMasterVolumeCommand(MixerComponent &mixer, float previousLevel)
    : Command("change master volume"), mixer(mixer), previousLevel(previousLevel) {}

ChangeMasterVolumeCommand::~ChangeMasterVolumeCommand() {}
