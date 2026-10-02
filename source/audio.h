#pragma once
#include "vector.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/efx-presets.h>
#include <map>
#include <string>
#include <vector>

class Audio {
public:
    struct Sample {
        unsigned int buffer;
        float length;
        std::string subtitle;
    };

    class Source {
    public:
        Source(Sample* sample);
        ~Source();

        void SetSample(Sample* sample);
        void Play();
        void Stop();
        bool IsPlaying();
        void Set3D();
        void SetLoop(bool loop);
        void SetPos(Vector pos);

    private:
        unsigned int m_source;
        Sample* m_sample;
    };

    class Sequence {
    public:
        Sequence(Source* source);
        ~Sequence();

        void Play();
        void Stop();
        void AddSample(Sample* sample);
        void Update();
        
    private:
        Source* m_source;
        std::vector<Sample*> m_samples;
        int m_index;
    };

    static Audio* Instance;

    Audio();
    ~Audio();

    Sample* LoadSample(std::string filename);
    void SetListenerPos(Vector pos);
    void SetListenerDir(Vector dir);
    void SetReverb(EFXEAXREVERBPROPERTIES reverb);

private:
    ALCdevice* m_device;
    ALCcontext* m_context;
    unsigned int m_auxSlot;
    unsigned int m_reverbEffect;
    std::map<std::string, Sample*> m_loadedSamples;
    std::map<std::string, std::string> m_subtitles;
};