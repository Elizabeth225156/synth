#include <JuceHeader.h>
#include "MainComponent.h"

class MyAudioApp : public juce::JUCEApplication //create the child class
{
public:
    //redefining functions
    const juce::String getApplicationName() override{
        return ProjectInfo::projectName;
    }
    const juce::String getApplicationVersion() override{
        return ProjectInfo::versionString;
    }
    bool moreThanOneInstanceAllowed() override {
        return true;
    }

    //Starts the gui
    void initialise(const juce::String&) override {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    //Shuts down the gui
    void shutdown() override {
        mainWindow = nullptr;
    }

    //actual stuff in the window
    struct MainWindow : public juce::DocumentWindow
    {
        //creates the main window
        MainWindow(juce::String name): 
                DocumentWindow(name, //title of the window
                juce::Desktop::getInstance() //fetches global application
                .getDefaultLookAndFeel() //fetches active visual theme
                .findColour(juce::ResizableWindow::backgroundColourId),
                DocumentWindow::allButtons){ //basic window control buttons
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true); //puts the stuff in mainComponent on the screen
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(MyAudioApp)
