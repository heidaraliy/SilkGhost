#include "PluginProcessor.h"
#include "PluginEditor.h"

SilkGhostAudioProcessor::SilkGhostAudioProcessor()
    : AudioProcessor (
#ifndef JucePlugin_PreferredChannelConfigurations
        BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
            .withInput ("Input", juce::AudioChannelSet::stereo(), true)
#endif
            .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
#endif
    )
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
    , irThreadPool(juce::ThreadPoolOptions()
         .withThreadName("IR Generation Pool")
         .withNumberOfThreads(1)
         .withThreadStackSizeBytes(juce::Thread::osDefaultStackSize)
         .withDesiredThreadPriority(juce::Thread::Priority::normal))
{
    
    // define presets -- ideally, this'd be a little prettier but we can just
    // stuff all of them in here for now and rev later.
    presets.push_back({"Large Hall", {
        {"decayTime", 5.0f},
        {"wetMix", 60.0f},
        {"highPassFreq", 100.0f},
        {"lowPassFreq", 15000.0f},
        {"preDelay", 20.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.2f},
        {"modulationRate", 0.5f},
        {"proximity", 80.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Small Room", {
        {"decayTime", 1.0f},
        {"wetMix", 30.0f},
        {"highPassFreq", 200.0f},
        {"lowPassFreq", 18000.0f},
        {"preDelay", 5.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.0f},
        {"modulationRate", 0.0f},
        {"proximity", 20.0f},
        {"postGain", 0.0f}
    }});
    
    presets.push_back({"Dark Ambience", {
        {"decayTime", 4.0f},
        {"wetMix", 50.0f},
        {"highPassFreq", 100.0f},
        {"lowPassFreq", 12000.0f},
        {"preDelay", 10.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.1f},
        {"modulationRate", 0.2f},
        {"proximity", 40.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Cathedral Space", {
        {"decayTime", 8.0f},
        {"wetMix", 70.0f},
        {"highPassFreq", 50.0f},
        {"lowPassFreq", 18000.0f},
        {"preDelay", 30.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.15f},
        {"modulationRate", 0.3f},
        {"proximity", 60.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Vocal Air", {
        {"decayTime", 2.0f},
        {"wetMix", 35.0f},
        {"highPassFreq", 150.0f},
        {"lowPassFreq", 17000.0f},
        {"preDelay", 15.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.05f},
        {"modulationRate", 0.1f},
        {"proximity", 30.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Plate Reflection", {
        {"decayTime", 3.5f},
        {"wetMix", 45.0f},
        {"highPassFreq", 200.0f},
        {"lowPassFreq", 16000.0f},
        {"preDelay", 5.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.2f},
        {"modulationRate", 1.0f},
        {"proximity", 50.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Reverse Bloom", {
        {"decayTime", 4.0f},
        {"wetMix", 80.0f},
        {"highPassFreq", 100.0f},
        {"lowPassFreq", 15000.0f},
        {"preDelay", 0.0f},
        {"reverseReverb", true},
        {"modulationDepth", 0.25f},
        {"modulationRate", 0.5f},
        {"proximity", 40.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Infinite Whisper", {
        {"decayTime", 10.0f},
        {"wetMix", 90.0f},
        {"highPassFreq", 50.0f},
        {"lowPassFreq", 20000.0f},
        {"preDelay", 20.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.3f},
        {"modulationRate", 0.4f},
        {"proximity", 20.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Ghostly Reverse", {
        {"decayTime", 5.0f},
        {"wetMix", 60.0f},
        {"highPassFreq", 120.0f},
        {"lowPassFreq", 14000.0f},
        {"preDelay", 0.0f},
        {"reverseReverb", true},
        {"modulationDepth", 0.4f},
        {"modulationRate", 0.3f},
        {"proximity", 50.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Drum Chamber", {
        {"decayTime", 1.5f},
        {"wetMix", 40.0f},
        {"highPassFreq", 200.0f},
        {"lowPassFreq", 18000.0f},
        {"preDelay", 10.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.0f},
        {"modulationRate", 0.0f},
        {"proximity", 70.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Metallic Plate", {
        {"decayTime", 2.5f},
        {"wetMix", 50.0f},
        {"highPassFreq", 80.0f},
        {"lowPassFreq", 15000.0f},
        {"preDelay", 5.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.1f},
        {"modulationRate", 2.0f},
        {"proximity", 40.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Reverse Swell Vox", {
        {"decayTime", 3.0f},
        {"wetMix", 70.0f},
        {"highPassFreq", 180.0f},
        {"lowPassFreq", 16000.0f},
        {"preDelay", 0.0f},
        {"reverseReverb", true},
        {"modulationDepth", 0.2f},
        {"modulationRate", 0.5f},
        {"proximity", 30.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Warm Hall", {
        {"decayTime", 6.0f},
        {"wetMix", 55.0f},
        {"highPassFreq", 100.0f},
        {"lowPassFreq", 14000.0f},
        {"preDelay", 25.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.1f},
        {"modulationRate", 0.2f},
        {"proximity", 60.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Short Ambience", {
        {"decayTime", 0.8f},
        {"wetMix", 25.0f},
        {"highPassFreq", 200.0f},
        {"lowPassFreq", 19000.0f},
        {"preDelay", 5.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.0f},
        {"modulationRate", 0.0f},
        {"proximity", 10.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Reverse Cascade", {
        {"decayTime", 4.5f},
        {"wetMix", 80.0f},
        {"highPassFreq", 150.0f},
        {"lowPassFreq", 17000.0f},
        {"preDelay", 0.0f},
        {"reverseReverb", true},
        {"modulationDepth", 0.35f},
        {"modulationRate", 1.0f},
        {"proximity", 20.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Bright Hall", {
        {"decayTime", 7.5f},
        {"wetMix", 65.0f},
        {"highPassFreq", 50.0f},
        {"lowPassFreq", 20000.0f},
        {"preDelay", 30.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.15f},
        {"modulationRate", 0.2f},
        {"proximity", 50.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Dreamscape Reverse", {
        {"decayTime", 5.5f},
        {"wetMix", 90.0f},
        {"highPassFreq", 100.0f},
        {"lowPassFreq", 18000.0f},
        {"preDelay", 0.0f},
        {"reverseReverb", true},
        {"modulationDepth", 0.3f},
        {"modulationRate", 0.4f},
        {"proximity", 40.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Distant Hallway", {
        {"decayTime", 3.0f},
        {"wetMix", 45.0f},
        {"highPassFreq", 120.0f},
        {"lowPassFreq", 15000.0f},
        {"preDelay", 15.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.05f},
        {"modulationRate", 0.1f},
        {"proximity", 15.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Soft Plate", {
        {"decayTime", 2.0f},
        {"wetMix", 40.0f},
        {"highPassFreq", 200.0f},
        {"lowPassFreq", 15000.0f},
        {"preDelay", 5.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.1f},
        {"modulationRate", 2.0f},
        {"proximity", 35.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Reverse Vocal Wash", {
        {"decayTime", 4.0f},
        {"wetMix", 75.0f},
        {"highPassFreq", 180.0f},
        {"lowPassFreq", 16000.0f},
        {"preDelay", 0.0f},
        {"reverseReverb", true},
        {"modulationDepth", 0.25f},
        {"modulationRate", 0.3f},
        {"proximity", 25.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Ethereal Chamber", {
        {"decayTime", 5.0f},
        {"wetMix", 55.0f},
        {"highPassFreq", 100.0f},
        {"lowPassFreq", 17000.0f},
        {"preDelay", 10.0f},
        {"reverseReverb", false},
        {"modulationDepth", 0.2f},
        {"modulationRate", 0.2f},
        {"proximity", 45.0f},
        {"postGain", 0.0f}
    }});

    presets.push_back({"Endless Reverse", {
        {"decayTime", 9.0f},
        {"wetMix", 95.0f},
        {"highPassFreq", 80.0f},
        {"lowPassFreq", 19000.0f},
        {"preDelay", 0.0f},
        {"reverseReverb", true},
        {"modulationDepth", 0.4f},
        {"modulationRate", 0.5f},
        {"proximity", 10.0f},
        {"postGain", 0.0f}
    }});
    
    // Add parameter listeners
    parameters.addParameterListener("decayTime", this);
    parameters.addParameterListener("highPassFreq", this);
    parameters.addParameterListener("lowPassFreq", this);
    parameters.addParameterListener("reverseReverb", this);
    parameters.addParameterListener("modulationDepth", this);
    parameters.addParameterListener("modulationRate", this);
    parameters.addParameterListener("proximity", this);
    parameters.addParameterListener("postGain", this);
    parameters.addParameterListener("wetMix", this);
    parameters.addParameterListener("signalQuality", this);
    parameters.addParameterListener("presetSelection", this);
}

SilkGhostAudioProcessor::~SilkGhostAudioProcessor()
{
    parameters.removeParameterListener("decayTime", this);
    parameters.removeParameterListener("highPassFreq", this);
    parameters.removeParameterListener("lowPassFreq", this);
    parameters.removeParameterListener("reverseReverb", this);
    parameters.removeParameterListener("modulationDepth", this);
    parameters.removeParameterListener("modulationRate", this);
    parameters.removeParameterListener("proximity", this);
    parameters.removeParameterListener("postGain", this);
    parameters.removeParameterListener("wetMix", this);
    parameters.removeParameterListener("presetSelection", this);
}

const juce::String SilkGhostAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SilkGhostAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SilkGhostAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SilkGhostAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SilkGhostAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SilkGhostAudioProcessor::getNumPrograms()
{
    return 1;
}

int SilkGhostAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SilkGhostAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SilkGhostAudioProcessor::getProgramName (int index)
{
    return {};
}

void SilkGhostAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void SilkGhostAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    convolution.prepare(spec);

    // prepare the dry/wet mixer.
    dryWetMixer.reset();
    dryWetMixer.prepare(spec);
    dryWetMixer.setMixingRule(juce::dsp::DryWetMixingRule::balanced);

    // initialize decayTime from parameters.
    decayTime = *parameters.getRawParameterValue("decayTime");
    reverseReverb = *parameters.getRawParameterValue("reverseReverb") > 0.5f;
    
    float proximity = *parameters.getRawParameterValue("proximity");
    proximityParameter.store(*parameters.getRawParameterValue("proximity"));

    auto impulseResponse = createReverbImpulseResponse(decayTime, sampleRate, reverseReverb, proximity);
    
    // set up the quality modes here. we'll cut the impulse responses by a factor of two for each
    // level after high.
    int qualityMode = signalQuality.load();
    
    if (qualityMode == 0)
    {
        convolution.loadImpulseResponse(
            std::move(impulseResponse),
            sampleRate,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            juce::dsp::Convolution::Normalise::yes);
    }
    else if (qualityMode == 1)
    {
        auto downsampledIR = downsampleImpulseResponse(impulseResponse, 2);
        convolution.loadImpulseResponse(
            std::move(downsampledIR),
            sampleRate / 2,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            juce::dsp::Convolution::Normalise::yes);
    }
    else if (qualityMode == 2)
    {
        auto downsampledIR = downsampleImpulseResponse(impulseResponse, 4);
        convolution.loadImpulseResponse(
            std::move(downsampledIR),
            sampleRate / 4,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            juce::dsp::Convolution::Normalise::yes);
    }
    else if (qualityMode == 3) // garbage (ew!)
    {
        auto downsampledIR = downsampleImpulseResponse(impulseResponse, 6);
        convolution.loadImpulseResponse(
            std::move(downsampledIR),
            sampleRate / 6,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            juce::dsp::Convolution::Normalise::yes);
    }
    
    int latencySamples = convolution.getLatency();
    dryWetMixer.setWetLatency(latencySamples);

    // report latency to host.
    setLatencySamples(latencySamples);

    // prepare filters.
    highPassFilter.prepare(spec);
    highPassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highPassFilter.setCutoffFrequency(highPassCutoff.load());

    lowPassFilter.prepare(spec);
    lowPassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    lowPassFilter.setCutoffFrequency(lowPassCutoff.load());

    // prepare pre-delay line.
    preDelayLine.reset();
    preDelayLine.prepare(spec);
    preDelayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.2f));

    // prepare diffusion filters.
    diffuser1.reset();
    diffuser1.prepare(spec);
    diffuser1.setType(juce::dsp::FirstOrderTPTFilterType::allpass);
    diffuser1.setCutoffFrequency(2000.0f);

    diffuser2.reset();
    diffuser2.prepare(spec);
    diffuser2.setType(juce::dsp::FirstOrderTPTFilterType::allpass);
    diffuser2.setCutoffFrequency(5000.0f);

    // prepare modulation processor.
    modulator.reset();
    modulator.prepare(spec);
    modulator.setCentreDelay(10.0f);
}

// the createReverbImpulseResponse impulse response handles a ton of the logic that drives the
// convolution engine. it'll read a signal into a buffer and generate impulse responses to simulate
// a reverb effect.
juce::AudioBuffer<float> SilkGhostAudioProcessor::createReverbImpulseResponse(float duration, double sampleRate, bool reverseReverb, float proximity)
{
    const int length = (int)(sampleRate * duration);
    juce::AudioBuffer<float> impulseResponse(2, length);
    impulseResponse.clear();

    // early reflections.
    const int numEarlyReflections = 12;
    float earlyDelaysMs[numEarlyReflections] = {7.0f,11.0f,13.0f,17.0f,23.0f,29.0f,31.0f,37.0f,41.0f,43.0f,47.0f,53.0f};
    float earlyGains[numEarlyReflections];
    for (int i = 0; i < numEarlyReflections; ++i)
        earlyGains[i] = juce::Random::getSystemRandom().nextFloat()*0.5f + 0.5f;

    for (int i = 0; i < numEarlyReflections; ++i)
    {
        int delaySamples = (int)(earlyDelaysMs[i] * sampleRate / 1000.0f);
        if (delaySamples < length)
        {
            float g = earlyGains[i];
            float signL = (juce::Random::getSystemRandom().nextBool() ? 1.0f : -1.0f);
            float signR = (juce::Random::getSystemRandom().nextBool() ? 1.0f : -1.0f);
            impulseResponse.setSample(0, delaySamples, g * signL);
            impulseResponse.setSample(1, delaySamples, g * signR);
        }
    }

    // late reverb: continuous noise with exponential decay.
    // use a stable, smooth exponential decay: ~-60 dB at 'duration':
    // decay envelope: exp(-6.91 * t / duration)
    const int lateStart = (int)(0.1 * sampleRate);
    for (int i = lateStart; i < length; ++i)
    {
        float t = (float)i / (float)sampleRate;
        float decayEnv = std::exp(-6.91f * t / duration);

        // generate noise: small random fluctuations for each channel.
        float nL = (juce::Random::getSystemRandom().nextFloat()*2.0f - 1.0f) * 0.5f;
        float nR = (juce::Random::getSystemRandom().nextFloat()*2.0f - 1.0f) * 0.5f;

        impulseResponse.setSample(0, i, nL * decayEnv);
        impulseResponse.setSample(1, i, nR * decayEnv);
    }

    // gentle amplitude modulation instead of re-indexing:
    // we'll vary amplitude slightly with a slow sine to avoid static sound.
    float modDepth = 0.05f;   // 5% amplitude variation.
    float modRate = 0.1f;     // slow modulation rate, 0.1 Hz.
    for (int c = 0; c < impulseResponse.getNumChannels(); ++c)
    {
        auto* data = impulseResponse.getWritePointer(c);
        for (int i = lateStart; i < length; ++i)
        {
            float t = (float)i / (float)sampleRate;
            float mod = 1.0f + modDepth * std::sin(2.0f * juce::MathConstants<float>::pi * modRate * t);
            data[i] *= mod;
        }
    }

    // adjust the proximity: early vs. late energy.
    proximity = juce::jlimit(0.0f, 1.0f, proximity / 100.0f);
    float earlyGain = juce::jmap(proximity, 1.0f, 0.0f);
    float lateGain = juce::jmap(proximity, 0.5f, 1.0f);
    for (int c = 0; c < impulseResponse.getNumChannels(); ++c)
    {
        auto* data = impulseResponse.getWritePointer(c);
        for (int i = 0; i < length; ++i)
        {
            if (i < lateStart)
                data[i] *= earlyGain;
            else
                data[i] *= lateGain;
        }
    }

    if (reverseReverb)
    {
        for (int channel = 0; channel < impulseResponse.getNumChannels(); ++channel)
        {
            auto* data = impulseResponse.getWritePointer(channel);
            std::reverse(data, data + length);
        }

        // analyze the first 100ms of reversed IR.
        int checkSamples = (int)(0.1 * sampleRate); // set the 100ms here.
        checkSamples = juce::jmin(checkSamples, length);

        float startMax = 0.0f;
        for (int c = 0; c < impulseResponse.getNumChannels(); ++c)
        {
            auto* d = impulseResponse.getReadPointer(c);
            for (int i = 0; i < checkSamples; ++i)
                startMax = juce::jmax(startMax, std::abs(d[i]));
        }

        // hacky, but if start is too quiet, boost the entire IR so that startMax ~ 0.9f!
        if (startMax < 0.9f && startMax > 0.0f)
        {
            float boost = 0.9f / startMax;
            impulseResponse.applyGain(boost);
        }
    }

    // finally, normalize the IR if needed.
    float maxAmp = 0.0f;
    for (int c = 0; c < impulseResponse.getNumChannels(); ++c)
        maxAmp = juce::jmax(maxAmp, impulseResponse.getMagnitude(c, 0, length));

    if (maxAmp > 0.0f)
        impulseResponse.applyGain(1.0f / maxAmp);

    return impulseResponse;
}

juce::AudioBuffer<float> SilkGhostAudioProcessor::downsampleImpulseResponse(const juce::AudioBuffer<float>& impulseResponse, int factor)
{
    int numChannels = impulseResponse.getNumChannels();
    int numSamples = impulseResponse.getNumSamples() / factor;
    juce::AudioBuffer<float> downsampledIR(numChannels, numSamples);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* readPtr = impulseResponse.getReadPointer(channel);
        auto* writePtr = downsampledIR.getWritePointer(channel);
        for (int i = 0; i < numSamples; ++i)
        {
            writePtr[i] = readPtr[i * factor];
        }
    }

    return downsampledIR;
}

void SilkGhostAudioProcessor::loadPreset(int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= (int)presets.size())
    {
        DBG("Preset index out of range: " + juce::String(presetIndex));
        return;
    }

    isLoadingPreset.store(true);

    const Preset& preset = presets[presetIndex];

    for (const auto& param : preset.parameters)
    {
        if (auto* parameter = parameters.getParameter(param.first))
        {
            float newValue = param.second;

            // special handling for bool parameters. they need to be set to
            // floats -- otherwise, they won't work.
            if (param.first == "reverseReverb")
            {
                newValue = (param.second > 0.5f) ? 1.0f : 0.0f;
            }

            parameter->beginChangeGesture();
            parameter->setValueNotifyingHost(parameters.getParameterRange(param.first).convertTo0to1(newValue));
            parameter->endChangeGesture();
        }
    }

    isLoadingPreset.store(false);

    // force an IR update once to rebuild the parameters properly.
    double sr = getSampleRate();
    if (sr > 0.0)
    {
        float irDuration = *parameters.getRawParameterValue("decayTime");
        bool reverse = (*parameters.getRawParameterValue("reverseReverb") > 0.5f);
        float proximityVal = *parameters.getRawParameterValue("proximity");

        auto newIR = createReverbImpulseResponse(irDuration, sr, reverse, proximityVal);
        {
            std::lock_guard<std::mutex> lock(irMutex);
            newImpulseResponseBuffer = std::move(newIR);
            irNeedsUpdate.store(true);
        }
    }

    // trigger UI and host updates.
    updateHostDisplay();
    if (auto* editor = getActiveEditor())
        editor->repaint();
}

juce::AudioProcessorValueTreeState::ParameterLayout SilkGhostAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    static constexpr const char* allPresetNames[] = {
        "Large Hall",
        "Small Room",
        "Dark Ambience",
        "Cathedral Space",
        "Vocal Air",
        "Plate Reflection",
        "Reverse Bloom",
        "Infinite Whisper",
        "Ghostly Reverse",
        "Drum Chamber",
        "Metallic Plate",
        "Reverse Swell Vox",
        "Warm Hall",
        "Short Ambience",
        "Reverse Cascade",
        "Bright Hall",
        "Dreamscape Reverse",
        "Distant Hallway",
        "Soft Plate",
        "Reverse Vocal Wash",
        "Ethereal Chamber",
        "Endless Reverse"
    };

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "decayTime",
        "Decay Time",
        juce::NormalisableRange<float>(0.5f, 10.0f, 0.1f),
        2.0f));

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "wetMix",
        "Wet Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        25.0f));

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "highPassFreq", "High-Pass Freq", juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f), 200.0f));

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "lowPassFreq", "Low-Pass Freq", juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f), 18000.0f));
    
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "preDelay",
        "Pre-Delay",
        juce::NormalisableRange<float>(0.0f, 200.0f, 1.0f),
        0.0f));

    params.emplace_back(std::make_unique<juce::AudioParameterBool>(
        "reverseReverb",
        "Reverse Reverb",
        false));

    juce::StringArray qualityOptions = { "High", "Medium", "Low", "Garbage" };
    params.emplace_back(std::make_unique<juce::AudioParameterChoice>(
        "qualityMode",
        "Quality Mode",
        qualityOptions,
        0));
    
    juce::StringArray presetOptions;
    for (auto preset : allPresetNames)
        presetOptions.add(preset);

    params.emplace_back(std::make_unique<juce::AudioParameterChoice>(
        "presetSelection",
        "Preset",
        presetOptions,
        0));
    
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "modulationDepth", "Modulation Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.1f));

    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "modulationRate", "Modulation Rate",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f), 0.1f));
    
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "proximity", "Proximity",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f));
    
    params.emplace_back(std::make_unique<juce::AudioParameterFloat>(
        "postGain", "Post Gain",
        juce::NormalisableRange<float>(-36.0f, 36.0f, 0.1f), 0.0f));

    return { params.begin(), params.end() };
}

void SilkGhostAudioProcessor::releaseResources()
{
    // reset convolution and filters so we don't hog CPU resources.
    convolution.reset();
    highPassFilter.reset();
    lowPassFilter.reset();
    preDelayLine.reset();
    diffuser1.reset();
    diffuser2.reset();
    modulator.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SilkGhostAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // only mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // input layout must match output layout.
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SilkGhostAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // update impulse response if needed.
    if (irNeedsUpdate.load())
    {
        juce::AudioBuffer<float> irToLoad;

        // lock mutex to safely access the new IR buffer!
        {
            std::lock_guard<std::mutex> lock(irMutex);
            irToLoad = newImpulseResponseBuffer;
            irNeedsUpdate.store(false);
        }

        // load the new impulse response into the convolution processor.
        convolution.loadImpulseResponse(
            std::move(irToLoad),
            getSampleRate(),
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::no,
            juce::dsp::Convolution::Normalise::yes);
    }

    // get the wet mix parameter.
    float wetMix = *parameters.getRawParameterValue("wetMix") / 100.0f;
    wetMix = juce::jlimit(0.0f, 1.0f, wetMix);
    
    // set mixing proportions.
    dryWetMixer.setWetMixProportion(wetMix);

    // create an AudioBlock from buffer.
    juce::dsp::AudioBlock<float> block(buffer);

    // save dry input signal.
    dryWetMixer.pushDrySamples(block);

    // get pre-delay in samples.
    float preDelayMs = *parameters.getRawParameterValue("preDelay");
    float preDelaySamples = (preDelayMs / 1000.0f) * getSampleRate();
    preDelayLine.setDelay(preDelaySamples);

    // process pre-delay.
    juce::dsp::ProcessContextReplacing<float> preDelayContext(block);
    preDelayLine.process(preDelayContext);

    // process diffusion filters.
    juce::dsp::ProcessContextReplacing<float> diffusionContext(block);
    diffuser1.process(diffusionContext);
    diffuser2.process(diffusionContext);

    // process convolution (wet signal).
    juce::dsp::ProcessContextReplacing<float> convolutionContext(block);
    convolution.process(convolutionContext);

    // update modulation parameters.
    modulator.setRate(modulationRate.load());
    modulator.setDepth(modulationDepth.load());

    // process modulation.
    juce::dsp::ProcessContextReplacing<float> modContext(block);
    modulator.process(modContext);

    // apply filters to the wet signal.
    highPassFilter.setCutoffFrequency(highPassCutoff.load());
    lowPassFilter.setCutoffFrequency(lowPassCutoff.load());
    juce::dsp::ProcessContextReplacing<float> filterContext(block);
    highPassFilter.process(filterContext);
    lowPassFilter.process(filterContext);
    
    float internalBoost = juce::Decibels::decibelsToGain(12.0f);
    float gain = juce::Decibels::decibelsToGain(postGain.load());
    block.multiplyBy(gain * internalBoost);

    // finally, mix dry and wet signals.
    dryWetMixer.mixWetSamples(block);
}

void SilkGhostAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (isLoadingPreset.load())
            return;
    if (parameterID == "decayTime" || parameterID == "reverseReverb" || parameterID == "proximity")
    {
        double sampleRate = getSampleRate();
        if (sampleRate > 0.0)
        {
            float irDuration = *parameters.getRawParameterValue("decayTime");
            bool reverse = *parameters.getRawParameterValue("reverseReverb") > 0.5f;
            float proximity = *parameters.getRawParameterValue("proximity");
            proximityParameter.store(proximity);
            
            auto newIR = createReverbImpulseResponse(irDuration, sampleRate, reverse, proximity);

            // lock mutex and update the new IR buffer.
            {
                std::lock_guard<std::mutex> lock(irMutex);
                newImpulseResponseBuffer = std::move(newIR);
                irNeedsUpdate.store(true);
            }
        }
    }
    else if (parameterID == "highPassFreq")
    {
        highPassCutoff.store(newValue);
    }
    else if (parameterID == "lowPassFreq")
    {
        lowPassCutoff.store(newValue);
    }
    else if (parameterID == "modulationDepth")
    {
        modulationDepth.store(newValue);
    }
    else if (parameterID == "modulationRate")
    {
        modulationRate.store(newValue);
    }
    else if (parameterID == "postGain")
    {
        postGain.store(newValue);
    }
    else if (parameterID == "signalQuality")
    {
        signalQuality.store(static_cast<int>(newValue));
    }
    else if (parameterID == "presetSelection")
    {
        int presetIndex = static_cast<int>(newValue);
        loadPreset(presetIndex);
    }
}

bool SilkGhostAudioProcessor::hasEditor() const
{
    // we're using a GUI editor -- this needs to be true!
    return true;
}

juce::AudioProcessorEditor* SilkGhostAudioProcessor::createEditor()
{
    return new SilkGhostAudioProcessorEditor (*this);
}

void SilkGhostAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // save the state as XML.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SilkGhostAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // restore the state from XML.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SilkGhostAudioProcessor();
}
