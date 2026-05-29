#pragma once
#include <JuceHeader.h>

class OscilloscopeComponent : public juce::Component,
                               public juce::Timer
{
public:
    OscilloscopeComponent()
    {
        startTimerHz(30); // refresh 30 times per second
    }

    ~OscilloscopeComponent() override
    {
        stopTimer();
    }

    // Called from the audio thread — writes samples into the ring buffer
    void pushBuffer(const juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
    {
        const float* channelData = buffer.getReadPointer(0, startSample);

        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite(numSamples, start1, size1, start2, size2);

        if (size1 > 0)
            fifoBuffer.copyFrom(0, start1, channelData, size1);
        if (size2 > 0)
            fifoBuffer.copyFrom(0, start2, channelData + size1, size2);

        abstractFifo.finishedWrite(size1 + size2);
    }

    void timerCallback() override
    {
        // Pull available samples from ring buffer into display buffer
        int available = abstractFifo.getNumReady();

        int start1, size1, start2, size2;
        abstractFifo.prepareToRead(available, start1, size1, start2, size2);

        if (size1 > 0)
            displayBuffer.copyFrom(0, start1, fifoBuffer, 0, start1, size1);
        if (size2 > 0)
            displayBuffer.copyFrom(0, start2, fifoBuffer, 0, start2, size2);

        abstractFifo.finishedRead(size1 + size2);

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(4.0f);

        // Background
        g.setColour(juce::Colour(0xff12121f));
        g.fillRoundedRectangle(bounds, 6.0f);

        // Border
        g.setColour(juce::Colour(0xff4a6fa5));
        g.drawRoundedRectangle(bounds, 6.0f, 1.5f);

        // Centre line
        const float centreY = bounds.getCentreY();
        g.setColour(juce::Colour(0xff2a2a4a));
        g.drawHorizontalLine((int)centreY, bounds.getX(), bounds.getRight());

        // Draw waveform
        const int numSamples = displayBuffer.getNumSamples();
        if (numSamples < 2)
            return;

        const float* data = displayBuffer.getReadPointer(0);
        const float width  = bounds.getWidth();
        const float height = bounds.getHeight();

        juce::Path wavePath;
        wavePath.startNewSubPath(bounds.getX(),
                                 centreY - data[0] * (height * 0.45f));

        for (int i = 1; i < numSamples; ++i)
        {
            const float x = bounds.getX() + (float)i / (float)numSamples * width;
            const float y = centreY - data[i] * (height * 0.45f);
            wavePath.lineTo(x, y);
        }

        g.setColour(juce::Colour(0xff82b4ff));
        g.strokePath(wavePath, juce::PathStrokeType(1.5f));
    }

    void resized() override
    {
        // Resize internal buffers to match display width in samples
        const int bufferSize = 2048;
        fifoBuffer   .setSize(1, bufferSize);
        displayBuffer.setSize(1, bufferSize);
        fifoBuffer   .clear();
        displayBuffer.clear();
        abstractFifo .reset();
    }

private:
    static constexpr int fifoSize = 2048;

    juce::AbstractFifo    abstractFifo  { fifoSize };
    juce::AudioBuffer<float> fifoBuffer    { 1, fifoSize };
    juce::AudioBuffer<float> displayBuffer { 1, fifoSize };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscilloscopeComponent)
};