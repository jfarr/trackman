#pragma once

#include <JuceHeader.h>

struct BurgerMenuHeader : public juce::Component
{
    BurgerMenuHeader(juce::SidePanel& sp)
        : sidePanel(sp)
    {
        static const unsigned char burgerMenuPathData[]
            = { 110,109,0,0,128,64,0,0,32,65,108,0,0,224,65,0,0,32,65,98,254,212,232,65,0,0,32,65,0,0,240,65,252,
                169,17,65,0,0,240,65,0,0,0,65,98,0,0,240,65,8,172,220,64,254,212,232,65,0,0,192,64,0,0,224,65,0,0,
                192,64,108,0,0,128,64,0,0,192,64,98,16,88,57,64,0,0,192,64,0,0,0,64,8,172,220,64,0,0,0,64,0,0,0,65,
                98,0,0,0,64,252,169,17,65,16,88,57,64,0,0,32,65,0,0,128,64,0,0,32,65,99,109,0,0,224,65,0,0,96,65,108,
                0,0,128,64,0,0,96,65,98,16,88,57,64,0,0,96,65,0,0,0,64,4,86,110,65,0,0,0,64,0,0,128,65,98,0,0,0,64,
                254,212,136,65,16,88,57,64,0,0,144,65,0,0,128,64,0,0,144,65,108,0,0,224,65,0,0,144,65,98,254,212,232,
                65,0,0,144,65,0,0,240,65,254,212,136,65,0,0,240,65,0,0,128,65,98,0,0,240,65,4,86,110,65,254,212,232,
                65,0,0,96,65,0,0,224,65,0,0,96,65,99,109,0,0,224,65,0,0,176,65,108,0,0,128,64,0,0,176,65,98,16,88,57,
                64,0,0,176,65,0,0,0,64,2,43,183,65,0,0,0,64,0,0,192,65,98,0,0,0,64,254,212,200,65,16,88,57,64,0,0,208,
                65,0,0,128,64,0,0,208,65,108,0,0,224,65,0,0,208,65,98,254,212,232,65,0,0,208,65,0,0,240,65,254,212,
                200,65,0,0,240,65,0,0,192,65,98,0,0,240,65,2,43,183,65,254,212,232,65,0,0,176,65,0,0,224,65,0,0,176,
                65,99,101,0,0 };

        juce::Path p;
        p.loadPathFromData(burgerMenuPathData, sizeof(burgerMenuPathData));
        burgerButton.setShape(p, true, true, false);

        burgerButton.onClick = [this] { showOrHide(); };
        addAndMakeVisible(burgerButton);
    }

    ~BurgerMenuHeader() override
    {
        sidePanel.showOrHide(false);
    }

private:
    void paint(juce::Graphics& g) override
    {
        auto titleBarBackgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId)
            .darker();

        g.setColour(titleBarBackgroundColour);
        g.fillRect(getLocalBounds());
    }

    void resized() override
    {
        auto r = getLocalBounds();

        burgerButton.setBounds(r.removeFromRight(40).withSizeKeepingCentre(20, 20));

        titleLabel.setFont(juce::Font((float)getHeight() * 0.5f, juce::Font::plain));
        titleLabel.setBounds(r);
    }

    void showOrHide()
    {
        sidePanel.showOrHide(!sidePanel.isPanelShowing());
    }

    juce::SidePanel& sidePanel;

    juce::Label titleLabel{ "titleLabel", "JUCE Demo" };
    juce::ShapeButton burgerButton{ "burgerButton", juce::Colours::lightgrey, juce::Colours::lightgrey, juce::Colours::white };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BurgerMenuHeader)
};


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, public juce::ChangeListener, public juce::Timer,
    public juce::ApplicationCommandTarget, public juce::MenuBarModel
{
public:
    //==============================================================================
    /** A list of the commands that this demo responds to. */
    enum CommandIDs
    {
        menuPositionInsideWindow = 1,
        menuPositionGlobalMenuBar,
        menuPositionBurgerMenu,
        outerColourRed,
        outerColourGreen,
        outerColourBlue,
        innerColourRed,
        innerColourGreen,
        innerColourBlue
    };

    //==============================================================================
    /** Represents the possible menu positions. */
    enum class MenuBarPosition
    {
        window,
        global,
        burger
    };

    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // Component
    void paint (juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    //==============================================================================
   // Timer
    void MainComponent::timerCallback() override;

    //==============================================================================

    juce::StringArray getMenuBarNames() override
    {
        return { "Menu Position", "Outer Colour", "Inner Colour" };
    }

    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& /*menuName*/) override
    {
        juce::PopupMenu menu;

        if (menuIndex == 0)
        {
            menu.addCommandItem(&commandManager, CommandIDs::menuPositionInsideWindow);
#if JUCE_MAC
            menu.addCommandItem(&commandManager, CommandIDs::menuPositionGlobalMenuBar);
#endif
            menu.addCommandItem(&commandManager, CommandIDs::menuPositionBurgerMenu);
        }
        else if (menuIndex == 1)
        {
            menu.addCommandItem(&commandManager, CommandIDs::outerColourRed);
            menu.addCommandItem(&commandManager, CommandIDs::outerColourGreen);
            menu.addCommandItem(&commandManager, CommandIDs::outerColourBlue);
        }
        else if (menuIndex == 2)
        {
            menu.addCommandItem(&commandManager, CommandIDs::innerColourRed);
            menu.addCommandItem(&commandManager, CommandIDs::innerColourGreen);
            menu.addCommandItem(&commandManager, CommandIDs::innerColourBlue);
        }

        return menu;
    }

    void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {}

    //==============================================================================
    // The following methods implement the ApplicationCommandTarget interface, allowing
    // this window to publish a set of actions it can perform, and which can be mapped
    // onto menus, keypresses, etc.

    ApplicationCommandTarget* getNextCommandTarget() override
    {
        return &outerCommandTarget;
    }

    void getAllCommands(juce::Array<juce::CommandID>& c) override
    {
        juce::Array<juce::CommandID> commands{ CommandIDs::menuPositionInsideWindow,
                                    CommandIDs::menuPositionGlobalMenuBar,
                                    CommandIDs::menuPositionBurgerMenu };
        c.addArray(commands);
    }
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override
    {
        switch (commandID)
        {
        case CommandIDs::menuPositionInsideWindow:
            result.setInfo("Inside Window", "Places the menu bar inside the application window", "Menu", 0);
            result.setTicked(menuBarPosition == MenuBarPosition::window);
            result.addDefaultKeypress('w', juce::ModifierKeys::shiftModifier);
            break;
        case CommandIDs::menuPositionGlobalMenuBar:
            result.setInfo("Global", "Uses a global menu bar", "Menu", 0);
            result.setTicked(menuBarPosition == MenuBarPosition::global);
            result.addDefaultKeypress('g', juce::ModifierKeys::shiftModifier);
            break;
        case CommandIDs::menuPositionBurgerMenu:
            result.setInfo("Burger Menu", "Uses a burger menu", "Menu", 0);
            result.setTicked(menuBarPosition == MenuBarPosition::burger);
            result.addDefaultKeypress('b', juce::ModifierKeys::shiftModifier);
            break;
        default:
            break;
        }
    }
    bool perform(const InvocationInfo& info) override
    {
        switch (info.commandID)
        {
        case CommandIDs::menuPositionInsideWindow:
            setMenuBarPosition(MenuBarPosition::window);
            break;
        case CommandIDs::menuPositionGlobalMenuBar:
            setMenuBarPosition(MenuBarPosition::global);
            break;
        case CommandIDs::menuPositionBurgerMenu:
            setMenuBarPosition(MenuBarPosition::burger);
            break;
        default:
            return false;
        }

        return true;
    }

    void setMenuBarPosition(MenuBarPosition newPosition)
    {
        if (menuBarPosition != newPosition)
        {
            menuBarPosition = newPosition;

            if (menuBarPosition != MenuBarPosition::burger)
                sidePanel.showOrHide(false);

#if JUCE_MAC
            MenuBarModel::setMacMainMenu(menuBarPosition == MenuBarPosition::global ? this : nullptr);
#endif

            menuBar->setVisible(menuBarPosition == MenuBarPosition::window);
            burgerMenu.setModel(menuBarPosition == MenuBarPosition::burger ? this : nullptr);
            menuHeader.setVisible(menuBarPosition == MenuBarPosition::burger);

            sidePanel.setContent(menuBarPosition == MenuBarPosition::burger ? &burgerMenu : nullptr, false);
            menuItemsChanged();

            resized();
        }
    }

private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };

    TransportState state;
    juce::AudioTransportSource transportSource;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    void changeState(TransportState newState);
    void updateLoopState(bool shouldLoop);

    juce::ApplicationCommandManager commandManager;

    std::unique_ptr<juce::MenuBarComponent> menuBar;
    MenuBarPosition menuBarPosition = MenuBarPosition::window;

    juce::SidePanel sidePanel{ "Menu", 300, false };

    juce::BurgerMenuComponent burgerMenu;
    BurgerMenuHeader menuHeader{ sidePanel };


    //==============================================================================
    /**
        Command messages that aren't handled in the main component will be passed
        to this class to respond to.
    */
    class OuterCommandTarget : public Component,
        public ApplicationCommandTarget
    {
    public:
        OuterCommandTarget(juce::ApplicationCommandManager& m)
            : commandManager(m),
            innerCommandTarget(commandManager)
        {
            commandManager.registerAllCommandsForTarget(this);

            addAndMakeVisible(innerCommandTarget);
        }

        void resized() override
        {
            innerCommandTarget.setBounds(getLocalBounds().reduced(50));
        }

        void paint(juce::Graphics& g) override
        {
            g.fillAll(currentColour);
        }

        //==============================================================================
        ApplicationCommandTarget* getNextCommandTarget() override
        {
            return &innerCommandTarget;
        }

        void getAllCommands(juce::Array<juce::CommandID>& c) override
        {
            juce::Array<juce::CommandID> commands{ CommandIDs::outerColourRed,
                                        CommandIDs::outerColourGreen,
                                        CommandIDs::outerColourBlue };

            c.addArray(commands);
        }

        void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override
        {
            switch (commandID)
            {
            case CommandIDs::outerColourRed:
                result.setInfo("Red", "Sets the outer colour to red", "Outer", 0);
                result.setTicked(currentColour == juce::Colours::red);
                result.addDefaultKeypress('r', juce::ModifierKeys::commandModifier);
                break;
            case CommandIDs::outerColourGreen:
                result.setInfo("Green", "Sets the outer colour to green", "Outer", 0);
                result.setTicked(currentColour == juce::Colours::green);
                result.addDefaultKeypress('g', juce::ModifierKeys::commandModifier);
                break;
            case CommandIDs::outerColourBlue:
                result.setInfo("Blue", "Sets the outer colour to blue", "Outer", 0);
                result.setTicked(currentColour == juce::Colours::blue);
                result.addDefaultKeypress('b', juce::ModifierKeys::commandModifier);
                break;
            default:
                break;
            }
        }

        bool perform(const InvocationInfo& info) override
        {
            switch (info.commandID)
            {
            case CommandIDs::outerColourRed:
                currentColour = juce::Colours::red;
                break;
            case CommandIDs::outerColourGreen:
                currentColour = juce::Colours::green;
                break;
            case CommandIDs::outerColourBlue:
                currentColour = juce::Colours::blue;
                break;
            default:
                return false;
            }

            repaint();
            return true;
        }

    private:
        //==============================================================================
        /**
            Command messages that aren't handled in the OuterCommandTarget will be passed
            to this class to respond to.
        */
        struct InnerCommandTarget : public Component,
            public ApplicationCommandTarget
        {
            InnerCommandTarget(juce::ApplicationCommandManager& m)
                : commandManager(m)
            {
                commandManager.registerAllCommandsForTarget(this);
            }

            void paint(juce::Graphics& g) override
            {
                g.fillAll(currentColour);
            }

            //==============================================================================
            ApplicationCommandTarget* getNextCommandTarget() override
            {
                // this will return the next parent component that is an ApplicationCommandTarget
                return findFirstTargetParentComponent();
            }

            void getAllCommands(juce::Array<juce::CommandID>& c) override
            {
                juce::Array<juce::CommandID> commands{ CommandIDs::innerColourRed,
                                            CommandIDs::innerColourGreen,
                                            CommandIDs::innerColourBlue };

                c.addArray(commands);
            }

            void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override
            {
                switch (commandID)
                {
                case CommandIDs::innerColourRed:
                    result.setInfo("Red", "Sets the inner colour to red", "Inner", 0);
                    result.setTicked(currentColour == juce::Colours::red);
                    result.addDefaultKeypress('r', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
                    break;
                case CommandIDs::innerColourGreen:
                    result.setInfo("Green", "Sets the inner colour to green", "Inner", 0);
                    result.setTicked(currentColour == juce::Colours::green);
                    result.addDefaultKeypress('g', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
                    break;
                case CommandIDs::innerColourBlue:
                    result.setInfo("Blue", "Sets the inner colour to blue", "Inner", 0);
                    result.setTicked(currentColour == juce::Colours::blue);
                    result.addDefaultKeypress('b', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
                    break;
                default:
                    break;
                }
            }

            bool perform(const InvocationInfo& info) override
            {
                switch (info.commandID)
                {
                case CommandIDs::innerColourRed:
                    currentColour = juce::Colours::red;
                    break;
                case CommandIDs::innerColourGreen:
                    currentColour = juce::Colours::green;
                    break;
                case CommandIDs::innerColourBlue:
                    currentColour = juce::Colours::blue;
                    break;
                default:
                    return false;
                }

                repaint();
                return true;
            }

            juce::ApplicationCommandManager& commandManager;

            juce::Colour currentColour{ juce::Colours::blue };
        };

        juce::ApplicationCommandManager& commandManager;
        InnerCommandTarget innerCommandTarget;

        juce::Colour currentColour{ juce::Colours::red };
    };

    OuterCommandTarget outerCommandTarget{ commandManager };

    //==============================================================================

    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void loopButtonChanged();

    std::unique_ptr<juce::FileChooser> chooser;

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::ToggleButton loopingToggle;
    juce::Label currentPositionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
