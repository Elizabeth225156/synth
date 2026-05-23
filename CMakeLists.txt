cmake_minimum_required(VERSION 3.22)
project(synth VERSION 0.1.0)

add_subdirectory(C:/dev/JUCE JUCE)

juce_add_gui_app(synth
    PRODUCT_NAME "Synth")

target_sources(synth PRIVATE
    source/main.cpp
    source/MainComponent.cpp)

juce_generate_juce_header(synth)

target_compile_definitions(synth PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)

target_link_libraries(synth PRIVATE
    juce::juce_core
    juce::juce_gui_basics
    juce::juce_audio_utils
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)