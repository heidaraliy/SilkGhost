/**
  ==============================================================================
    CustomLookAndFeel.h
    Created: 1 Dec 2024 3:11:00pm
    Author:  Heidar Aliy
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        auto arimoRegularTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::ArimoRegular_ttf, BinaryData::ArimoRegular_ttfSize);
        if (arimoRegularTypeface != nullptr)
        {
            arimoFont = juce::FontOptions(arimoRegularTypeface).withHeight(14.0f);
        }
        
        // load the on/off icons.
        onIcon = juce::ImageCache::getFromMemory(BinaryData::IconOn_png, BinaryData::IconOn_pngSize);
        offIcon = juce::ImageCache::getFromMemory(BinaryData::IconOff_png, BinaryData::IconOff_pngSize);
    }

    ~CustomLookAndFeel() override {}

    // override the drawToggleButton method to customize the ToggleButton's appearance.
    void drawToggleButton(juce::Graphics& g,
                          juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override
    {

        // draw the appropriate icon based on toggle state.
        juce::Image currentIcon = button.getToggleState() ? onIcon : offIcon;

        if (currentIcon.isValid())
        {
            const float fixedIconWidth = 36.0f;  // Desired icon width
            const float fixedIconHeight = 36.0f; // Desired icon height

            // calculate position to align the icon relative to the button's top-left corner.
            float iconX = 2.0f;
            float iconY = button.getHeight() - fixedIconHeight - 2.0f;

            // draw the icon with fixed size.
            g.drawImage(currentIcon,
                        iconX,
                        iconY,
                        fixedIconWidth,
                        fixedIconHeight,
                        0,
                        0,
                        static_cast<int>(currentIcon.getWidth()),
                        static_cast<int>(currentIcon.getHeight()));
        }
    }

private:
    juce::FontOptions arimoFont; // member variable to store the custom font.

    // icons for toggle states.
    juce::Image onIcon;
    juce::Image offIcon;
};
