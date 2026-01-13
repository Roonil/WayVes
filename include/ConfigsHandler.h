#ifndef CONFIGS_HANDLER_H
#define CONFIGS_HANDLER_H

#include <string>
#include <map>
#include <vector>

#include "Configs.h"

class ConfigsHandler
{
private:
    std::string configFileName;

    bool globalAudioOverridesMatchShaderAudioOverrides(AudioOverride *globalAudioOverride, AudioOverride *shaderAudioOverride);
    bool globalGravitySettingsMatchShaderGravitySettings(GravitySettings *globalGravitySettings, GravitySettings *shaderGravitySettings);
    bool globalSmoothSettingsMatchShaderSmoothSettings(SmoothSettings *globalSmoothSettings, SmoothSettings *shaderSmoothSettings);

    std::map<std::string, std::string> extractOverridesMap(char **shaderOverrides, int shaderOverrides_count, std::string errorContext);

    void addPaintableWindows(ShaderWindowHandler *windowHandler, ShaderProps *shader);
    ShaderWindowHandler *getShaderWindow(ShaderProps *currentShader, Audio *currentYAMLAudio, bool *isFirst, std::string directoryPrefix, AudioShaderStages *audioShaderStages);

public:
    Configs **configsArray = NULL;
    std::vector<std::string> classNames;

    ConfigsHandler(char *configFileName, std::string directoryPrefix);
    void initialiseGTKApps();
};

#endif