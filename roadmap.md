# Roadmap

## Short-Term
- Undo change tempo / time signature
- Stretch / loop note rolls
- Factor out base classes for samples / note rolls
- Edit note rolls
- Undo edit note roll
- Fix bugs
  - Popping at begin/end of midi tracks
  - Missing note-offs when moving note rolls while playing
- Amplitude ASDR envelopes
- Stretch sample loops instead of resampling
- Stereo panner
- Hold zoom buttons zooms exponentially faster (possibly zoom slider instead)
- Song start / end position
- Loop start / end position
- Sample info popup
- UI enhancements
  - Rescale note roll
  - Drop sample disallowed graphic


## Long Term

- Use lambdas / ListenerList
- Use undo manager
- Select MIDI devices
- CPU meter (AudioDeviceManager::getCpuUsage)
- Midi CV
- Master track automation
  - Tempo change
  - Time signature change
- Metronome
- Export midi
- Time-shift samples (without pitch-shifting)
- Click+drag to select samples
- Drop multiple files
  - Default one track
  - Ctrl-drag multiple tracks
- Copy / paste
- Console commands
- Script window
- Unit tests
- GPL license
- Automation events
- Sequencer
- LFO
- Filter
- EQ
- VST Plugins
- Configure audio settings
  - Sample rate
  - Block size
  - Channels / bus layout
- Skins
