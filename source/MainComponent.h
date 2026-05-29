#pragma once
#include <JuceHeader.h>
#include "SineWaveSound.h"
#include "SineWaveVoice.h"
#include "OscilloscopeComponent.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::Slider::Listener,
                      public juce::Button::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

private:
    void updateAllVoices();
    void updateAllFilters();
    void drawPanel(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);

    juce::Synthesiser synth;
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent {
        keyboardState,
        juce::MidiKeyboardComponent::horizontalKeyboard
    };

    // Oscilloscope
    OscilloscopeComponent oscilloscope;

    // ADSR
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label  attackLabel,  decayLabel,  sustainLabel,  releaseLabel;
    juce::ADSR::Parameters adsrParams { 0.1f, 0.1f, 0.8f, 0.5f };

    // Filter
    juce::Slider cutoffSlider, resonanceSlider;
    juce::Label  cutoffLabel,  resonanceLabel;
    juce::TextButton filterTypeButton { "Low-pass" };
    bool isLowPass  = true;
    float cutoffHz  = 2000.0f;
    float resonance = 1.0f;

    // Panel bounds stored for paint()
    juce::Rectangle<int> adsrPanel, filterPanel, scopePanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};