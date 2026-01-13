#include <algorithm>

#include "ConfigsHandler.h"
#include "GApplicationHandler.h"

bool ConfigsHandler::globalSmoothSettingsMatchShaderSmoothSettings(SmoothSettings *globalSmoothSettings, SmoothSettings *shaderSmoothSettings)
{
    return shaderSmoothSettings->roundFormula == globalSmoothSettings->roundFormula && shaderSmoothSettings->adjacentSampleNums == globalSmoothSettings->adjacentSampleNums && shaderSmoothSettings->smoothFactor == globalSmoothSettings->smoothFactor && shaderSmoothSettings->sampleHybridWeight == globalSmoothSettings->sampleHybridWeight && shaderSmoothSettings->sampleScale == globalSmoothSettings->sampleScale && shaderSmoothSettings->sampleRange == globalSmoothSettings->sampleRange && shaderSmoothSettings->sampleMode == globalSmoothSettings->sampleMode;
}

bool ConfigsHandler::globalGravitySettingsMatchShaderGravitySettings(GravitySettings *globalGravitySettings, GravitySettings *shaderGravitySettings)
{

    return shaderGravitySettings->gravityStep == globalGravitySettings->gravityStep &&
           shaderGravitySettings->averageFrames == globalGravitySettings->averageFrames;
}

bool ConfigsHandler::globalAudioOverridesMatchShaderAudioOverrides(AudioOverride *globalAudioOverride, AudioOverride *shaderAudioOverride)
{
    return shaderAudioOverride->minFrequency == globalAudioOverride->minFrequency &&
           shaderAudioOverride->maxFrequency == globalAudioOverride->maxFrequency && globalGravitySettingsMatchShaderGravitySettings(globalAudioOverride->gravitySettings, shaderAudioOverride->gravitySettings) && globalSmoothSettingsMatchShaderSmoothSettings(globalAudioOverride->smoothSettings, shaderAudioOverride->smoothSettings);
}

void ConfigsHandler::addPaintableWindows(ShaderWindowHandler *windowHandler, ShaderProps *shader)
{
    for (int i = 0; i < shader->paintableShaderProps_count; i++)
    {
        if (windowHandler->paintableWindows == NULL)
            windowHandler->paintableWindows = new PaintableWindowHandler *[shader->paintableShaderProps_count];

        windowHandler->paintableWindows[i] = new PaintableWindowHandler(&shader->paintableShaderProps[i]);
        classNames.push_back(std::string(windowHandler->paintableWindows[i]->shaderProgram.paintableShaderConfig->className));
    }
}

std::map<std::string, std::string> ConfigsHandler::extractOverridesMap(char **shaderOverrides, int shaderOverrides_count, std::string errorContext)
{
    std::map<std::string, std::string> overridesMap = {};
    for (int i = 0; i < shaderOverrides_count; i++)
    {
        std::string overrideString = std::string(shaderOverrides[i]);

        int equalsIdx = overrideString.find("=");
        if (equalsIdx == std::string::npos)
            Errors::throwError("Invalid override format", errorContext, "In");
        std::string variableName = overrideString.substr(0, equalsIdx);
        std::string variableValue = overrideString.substr(equalsIdx + 1);

        variableName.erase(std::remove_if(variableName.begin(), variableName.end(), ::isspace), variableName.end());
        variableValue.erase(std::remove_if(variableValue.begin(), variableValue.end(), ::isspace), variableValue.end());
        overridesMap[variableName] = variableValue;
    }

    return overridesMap;
}

ShaderWindowHandler *ConfigsHandler::getShaderWindow(ShaderProps *currentShader, Audio *currentYAMLAudio, bool *isFirst, std::string directoryPrefix, AudioShaderStages *audioShaderStages)
{
    AudioOverride *globalYAMLAudioOverrides = currentYAMLAudio->audioOverrides;

    ShaderWindowHandler *windowHandler = NULL;

    classNames.push_back(currentShader->className);

    std::string configFileName = currentShader->configFileName;

    std::map<std::string, std::string> overridesMap = extractOverridesMap(currentShader->shaderOverrides, currentShader->shaderOverrides_count, std::string(currentShader->shaderName));

    if (!(globalAudioOverridesMatchShaderAudioOverrides(globalYAMLAudioOverrides, currentShader->audioOverrides)))
    {

        windowHandler = new ShaderWindowHandler(currentShader, currentYAMLAudio->pipewireSettings, new AudioShaderStages(currentShader->audioOverrides->gravitySettings->averageFrames, currentYAMLAudio->pipewireSettings->applyFFT), *isFirst, directoryPrefix, overridesMap);

        windowHandler->shaderProgram.renderAudio = true;
    }
    else
    {
        windowHandler = new ShaderWindowHandler(currentShader, currentYAMLAudio->pipewireSettings, audioShaderStages, *isFirst, directoryPrefix, overridesMap);

        if (*isFirst)
        {
            windowHandler->shaderProgram.renderAudio = true;
            *isFirst = false;
        }
    }

    windowHandler->paintableWindowsCount = currentShader->paintableShaderProps_count;

    addPaintableWindows(windowHandler, currentShader);

    return windowHandler;
}

ConfigsHandler::ConfigsHandler(char *configFileName, std::string directoryPrefix)
{

    this->configFileName = std::string(configFileName == NULL ? "config" : configFileName);
    YAMLConfig *yamlConfig = new YAMLConfig;

    yamlConfig->loadYaml(&yamlConfig, this->configFileName, directoryPrefix);

    configsArray = new Configs *[yamlConfig->audios_count];

    Configs *configs = new Configs, *prevConfigs = NULL;

    configs->totalAudios = yamlConfig->audios_count;

    for (int i = 0; i < yamlConfig->audios_count; i++)
    {
        configsArray[i] = configs;

        Audio *currentYAMLAudio = &yamlConfig->audios[i];
        configs->audioName = std::string(yamlConfig->audios[i].audioName);
        configs->directoryPrefix = directoryPrefix;

        currentYAMLAudio->pipewireSettings->audioName = currentYAMLAudio->audioName;

        AudioShaderStages *audioShaderStages = new AudioShaderStages(currentYAMLAudio->audioOverrides->gravitySettings->averageFrames, currentYAMLAudio->pipewireSettings->applyFFT);

        bool isFirst = true;

        for (int j = 0; j < currentYAMLAudio->shaderProps_count; j++)
        {
            ShaderProps *currentShader = &currentYAMLAudio->shaderProps[j];

            configs->windowHandler = getShaderWindow(currentShader, currentYAMLAudio, &isFirst, directoryPrefix, audioShaderStages);
            configs->windowHandler->audioIdx = i;

            if (prevConfigs == NULL)
                configs->windowHandler->shaderProgram.shouldGLFinish = true;

            prevConfigs = configs;
            configs->next = new Configs;
            configs = configs->next;
        }
        prevConfigs->next = NULL;
    }
    free(configs);
    prevConfigs->next = NULL;
}

void ConfigsHandler::initialiseGTKApps()
{
    for (int i = 0; i < configsArray[0]->totalAudios; i++)
    {
        int pID = fork();
        if (pID != 0)
            continue;

        setpgid(0, getppid());

        configsArray[i]->windowHandler->shaderProgram.pipeWireSetting->createPipeWireThread();

        GApplicationHandler gApplicationHandler(configsArray[i]);
        gApplicationHandler.runApp();

        break;
    }
}