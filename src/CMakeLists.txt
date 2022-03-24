
juce_add_gui_app(Trackman
        PRODUCT_NAME Trackman
        MICROPHONE_PERMISSION_ENABLED TRUE
        CAMERA_PERMISSION_ENABLED TRUE
        BLUETOOTH_PERMISSION_ENABLED TRUE
        FILE_SHARING_ENABLED TRUE
        DOCUMENT_BROWSER_ENABLED TRUE
        DOCUMENT_EXTENSIONS .track
        BUNDLE_ID org.trackman
        )

target_sources(Trackman PRIVATE
        JuceHeader.h
        Main.cpp
        common/listutil.h
        common/midiutil.h
        audio/GainAudioSource.cpp
        audio/GainAudioSource.h
        audio/MeteredAudioSource.cpp
        audio/MeteredAudioSource.h
        model/MidiRecorder.cpp
        model/MidiRecorder.h
        audio/PositionableAudioSourceAdapter.h
        audio/PositionableMixingAudioSource.cpp
        audio/PositionableMixingAudioSource.h
        audio/PositionableResamplingAudioSource.cpp
        audio/PositionableResamplingAudioSource.h
        audio/ProcessingAudioSource.h
        model/SamplePlayer.cpp
        model/SamplePlayer.h
        audio/SynthAudioSource.cpp
        audio/SynthAudioSource.h
        model/Timeline.h
        model/TimeRange.h
        commands/Command.h
        commands/CommandList.cpp
        commands/CommandList.h
        commands/MixerCommands.h
        commands/TrackCommands.h
        commands/TrackListCommands.h
        model/Instrument.cpp
        model/Instrument.h
        model/MidiPlayer.cpp
        model/MidiPlayer.h
        model/Mixer.cpp
        model/Mixer.h
        model/NoteRoll.cpp
        model/NoteRoll.h
        model/Project.cpp
        model/Project.h
        model/Sample.cpp
        model/Sample.h
        model/TimeSignature.cpp
        model/TimeSignature.h
        model/Track.cpp
        model/Track.h
        model/TrackRegion.cpp
        model/TrackRegion.h
        model/TrackList.cpp
        model/TrackList.h
        model/Transport.cpp
        model/Transport.h
        ui/AudioPlayer.cpp
        ui/AudioPlayer.h
        ui/KeyboardControl.cpp
        ui/KeyboardControl.h
        ui/MainAudioComponent.cpp
        ui/MainAudioComponent.h
        ui/MainWindow.cpp
        ui/MainWindow.h
        ui/common/DecibelSlider.h
        ui/common/FileChooserControl.cpp
        ui/common/FileChooserControl.h
        ui/common/PositionOverlay.cpp
        ui/common/PositionOverlay.h
        ui/common/ThumbnailComponent.cpp
        ui/common/ThumbnailComponent.h
        ui/common/TransportControl.cpp
        ui/common/TransportControl.h
        ui/desktop/ChildWindow.cpp
        ui/desktop/ChildWindow.h
        ui/desktop/DesktopComponent.cpp
        ui/desktop/DesktopComponent.h
        ui/desktop/DesktopController.cpp
        ui/desktop/DesktopController.h
        ui/desktop/FileDragDropTarget.h
        ui/desktop/ScaleButtonPanel.cpp
        ui/desktop/ScaleButtonPanel.h
        ui/desktop/TimeMeter.cpp
        ui/desktop/TimeMeter.h
        ui/desktop/TrackScaleListener.h
        ui/desktop/TrackListListener.h
        ui/instruments/InstrumentControl.cpp
        ui/instruments/InstrumentControl.h
        ui/instruments/InstrumentController.cpp
        ui/instruments/InstrumentController.h
        ui/instruments/InstrumentsController.cpp
        ui/instruments/InstrumentsController.h
        ui/instruments/InstrumentsPanel.cpp
        ui/instruments/InstrumentsPanel.h
        ui/mixer/LevelMeterLookAndFeel.h
        ui/mixer/MasterLevelMeterLookAndFeel.h
        ui/mixer/MasterTrackControl.cpp
        ui/mixer/MasterTrackControl.h
        ui/mixer/MasterTrackListener.h
        ui/mixer/MixerPanel.cpp
        ui/mixer/MixerPanel.h
        ui/mixer/MixerController.cpp
        ui/mixer/MixerController.h
        ui/mixer/TrackControl.cpp
        ui/mixer/TrackControl.h
        ui/mixer/TrackController.cpp
        ui/mixer/TrackController.h
        ui/mixer/TrackControlListener.h
        ui/mixer/TrackPanel.cpp
        ui/mixer/TrackPanel.h
        ui/tracks/NoteCanvas.cpp
        ui/tracks/NoteCanvas.h
        ui/tracks/SampleListener.h
        ui/tracks/SampleThumbnail.cpp
        ui/tracks/SampleThumbnail.h
        ui/tracks/TrackLaneControl.cpp
        ui/tracks/TrackLaneControl.h
        ui/tracks/TrackLaneController.cpp
        ui/tracks/TrackLaneController.h
        ui/tracks/TrackListController.cpp
        ui/tracks/TrackListController.h
        ui/tracks/TrackListPanel.cpp
        ui/tracks/TrackListPanel.h
        ui/tracks/TrackListViewport.cpp
        ui/tracks/TrackListViewport.h
        ui/transport/TransportController.cpp
        ui/transport/TransportController.h
        ui/transport/TransportPanel.cpp
        ui/transport/TransportPanel.h
        )

target_include_directories(Trackman PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

if (APPLE)
    target_compile_definitions(Trackman PRIVATE
            JUCE_MAC=1
            )
    target_link_libraries(Trackman PRIVATE "-framework CoreAudioKit")
elseif (WIN32)
    set_target_properties(Trackman PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Trackman>"
            VS_DEBUGGER_COMMAND "$<TARGET_FILE:Trackman>"
            VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_PREFIX_PATH}/bin")
else ()
    install(TARGETS Trackman DESTINATION bin)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/resource DESTINATION share/Trackman)
    install(FILES ${CMAKE_SOURCE_DIR}/scripts/installer_linux/Trackman.desktop DESTINATION share/applications)
endif ()

target_link_libraries(Trackman PRIVATE
        nlohmann_json::nlohmann_json
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        # if JUCE is devendored, these libraries need to be linked against
        $<$<BOOL:${TRACKMAN_DEVENDORED_SYSTEM_JUCE}>:FLAC>
        $<$<BOOL:${TRACKMAN_DEVENDORED_SYSTEM_JUCE}>:ogg>
        $<$<BOOL:${TRACKMAN_DEVENDORED_SYSTEM_JUCE}>:vorbis>
        $<$<BOOL:${TRACKMAN_DEVENDORED_SYSTEM_JUCE}>:vorbisenc>
        $<$<BOOL:${TRACKMAN_DEVENDORED_SYSTEM_JUCE}>:vorbisfile>
        )
