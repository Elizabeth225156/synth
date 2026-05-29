cmake_minimum_required(VERSION 3.22)
project(synth VERSION 0.1.0)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
)

FetchContent_MakeAvailable(JUCE)

juce_add_gui_app(synth
    PRODUCT_NAME "Synth")

target_sources(synth PRIVATE
    source/main.cpp
    source/MainComponent.cpp
    source/SineWaveVoice.h
    source/SineWaveSound.h
    source/OscilloscopeComponent.h)

juce_generate_juce_header(synth)

target_compile_definitions(synth PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)

target_link_libraries(synth PRIVATE
    juce::juce_core
    juce::juce_gui_basics
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)