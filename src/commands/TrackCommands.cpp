#include "commands/TrackCommands.h"

AddTrackCommand::AddTrackCommand(TrackListController &controller) : Command("add track"), controller(controller) {}

AddTrackCommand::~AddTrackCommand() {}
