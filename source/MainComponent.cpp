#include "MainComponent.h"

//creating object MainComponent
MainComponent::MainComponent()
{
    setSize(800, 600);
    addAndMakeVisible(keyboardComponent); //on screen keyboard
    addAndMakeVisible(oscilloscope); //on screen oscilloscope
    setAudioChannels(0, 2);

    for (int i = 0; i < 8; ++i){
        synth.addVoice(new SineWaveVoice()); //how many notes at a time
    }

    synth.addSound(new SineWaveSound()); //actual sound creations

    //sets up the slider with a lambda functions
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label,
                               const juce::String& labelText,
                               double min, double max, double initial) {
        slider.setRange(min, max);
        slider.setValue(initial);
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff82b4ff));
        slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff4a6fa5));
        slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xff2a2a3a));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        slider.addListener(this);
        addAndMakeVisible(slider);

        label.setText(labelText, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0xffaaaacc));
        label.setFont(juce::FontOptions(12.0f));
        addAndMakeVisible(label);
    };

    //set up each slider
    setupSlider(attackSlider, attackLabel, "Attack", 0.001, 5.0, adsrParams.attack);
    setupSlider(decaySlider, decayLabel, "Decay", 0.001, 5.0, adsrParams.decay);
    setupSlider(sustainSlider, sustainLabel, "Sustain", 0.0, 1.0, adsrParams.sustain);
    setupSlider(releaseSlider, releaseLabel, "Release", 0.001, 5.0, adsrParams.release);
    setupSlider(cutoffSlider, cutoffLabel, "Cutoff", 20.0,  20000.0, cutoffHz);
    setupSlider(resonanceSlider, resonanceLabel, "Resonance", 0.1, 5.0, resonance);

    cutoffSlider.setSkewFactorFromMidPoint(1000.0);

    filterTypeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff4a6fa5));
    filterTypeButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff82b4ff));
    filterTypeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    filterTypeButton.addListener(this);
    addAndMakeVisible(filterTypeButton);
}

MainComponent::~MainComponent(){
    shutdownAudio();
}

void MainComponent::drawPanel(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title){
    g.setColour(juce::Colour(0xff1e1e2e));
    g.fillRoundedRectangle(bounds.toFloat(), 8.0f);

    g.setColour(juce::Colour(0xff4a6fa5));
    g.drawRoundedRectangle(bounds.toFloat(), 8.0f, 1.5f);

    g.setColour(juce::Colour(0xff82b4ff));
    g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    g.drawText(title, bounds.removeFromTop(24).reduced(8, 0), juce::Justification::centredLeft);
}

void MainComponent::paint(juce::Graphics& g){
    juce::ColourGradient gradient(juce::Colour(0xff12121f), 0.0f, 0.0f,
                                  juce::Colour(0xff1a1a2e), 0.0f, (float)getHeight(),
                                  false);
    g.setGradientFill(gradient);
    g.fillAll();

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(18.0f).withStyle("Bold"));
    g.drawText("Sine Wave Synth", juce::Rectangle<int>(0, 0, getWidth(), 36), juce::Justification::centred);

    drawPanel(g, adsrPanel, "ADSR Envelope");
    drawPanel(g, filterPanel, "Filter");
    drawPanel(g, scopePanel, "Oscilloscope");
}

void MainComponent::resized(){
    const int padding = 10;
    const int keyboardHeight = 80;
    const int titleHeight = 36;
    const int buttonHeight = 30;
    const int labelHeight = 18;
    const int textBoxHeight = 20;
    const int scopeHeight = 120;

    const int halfWidth = getWidth() / 2;

    //set oscilloscope panel
    scopePanel = juce::Rectangle<int>(padding, titleHeight + padding, getWidth() - padding * 2, scopeHeight);
    oscilloscope.setBounds(scopePanel.reduced(4));

    //set asdr and filter panels
    const int panelY = scopePanel.getBottom() + padding;
    const int panelBottom = getHeight() - keyboardHeight - padding;
    const int panelHeight = panelBottom - panelY;

    adsrPanel = juce::Rectangle<int>(padding, panelY, halfWidth - padding * 2, panelHeight);
    filterPanel = juce::Rectangle<int>(halfWidth + padding, panelY, halfWidth - padding * 2, panelHeight);

    //ASDR sliders
    const int adsrInnerX = adsrPanel.getX() + padding;
    const int adsrInnerY = adsrPanel.getY() + 28;
    const int adsrInnerW = adsrPanel.getWidth()  - padding * 2;
    const int adsrInnerH = adsrPanel.getHeight() - 28 - padding;
    const int adsrSliderW = adsrInnerW / 4;
    const int adsrSliderH = adsrInnerH - labelHeight - textBoxHeight;

    juce::Slider* adsrSliders[] = { &attackSlider, &decaySlider, &sustainSlider, &releaseSlider };
    juce::Label*  adsrLabels[]  = { &attackLabel,  &decayLabel,  &sustainLabel,  &releaseLabel  };

    //set the sliders limits
    for (int i = 0; i < 4; ++i){
        adsrLabels[i] -> setBounds(adsrInnerX + adsrSliderW * i, adsrInnerY, adsrSliderW, labelHeight);
        adsrSliders[i]-> setBounds(adsrInnerX + adsrSliderW * i, adsrInnerY + labelHeight, adsrSliderW, adsrSliderH);
    }

    //filter sliders
    const int filterInnerX = filterPanel.getX() + padding;
    const int filterInnerY = filterPanel.getY() + 28;
    const int filterInnerW = filterPanel.getWidth()  - padding * 2;
    const int filterInnerH = filterPanel.getHeight() - 28 - padding;
    const int filterSliderW = filterInnerW / 2;
    const int filterSliderH = filterInnerH - labelHeight - textBoxHeight - buttonHeight - padding;

    cutoffLabel.setBounds(filterInnerX, filterInnerY, filterSliderW, labelHeight);
    cutoffSlider.setBounds(filterInnerX, filterInnerY + labelHeight, filterSliderW, filterSliderH);
    resonanceLabel.setBounds(filterInnerX + filterSliderW, filterInnerY, filterSliderW, labelHeight);
    resonanceSlider.setBounds(filterInnerX + filterSliderW, filterInnerY + labelHeight, filterSliderW, filterSliderH);

    filterTypeButton.setBounds(filterInnerX, filterInnerY + labelHeight + filterSliderH + padding, filterInnerW, buttonHeight);

    keyboardComponent.setBounds(0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);
}

void MainComponent::sliderValueChanged(juce::Slider* slider){
    //changing the slider values
    if (slider == &attackSlider){
        adsrParams.attack  = (float)attackSlider.getValue();
    }else if (slider == &decaySlider){
        adsrParams.decay   = (float)decaySlider.getValue();
    }else if (slider == &sustainSlider){
        adsrParams.sustain = (float)sustainSlider.getValue();
    }else if (slider == &releaseSlider){
        adsrParams.release = (float)releaseSlider.getValue();
    }else if (slider == &cutoffSlider){
        cutoffHz = (float)cutoffSlider.getValue();
    }else if (slider == &resonanceSlider){
        resonance = (float)resonanceSlider.getValue();
    }

    //update sound based on sliders
    updateAllVoices();
    updateAllFilters();
}

void MainComponent::buttonClicked(juce::Button* button){
    if (button == &filterTypeButton){
        isLowPass = !isLowPass;
        filterTypeButton.setButtonText(isLowPass ? "Low-pass" : "High-pass");
        updateAllFilters();
    }
}

void MainComponent::updateAllVoices(){
    for (int i = 0; i < synth.getNumVoices(); ++i){
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synth.getVoice(i))){
            voice->updateAdsrParameters(adsrParams);
        }
    }
}

void MainComponent::updateAllFilters(){
    for (int i = 0; i < synth.getNumVoices(); ++i){
        if (auto* voice = dynamic_cast<SineWaveVoice*>(synth.getVoice(i))){
            voice->updateFilter(cutoffHz, resonance, isLowPass);
        }
    }
}

void MainComponent::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate){
    synth.setCurrentPlaybackSampleRate(sampleRate);
    updateAllVoices();
    updateAllFilters();
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill){
    bufferToFill.clearActiveBufferRegion(); //clear the buffer

    juce::MidiBuffer incomingMidi;
    keyboardState.processNextMidiBuffer(incomingMidi, 0, bufferToFill.numSamples, true); //process the next buffer

    //push it to the synth
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);

    //push it to the oscilliscope
    oscilloscope.pushBuffer(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
}

void MainComponent::releaseResources() {}