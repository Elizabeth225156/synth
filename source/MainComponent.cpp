#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(600, 400);
}

MainComponent::~MainComponent() {}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(
        juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(20.0f));
    g.drawText("Hello JUCE!", getLocalBounds(),
               juce::Justification::centred, true);
}

void MainComponent::resized() {}
