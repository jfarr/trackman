#include "commands/TrackCommands.h"

AddTrackCommand::AddTrackCommand(DesktopController &controller, Track *track)
    : Command("add track"), controller(controller), track(track) {}

AddTrackCommand::~AddTrackCommand() {}

ChangeMasterVolumeCommand::ChangeMasterVolumeCommand(MixerPanel &mixerPanel, float previousLevel)
    : Command("change master volume"), mixerPanel(mixerPanel), previousLevel(previousLevel) {}

ChangeMasterVolumeCommand::~ChangeMasterVolumeCommand() {}
