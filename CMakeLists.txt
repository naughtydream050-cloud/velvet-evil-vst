cmake_minimum_required(VERSION 3.22)
project(VelvetEvil VERSION 1.0.0)

# ── JUCE: ローカル優先 → FetchContent フォールバック ───────────────────────
set(JUCE_TAG "7.0.12")
if(EXISTS "C:/JUCE/CMakeLists.txt")
    message(STATUS "[JUCE] C:/JUCE (local)")
    add_subdirectory("C:/JUCE" JUCE)
elseif(EXISTS "C:/dev/JUCE/CMakeLists.txt")
    message(STATUS "[JUCE] C:/dev/JUCE (local)")
    add_subdirectory("C:/dev/JUCE" JUCE)
else()
    message(STATUS "[JUCE] FetchContent ${JUCE_TAG}")
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        ${JUCE_TAG}
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

# ── Plugin ──────────────────────────────────────────────────────────────────
juce_add_plugin(VelvetEvil
    COMPANY_NAME                "Naughty"
    PLUGIN_MANUFACTURER_CODE    "Nght"
    PLUGIN_CODE                 "VlEv"
    FORMATS                     VST3
    PRODUCT_NAME                "VELVET EVIL"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     FALSE
)

# ── Binary Access: 画像を BinaryData に埋め込む ─────────────────────────
juce_add_binary_data(VelvetEvilBinaryData
    SOURCES
        Resources/image_0.png
        Resources/image_2.png
)

juce_generate_juce_header(VelvetEvil)

target_sources(VelvetEvil PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/KnobLookAndFeel.cpp
    Source/KnobLookAndFeel.h
)
target_include_directories(VelvetEvil PRIVATE Source)

if(MSVC)
    target_compile_options(VelvetEvil PRIVATE /utf-8)
endif()

target_compile_definitions(VelvetEvil PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(VelvetEvil
    PRIVATE
        VelvetEvilBinaryData
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# ── ポストビルド: C:/Program Files/Common Files/VST3 へコピー ──────────────
if(WIN32)
    set(VST3_DEST "C:/Program Files/Common Files/VST3")
    add_custom_command(TARGET VelvetEvil_VST3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${VST3_DEST}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "$<TARGET_FILE_DIR:VelvetEvil_VST3>"
            "${VST3_DEST}/VELVET EVIL.vst3/Contents/x86_64-win"
        COMMENT "Installing VST3 to ${VST3_DEST}"
    )
endif()
