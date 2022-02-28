
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
        audio/PositionableMixingAudioSource.cpp
        audio/PositionableMixingAudioSource.h
        audio/PositionableResamplingAudioSource.cpp
        audio/PositionableResamplingAudioSource.h
        audio/ProcessingAudioSource.h
        controls/AudioPlayer.cpp
        controls/AudioPlayer.h
        controls/common/DecibelSlider.h
        controls/common/FileChooserControl.cpp
        controls/common/FileChooserControl.h
        controls/common/ThumbnailComponent.cpp
        controls/common/ThumbnailComponent.h
        controls/common/TransportControl.cpp
        controls/common/TransportControl.h
        controls/desktop/ChildWindow.cpp
        controls/desktop/ChildWindow.h
        controls/desktop/DesktopComponent.cpp
        controls/desktop/DesktopComponent.h
        controls/mixer/MasterTrackControl.cpp
        controls/mixer/MasterTrackControl.h
        controls/mixer/MixerComponent.cpp
        controls/mixer/MixerComponent.h
        controls/mixer/TrackControl.cpp
        controls/mixer/TrackControl.h
        controls/mixer/TrackLaneControl.cpp
        controls/mixer/TrackLaneControl.h
        model/Track.cpp
        model/Track.h
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
