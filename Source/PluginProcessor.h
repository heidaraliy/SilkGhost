#pragma once

#include <JuceHeader.h>

class SilkGhostAudioProcessor  : public juce::AudioProcessor,
                                 public juce::AudioProcessorValueTreeState::Listener
{
public:
    SilkGhostAudioProcessor();
    ~SilkGhostAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // public getter for parameters.
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    const juce::AudioProcessorValueTreeState& getParameters() const { return parameters; }

    // listener for parameter changes.
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    
    // build out a struct so that we can define presets, a vector to store them,
    // and a function to load them --  this will allow us to handle pre-created
    // plugin presets as well as supporting user-created presets! make them public
    // so that we can access them in our PluginEditor files.
    struct Preset
    {
        juce::String name;
        std::map<juce::String, float> parameters;
    };
    std::vector<Preset> presets;
    void loadPreset(int presetIndex);
    std::atomic<bool> isLoadingPreset { false };
    
    // use JUCE's value tree to store parameters and manage state.
    juce::AudioProcessorValueTreeState parameters;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SilkGhostAudioProcessor)

    // declare a convolution engine, the crux of this plugin.
    juce::dsp::Convolution convolution;

    // functions to create a parameter layout, generate impulse
    // responses, and downsample IRs when we modify the signal quality.
    juce::AudioBuffer<float> downsampleImpulseResponse(const juce::AudioBuffer<float>& impulseResponse, int factor);
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioBuffer<float> createReverbImpulseResponse(float duration, double sampleRate, bool reverseReverb, float proximity);
    float decayTime = 1.0f;

    // set up variables to precompute the impulse responses --
    // we're precomputing because recalculating the IR with each
    // minute change a user makes to the Decay Time knob will
    // *obliterate* CPU performance.
    std::map<float, juce::AudioBuffer<float>> precomputedImpulseResponses;
    const float minDecayTime = 0.5f;
    const float maxDecayTime = 10.0f;
    const float decayStep = 0.5f;
    bool reverseReverb = false;
    
    // build some variables using JUCE classes to control
    // filters, set up preDelayLines for IR (so that we can introduce
    // delay before a reverb tail and simulate large spaces), and
    // diffusers to set proximity.
    juce::dsp::StateVariableTPTFilter<float> highPassFilter;
    juce::dsp::StateVariableTPTFilter<float> lowPassFilter;
    juce::dsp::DelayLine<float> preDelayLine;
    juce::dsp::FirstOrderTPTFilter<float> diffuser1;
    juce::dsp::FirstOrderTPTFilter<float> diffuser2;

    // set up a modulation processor using JUCE's chorus class.
    juce::dsp::Chorus<float> modulator;

    // we need to declare these as atomic floats so that we can access
    // them within the thread safely.
    std::atomic<float> proximityParameter { 0.5f };
    std::atomic<float> modulationDepth { 0.5f };
    std::atomic<float> modulationRate { 0.5f };
    std::atomic<float> highPassCutoff { 200.0f };
    std::atomic<float> lowPassCutoff { 18000.0f };
    std::atomic<float> postGain { 0.0f };
    std::atomic<int> signalQuality;

    // need to use a mutex to update the IR within the thread safely as
    // well -- failure to do so will cause race conditions!
    std::mutex irMutex;
    std::atomic<bool> irNeedsUpdate { false };
    juce::AudioBuffer<float> newImpulseResponseBuffer;

    // declare a thread pool so that we can move resources to the thread
    // vs. updating directly on the buffer, which will cause really
    // poor performance stemming from extreme CPU usage.
    juce::ThreadPool irThreadPool;

    // use JUCE's built-in DryWetMixer to mix the signal easily --
    // it's a bit tough to get equilibrium between a dry and wet signal
    // manually because we need to factor in latency, and WetDryMixer
    // does that for us automatically.
    juce::dsp::DryWetMixer<float> dryWetMixer;
        
};
