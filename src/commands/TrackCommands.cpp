#include "commands/TrackCommands.h"

AddTrackCommand::AddTrackCommand(DesktopController &controller, Track *track)
    : Command("add track"), controller(controller), track(track) {}

AddTrackCommand::~AddTrackCommand() {}

ChangeMasterVolumeCommand::ChangeMasterVolumeCommand(MixerPanel &mixer, float previousLevel)
    : Command("change master volume"), mixer(mixer), previousLevel(previousLevel) {}

ChangeMasterVolumeCommand::~ChangeMasterVolumeCommand() {}
