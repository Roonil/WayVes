
#ifndef YAML_CONFIG_H
#define YAML_CONFIG_H

#include "CYAMLClasses.h"

class YAMLConfig
{

public:
    Audio *audios;
    int audios_count;

    void initialisePipewireSettings(PipeWireHandler *currentPipewireSetting);

    void initialiseGravitySettings(GravitySettings *gravitySettings);
    void initialiseSmoothSettings(SmoothSettings *smoothSettings);
    void initialiseAudioOverride(AudioOverride *currentAudioOverride);
    void initialiseDefaultValues(YAMLConfig *config);
    void loadYaml(YAMLConfig **config, std::string configFileName, std ::string directoryPrefix);
};

#endif