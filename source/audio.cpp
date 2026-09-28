#include "audio.h"
#include "SDL3/SDL_audio.h"
#include "alhelpers.h"
#include <AL/alext.h>
#include <stdexcept>

Audio* Audio::Instance = nullptr;

Audio::Source::Source(Audio::Sample* sample) {
    alGenSources(1, &m_source);
    alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSource3i(m_source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
    if (sample) {
        alSourcei(m_source, AL_BUFFER, sample->buffer);
    }
    SetPos(0.0f);
}

Audio::Source::~Source() {
    alDeleteSources(1, &m_source);
}

void Audio::Source::SetSample(Sample* sample) {
    alSourcei(m_source, AL_BUFFER, sample->buffer);
}

void Audio::Source::Play() {
    alSourcePlay(m_source);
}

void Audio::Source::Stop() {
    alSourceStop(m_source);
}

bool Audio::Source::IsPlaying() {
    ALint state;

    alGetSourcei(m_source, AL_SOURCE_STATE, &state);

    return state == AL_PLAYING;
}

void Audio::Source::Set3D() {
    alSource3i(m_source, AL_AUXILIARY_SEND_FILTER, Audio::Instance->m_reverbEffect, 0, AL_FILTER_NULL);
    alSourcei(m_source, AL_SOURCE_RELATIVE, AL_FALSE);
    alSourcef(m_source, AL_ROLLOFF_FACTOR, 1.0f);
    alSourcef(m_source, AL_REFERENCE_DISTANCE, 1.0f);
}

void Audio::Source::SetLoop(bool loop) {
    alSourcei(m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

void Audio::Source::SetPos(Vector pos) {
    alSource3f(m_source, AL_POSITION, pos.x, pos.y, pos.z);
}

Audio::Sequence::Sequence(Source* source) : m_source(source), m_index(-1) {}

Audio::Sequence::~Sequence() {
    Stop();
}

void Audio::Sequence::Play() {
    m_index = 0;
    m_source->SetSample(m_samples[0]);
    m_source->Play();
}

void Audio::Sequence::Stop() {
    m_source->Stop();
    m_index = -1;
}

void Audio::Sequence::AddSample(Sample* sample) {
    m_samples.push_back(sample);
}

void Audio::Sequence::Update() {
    if (m_index >= 0 && !m_source->IsPlaying()) {
        if (m_index + 1 == m_samples.size()) {
            m_index = -1;
            return;
        }
        m_source->SetSample(m_samples[++m_index]);
        m_source->Play();
    }
}

Audio::Audio() {
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        throw std::runtime_error("no audio device found");
    }

    ALCint attribs[] = {
        ALC_HRTF_SOFT, ALC_TRUE,
        0
    };

    m_context = alcCreateContext(m_device, attribs);
    if (!m_context || !alcMakeContextCurrent(m_context)) {
        alcCloseDevice(m_device);
        throw std::runtime_error("failed to create audio context");
    }

    LoadALExtensions();

    palGenAuxiliaryEffectSlots(1, &m_auxSlot);
    palGenEffects(1, &m_reverbEffect);
    palEffecti(m_reverbEffect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);

    Instance = this;
}

Audio::~Audio() {
    for (auto& entry : m_loadedSamples) {
        delete entry.second;
    }

    palDeleteEffects(1, &m_reverbEffect);
    palDeleteAuxiliaryEffectSlots(1, &m_auxSlot);

    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

Audio::Sample* Audio::LoadSample(std::string filename) {
    if (m_loadedSamples.count(filename)) {
        return m_loadedSamples[filename];
    }

    SDL_AudioSpec spec;
    Uint8* data;
    Uint32 dataSize;

    if (!SDL_LoadWAV(filename.c_str(), &spec, &data, &dataSize)) {
        return nullptr;
    }

    if (spec.format != SDL_AUDIO_S16) {
        printf("unsupported audio format: %d\n", spec.format);
        return nullptr;
    }

    unsigned int buffer;
    alGenBuffers(1, &buffer);
    if (!buffer) {
        SDL_free(data);
        return nullptr;
    }

    int format = (spec.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    alBufferData(buffer, format, data, dataSize, spec.freq);

    SDL_free(data);

    Sample* sample = new Sample();
    sample->buffer = buffer;
    m_loadedSamples[filename] = sample;

    return sample;
}

void Audio::SetListenerPos(Vector pos) {
    alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void Audio::SetListenerDir(Vector dir) {
    float orientation[6] = {dir.x, dir.y, dir.z, 0.0f, 1.0f, 0.0f};
    alListenerfv(AL_ORIENTATION, orientation);
}

void Audio::SetReverb(EFXEAXREVERBPROPERTIES reverb) {
    palEffectf(m_reverbEffect, AL_REVERB_DENSITY, reverb.flDensity);
    palEffectf(m_reverbEffect, AL_REVERB_DIFFUSION, reverb.flDiffusion);
    palEffectf(m_reverbEffect, AL_REVERB_GAIN, reverb.flGain);
    palEffectf(m_reverbEffect, AL_REVERB_GAINHF, reverb.flGainHF);
    palEffectf(m_reverbEffect, AL_REVERB_DECAY_TIME, reverb.flDecayTime);
    palEffectf(m_reverbEffect, AL_REVERB_DECAY_HFRATIO, reverb.flDecayHFRatio);
    palEffectf(m_reverbEffect, AL_REVERB_REFLECTIONS_GAIN, reverb.flReflectionsGain);
    palEffectf(m_reverbEffect, AL_REVERB_REFLECTIONS_DELAY, reverb.flReflectionsDelay);
    palEffectf(m_reverbEffect, AL_REVERB_LATE_REVERB_GAIN, reverb.flLateReverbGain);
    palEffectf(m_reverbEffect, AL_REVERB_LATE_REVERB_DELAY, reverb.flLateReverbDelay);
    palEffectf(m_reverbEffect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb.flAirAbsorptionGainHF);
    palEffectf(m_reverbEffect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb.flRoomRolloffFactor);
    palEffecti(m_reverbEffect, AL_REVERB_DECAY_HFLIMIT, reverb.iDecayHFLimit);

    palAuxiliaryEffectSloti(m_reverbEffect, AL_EFFECTSLOT_EFFECT, m_reverbEffect);
}