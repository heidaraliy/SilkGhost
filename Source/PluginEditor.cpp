#include "PluginEditor.h"

SilkGhostAudioProcessorEditor::SilkGhostAudioProcessorEditor (SilkGhostAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // declare type faces -- we'll declare them globally so we can access them across the entire file.
    vidalokaTypeFace = juce::Typeface::createSystemTypefaceFor(BinaryData::VidalokaRegular_ttf, BinaryData::VidalokaRegular_ttfSize);
    jassert(vidalokaTypeFace != nullptr);

    arimoRegularTypeFace = juce::Typeface::createSystemTypefaceFor(BinaryData::ArimoRegular_ttf, BinaryData::ArimoRegular_ttfSize);
    jassert(arimoRegularTypeFace != nullptr);

    juce::FontOptions vidalokaFontOptions;
    vidalokaFontOptions = vidalokaFontOptions.withTypeface(vidalokaTypeFace).withHeight(20.0f);
    vidaloka = juce::FontOptions(vidalokaFontOptions);

    juce::FontOptions arimoFontOptions;
    arimoFontOptions = arimoFontOptions.withTypeface(arimoRegularTypeFace).withHeight(14.0f);
    arimo = juce::FontOptions(arimoFontOptions);
    
    // set the logo.
    logoImage = juce::ImageCache::getFromMemory(BinaryData::SilkGhost_png, BinaryData::SilkGhost_pngSize);

    // set the dimensions of the plugin window statically - easier to manage!
    setSize(640, 370);

    /**
     
     begin initialization of GUI elements below. we can declare them, set styles, and attach them to their corresponding values from the AudioProcessor (if required, not all elements will be parameters).
     
     the process is pretty straightforward:
     1. set the slider and text box style, then make it visible.
     2. set the label set and attach it to the component.
     3. attach the *value* of the parameter to audioProcessor (so that it can do its job).
     
     we can repeat this process continually for each element defined, with slight adjustments happening based on the type of interface being used (like a slider, toggle button, chart, title, etc.).

     */
    
    // set the logo.
    logoComponent.setImage(logoImage, juce::RectanglePlacement::stretchToFit);
    addAndMakeVisible(logoComponent);
    
    // setting the preset dropdown.
    for (int i = 0; i < audioProcessor.presets.size(); ++i)
    {
        presetsComboBox.addItem(audioProcessor.presets[i].name, i + 1);
    }
    addAndMakeVisible(presetsComboBox);
    
    presetsLabel.setText("Presets", juce::dontSendNotification);
    presetsLabel.setFont(arimo);
    presetsLabel.attachToComponent(&presetsComboBox, false);
    addAndMakeVisible(presetsLabel);
    
    presetsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "presetSelection", presetsComboBox);
    
    // setting the signal quality dropdown.
    qualityModeComboBox.addItem("High", 1);
    qualityModeComboBox.addItem("Medium", 2);
    qualityModeComboBox.addItem("Low", 3);
    qualityModeComboBox.addItem("Garbage", 4);
    addAndMakeVisible(qualityModeComboBox);
    qualityModeLabel.setText("Signal Quality", juce::dontSendNotification);
    qualityModeLabel.setFont(arimo);
    qualityModeLabel.attachToComponent(&qualityModeComboBox, false);
    addAndMakeVisible(qualityModeLabel);

    qualityModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getParameters(), "qualityMode", qualityModeComboBox);
    
    // setting the reverb reversal button.
    reverseReverbButton.setButtonText("");
    reverseReverbButton.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(reverseReverbButton);
    
    reverbReversalLabel.setText("Reverse", juce::dontSendNotification);
    reverbReversalLabel.setFont(arimo);
    addAndMakeVisible(reverbReversalLabel);

    reverseReverbAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), "reverseReverb", reverseReverbButton);
    
    // setting the decay.
    decayTimeSlider.setRange(0.1, 20.0, 0.1);
    decayTimeSlider.setNumDecimalPlacesToDisplay(1);
    decayTimeSlider.setValue(1.0);
    decayTimeSlider.setTextValueSuffix("s");
    decayTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(decayTimeSlider);
    
    decayTimeLabel.setText("Decay Time", juce::dontSendNotification);
    decayTimeLabel.attachToComponent(&decayTimeSlider, false);
    addAndMakeVisible(decayTimeLabel);
    
    decayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "decayTime", decayTimeSlider);
    
    // setting the wet mix.
    wetMixSlider.setRange(0.0, 100.0, 1.0);
    wetMixSlider.setValue(25.0);
    wetMixSlider.setTextValueSuffix("%");
    wetMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    wetMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(wetMixSlider);
    
    wetMixLabel.setText("Mix", juce::dontSendNotification);
    wetMixLabel.attachToComponent(&wetMixSlider, false);
    addAndMakeVisible(wetMixLabel);
    
    wetMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "wetMix", wetMixSlider);
    
    // setting the mod depth.
    modulationDepthSlider.setRange(0.0, 1.0, 0.1);
    modulationDepthSlider.setNumDecimalPlacesToDisplay(1);
    modulationDepthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    modulationDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(modulationDepthSlider);
    
    modulationDepthLabel.setText("Mod. Depth", juce::dontSendNotification);
    modulationDepthLabel.attachToComponent(&modulationDepthSlider, false);
    addAndMakeVisible(modulationDepthLabel);
    
    modulationDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "modulationDepth", modulationDepthSlider);
    
    // setting the mod rate.
    modulationRateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    modulationRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    modulationRateSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(modulationRateSlider);
    modulationRateLabel.setText("Mod. Rate", juce::dontSendNotification);
    modulationRateLabel.attachToComponent(&modulationRateSlider, false);
    addAndMakeVisible(modulationRateLabel);
    
    modulationRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "modulationRate", modulationRateSlider);
    
    // setting the pre-delay.
    preDelaySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    preDelaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    preDelaySlider.setTextValueSuffix("ms");
    addAndMakeVisible(preDelaySlider);

    preDelayLabel.setText("Pre-Delay", juce::dontSendNotification);
    preDelayLabel.attachToComponent(&preDelaySlider, false);
    preDelayLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(preDelayLabel);

    preDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "preDelay", preDelaySlider);

    // setting the proximity.
    proximitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    proximitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    proximitySlider.setTextValueSuffix("%");
    addAndMakeVisible(proximitySlider);

    proximityLabel.setText("Proximity", juce::dontSendNotification);
    proximityLabel.attachToComponent(&proximitySlider, false);
    proximityLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(proximityLabel);

    proximityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "proximity", proximitySlider);

    // setting the high pass.
    highPassFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    highPassFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(highPassFreqSlider);

    highPassFreqLabel.setText("High Pass", juce::dontSendNotification);
    highPassFreqLabel.attachToComponent(&highPassFreqSlider, false);
    highPassFreqLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(highPassFreqLabel);

    highPassFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "highPassFreq", highPassFreqSlider);

    // setting the low pass.
    lowPassFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowPassFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(lowPassFreqSlider);

    lowPassFreqLabel.setText("Low Pass", juce::dontSendNotification);
    lowPassFreqLabel.attachToComponent(&lowPassFreqSlider, false);
    lowPassFreqLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(lowPassFreqLabel);

    lowPassFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "lowPassFreq", lowPassFreqSlider);

    // setting the post gain.
    postGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    postGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    postGainSlider.setTextValueSuffix("dB");
    addAndMakeVisible(postGainSlider);

    postGainLabel.setText("Post-Gain", juce::dontSendNotification);
    postGainLabel.attachToComponent(&postGainSlider, false);
    postGainLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(postGainLabel);

    postGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "postGain", postGainSlider);

    // declare the section labels.
    convolutionEngineLabel.setText("Convolution Engine", juce::dontSendNotification);
    convolutionEngineLabel.setFont(vidaloka);
    addAndMakeVisible(convolutionEngineLabel);

    frequencyShaperLabel.setText("Frequency Shaper", juce::dontSendNotification);
    frequencyShaperLabel.setFont(vidaloka);
    addAndMakeVisible(frequencyShaperLabel);
    
    // manually set the font for each label -- it's a little inefficient,
    // but shouldn't impact performance (i'm just being lazy o_o).
    decayTimeLabel.setFont(arimo);
    wetMixLabel.setFont(arimo);
    reverbReversalLabel.setFont(arimo);
    preDelayLabel.setFont(arimo);
    proximityLabel.setFont(arimo);
    modulationDepthLabel.setFont(arimo);
    modulationRateLabel.setFont(arimo);
    highPassFreqLabel.setFont(arimo);
    lowPassFreqLabel.setFont(arimo);
    postGainLabel.setFont(arimo);

}

SilkGhostAudioProcessorEditor::~SilkGhostAudioProcessorEditor()
{
    reverseReverbButton.setLookAndFeel(nullptr);
}

void SilkGhostAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(0xFF71769D));
}

void SilkGhostAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
        
        // set the top bar dimensions.
        auto topBarArea = area.removeFromTop(80);
        int comboBoxWidth = 180;
        int padding = 10;
        int buttonSize = 40;
        int logoWidth = 250;
        int comboBoxDesiredHeight = 20;
        int comboBoxYPos = (topBarArea.getHeight() - comboBoxDesiredHeight) / 2;
        
        // 1. position the logo.
        auto logoArea = topBarArea.removeFromLeft(logoWidth).reduced(padding);
        logoComponent.setBounds(logoArea);
        
        // 2. position the reverb reverse button.
        const int reverseButtonLabelHeight = 20;
        const int reverseButtonLabelWidth = 60;
        auto reverbButtonArea = topBarArea
            .removeFromLeft(buttonSize + padding)
            .reduced(padding);
        reverseReverbButton.setBounds(
            reverbButtonArea.getX(),
            topBarArea.getY() + comboBoxYPos - 10,
                                      reverbButtonArea.getWidth() + 20,
            buttonSize
        );
        reverbReversalLabel.setBounds(
            reverbButtonArea.getX(),
            reverseReverbButton.getY() - 10,
                                      reverseButtonLabelWidth,
            reverseButtonLabelHeight
        );
        
        // 3. position the presets dropdown.
        auto presetsBoxArea = topBarArea
            .removeFromRight(comboBoxWidth + padding).reduced(padding);
        presetsComboBox.setBounds(presetsBoxArea.getX(),
                                  topBarArea.getY() + comboBoxYPos,
                                  presetsBoxArea.getWidth(),
                                  comboBoxDesiredHeight);
        presetsLabel.setBounds(presetsComboBox.getX(),
                               presetsComboBox.getY(),
                               50,
                               comboBoxDesiredHeight);
        
        // 4. position the quality mode dropdown.
        auto qualityBoxArea = topBarArea
            .removeFromRight(100 + padding).reduced(padding);
        qualityModeComboBox.setBounds(qualityBoxArea.getX(),
                                      topBarArea.getY() + comboBoxYPos,
                                      qualityBoxArea.getWidth(),
                                      comboBoxDesiredHeight);
        qualityModeLabel.setBounds(qualityModeComboBox.getX(),
                                   qualityModeComboBox.getY(),
                                   50,
                                   comboBoxDesiredHeight);
    
    /** GUI elements */
    
    const int leftAreaWidth = 320;
    const int rightAreaWidth = 320;
    
    jassert(leftAreaWidth + rightAreaWidth == area.getWidth());
    
    auto leftArea = area.removeFromLeft(leftAreaWidth);
    auto rightArea = area;
    
    const int sectionLabelHeight = 40;
    const int knobSize = 85;
    
    // beginning of the Convolution Engine section.
    {
        leftArea.removeFromLeft(10);
        
        convolutionEngineLabel.setBounds(leftArea.removeFromTop(sectionLabelHeight));
        leftArea.removeFromTop(20);
        
        juce::Grid convolutionGrid;
        
        // define a 2x3 grid.
        convolutionGrid.templateRows = {
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
            juce::Grid::TrackInfo(juce::Grid::Fr(1))
        };
        convolutionGrid.templateColumns = {
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
            juce::Grid::TrackInfo(juce::Grid::Fr(1))
        };
        
        // build some padding.
        convolutionGrid.rowGap = juce::Grid::Px(10);
        convolutionGrid.columnGap = juce::Grid::Px(10);
        
        convolutionGrid.items = {
            juce::GridItem(decayTimeSlider).withWidth(knobSize).withHeight(knobSize),
            juce::GridItem(wetMixSlider).withWidth(knobSize).withHeight(knobSize),
            juce::GridItem(preDelaySlider).withWidth(knobSize).withHeight(knobSize),
            juce::GridItem(proximitySlider).withWidth(knobSize).withHeight(knobSize),
            juce::GridItem(modulationDepthSlider).withWidth(knobSize).withHeight(knobSize),
            juce::GridItem(modulationRateSlider).withWidth(knobSize).withHeight(knobSize)
        };
        
        convolutionGrid.performLayout(leftArea);
    }
    
    // beginning of the Frequency Shaper section.
    {
        frequencyShaperLabel.setBounds(rightArea.removeFromTop(sectionLabelHeight));
        rightArea.removeFromTop(20);
        
        juce::Grid frequencyShaperGrid;
        
        frequencyShaperGrid.templateRows = {
            juce::Grid::TrackInfo(juce::Grid::Fr(1))
        };
        frequencyShaperGrid.templateColumns = {
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
            juce::Grid::TrackInfo(juce::Grid::Fr(1)),
            juce::Grid::TrackInfo(juce::Grid::Fr(1))
        };
        
        frequencyShaperGrid.columnGap = juce::Grid::Px(10); // Space between columns
        
        frequencyShaperGrid.items = {
            juce::GridItem(highPassFreqSlider).withWidth(knobSize).withHeight(knobSize),
            juce::GridItem(lowPassFreqSlider).withWidth(knobSize).withHeight(knobSize),
            juce::GridItem(postGainSlider).withWidth(knobSize).withHeight(knobSize)
        };
        
        frequencyShaperGrid.performLayout(rightArea);
    }
}
