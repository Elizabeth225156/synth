#pragma once
#include <JuceHeader.h>
#include "SineWaveSound.h"

struct SineWaveVoice : public juce::SynthesiserVoice
{
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int) override
    {
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        phase = 0.0;
        amplitude = velocity;
        adsr.noteOn();
    }

    void stopNote(float, bool allowTailOff) override
    {
        if (allowTailOff)
            adsr.noteOff();
        else
        {
            adsr.reset();
            clearCurrentNote();
            phase = 0.0;
        }
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override
    {
        if (!adsr.isActive())
        {
            clearCurrentNote();
            return;
        }

        const double sampleRate = getSampleRate();
        const double phaseIncrement = (2.0 * juce::MathConstants<double>::pi * frequency) / sampleRate;

        juce::AudioBuffer<float> buffer(outputBuffer.getNumChannels(), numSamples);
        buffer.clear();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float currentSample = (float)(std::sin(phase) * amplitude);

            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                buffer.addSample(channel, sample, currentSample);

            phase = std::fmod(phase + phaseIncrement,
                              2.0 * juce::MathConstants<double>::pi);
        }

        adsr.applyEnvelopeToBuffer(buffer, 0, numSamples);

        // Apply filter to each channel
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            filters[channel].processSamples(channelData, numSamples);
        }

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addFrom(channel, startSample, buffer, channel, 0, numSamples);

        if (!adsr.isActive())
            clearCurrentNote();
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void updateAdsrParameters(juce::ADSR::Parameters params)
    {
        adsr.setParameters(params);
    }

    // Called from MainComponent whenever a slider or button changes
    void updateFilter(float cutoffHz, float resonance, bool isLowPass)
    {
        for (auto& f : filters)
        {
            if (isLowPass)
                f.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), cutoffHz, resonance));
            else
                f.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), cutoffHz, resonance));
        }
    }

private:
    double frequency = 0.0;
    double phase     = 0.0;
    double amplitude = 0.0;

    juce::ADSR adsr;

    // One filter per channel (stereo = 2)
    juce::IIRFilter filters[2];
};