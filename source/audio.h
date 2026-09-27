#pragma once
#include "vector.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/efx-presets.h>
#include <map>
#include <string>

class Audio {
public:
    struct Sample {
        unsigned int buffer;
    };

    class Source {
    public:
        Source(Sample* sample);
        ~Source();

        void SetSample(Sample* sample);
        void Play();
        void Stop();
        void Set3D();
        void SetPos(Vector pos);

    private:
        unsigned int m_source;
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
};