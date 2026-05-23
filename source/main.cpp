#include <JuceHeader.h>
#include "MainComponent.h"

class MyAudioApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override
        { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override
        { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow(
            getApplicationName()));
    }

    void shutdown() override { mainWindow = nullptr; }

    struct MainWindow : public juce::DocumentWindow
    {
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Desktop::getInstance()
                    .getDefaultLookAndFeel()
                    .findColour(
                        juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true);
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->
                systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(MyAudioApp)
