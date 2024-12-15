#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

class SilkGhostAudioProcessorEditor  :  public juce::AudioProcessorEditor

{
public:
    SilkGhostAudioProcessorEditor (SilkGhostAudioProcessor&);
    ~SilkGhostAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
        
private:
    SilkGhostAudioProcessor& audioProcessor;

    // interactive elements.
    juce::Slider decayTimeSlider;
    juce::Slider wetMixSlider;
    juce::Slider highPassFreqSlider;
    juce::Slider lowPassFreqSlider;
    juce::Slider preDelaySlider;
    juce::Slider modulationDepthSlider;
    juce::Slider modulationRateSlider;
    juce::Slider proximitySlider;
    juce::Slider postGainSlider;
    juce::ToggleButton reverseReverbButton;

    // labels.
    juce::Label decayTimeLabel;
    juce::Label wetMixLabel;
    juce::Label highPassFreqLabel;
    juce::Label lowPassFreqLabel;
    juce::Label preDelayLabel;
    juce::Label qualityModeLabel;
    juce::Label modulationDepthLabel;
    juce::Label modulationRateLabel;
    juce::Label proximityLabel;
    juce::Label postGainLabel;
    juce::Label presetsLabel;
    juce::Label reverbReversalLabel;

    // section headers.
    juce::Label convolutionEngineLabel;
    juce::Label reverbModulatorLabel;
    juce::Label frequencyShaperLabel;
    juce::Label frequencySpectrumLabel;

    // dropdowns.
    juce::ComboBox qualityModeComboBox;
    juce::ComboBox presetsComboBox;

    // attachments.
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> preDelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverseReverbAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> qualityModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modulationDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modulationRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> proximityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> postGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> presetsAttachment;

    // logo.
    juce::Image logoImage;
    juce::ImageComponent logoComponent;

    // fonts.
    juce::ReferenceCountedObjectPtr<juce::Typeface> vidalokaTypeFace;
    juce::FontOptions vidaloka;

    juce::ReferenceCountedObjectPtr<juce::Typeface> arimoRegularTypeFace;
    juce::FontOptions arimo;
    
    // declare a custom LookAndFeel instance!
    CustomLookAndFeel customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SilkGhostAudioProcessorEditor)
};
