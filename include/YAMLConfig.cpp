
#include <iostream>
#include <map>
#include <signal.h>
#include <string.h>

#include <cctype>
#include "Errors.h"
#include "YAMLConfig.h"
#include <cyaml/cyaml.h>
#include "CYAMLTypes.h"

void YAMLConfig::initialisePipewireSettings(PipeWireHandler *currentPipewireSetting)
{
    currentPipewireSetting->applyFFT = true;
    currentPipewireSetting->audioFormat = (char *)"F32_LE";
    currentPipewireSetting->captureMic = false;
    currentPipewireSetting->targetObject = NULL;
    currentPipewireSetting->channels = 2;
    currentPipewireSetting->fragmentSize = 4096;
    currentPipewireSetting->sampleRate = 22050;
    currentPipewireSetting->sampleSize = 1024;
    currentPipewireSetting->fftScale = 10.2f;
    currentPipewireSetting->fftCutOff = 0.3f;
}

void YAMLConfig::initialiseGravitySettings(GravitySettings *gravitySettings)
{
    gravitySettings->averageFrames = 5;
    gravitySettings->gravityStep = 4.2f;
}

void YAMLConfig::initialiseSmoothSettings(SmoothSettings *smoothSettings)
{
    smoothSettings->sampleHybridWeight = .065f;
    smoothSettings->sampleMode = 0;
    smoothSettings->adjacentSampleNums = 1;
    smoothSettings->roundFormula = 0;
    smoothSettings->sampleRange = .9f;
    smoothSettings->sampleScale = 8.0f;
    smoothSettings->smoothFactor = .025f;
}

void YAMLConfig::initialiseAudioOverride(AudioOverride *currentAudioOverride)
{
    currentAudioOverride->maxFrequency = 22000;
    currentAudioOverride->minFrequency = 0;
    currentAudioOverride->strictFrequencyBounds = new bool;
    *currentAudioOverride->strictFrequencyBounds = false;
    currentAudioOverride->smoothAudio = new bool;
    *currentAudioOverride->smoothAudio = true;
    currentAudioOverride->audioMap = new AudioMap;
    currentAudioOverride->audioMap->left = 0;
    currentAudioOverride->audioMap->right = 1;

    currentAudioOverride->gravitySettings = new GravitySettings;
    initialiseGravitySettings(currentAudioOverride->gravitySettings);

    currentAudioOverride->smoothSettings = new SmoothSettings;
    initialiseSmoothSettings(currentAudioOverride->smoothSettings);
}

void YAMLConfig::initialiseDefaultValues(YAMLConfig *config)
{

    for (int i = 0; i < config->audios_count; i++)
    {
        Audio *currentAudio = &config->audios[i];
        if (currentAudio->pipewireSettings == NULL)
        {
            currentAudio->pipewireSettings = new PipeWireHandler;
            initialisePipewireSettings(currentAudio->pipewireSettings);
        }

        AudioOverride *currentGlobalAudioOverride = currentAudio->audioOverrides;

        if (currentGlobalAudioOverride == NULL)
        {
            currentAudio->audioOverrides = new AudioOverride;
            currentGlobalAudioOverride = currentAudio->audioOverrides;
            initialiseAudioOverride(currentGlobalAudioOverride);
        }

        currentGlobalAudioOverride->maxFrequency = std::min(currentGlobalAudioOverride->maxFrequency, (int)currentAudio->pipewireSettings->sampleRate / 2);

        if (currentGlobalAudioOverride->maxFrequency < currentGlobalAudioOverride->minFrequency)
            Errors::throwError("Max Frequency cannot be smaller than Min Frequency", std::string(currentAudio->audioName), "For");

        if (currentGlobalAudioOverride->audioMap == NULL)
        {
            currentGlobalAudioOverride->audioMap = new AudioMap;
            currentGlobalAudioOverride->audioMap->left = 0;
            currentGlobalAudioOverride->audioMap->right = 1;
        }

        if (currentGlobalAudioOverride->smoothAudio == NULL)
        {
            currentGlobalAudioOverride->smoothAudio = new bool;
            *currentGlobalAudioOverride->smoothAudio = true;
        }

        if (currentGlobalAudioOverride->strictFrequencyBounds == NULL)
        {
            currentGlobalAudioOverride->strictFrequencyBounds = new bool;
            *currentGlobalAudioOverride->strictFrequencyBounds = false;
        }

        if (currentGlobalAudioOverride->smoothSettings == NULL)
        {
            currentGlobalAudioOverride->smoothSettings = new SmoothSettings;
            initialiseSmoothSettings(currentGlobalAudioOverride->smoothSettings);
        }

        if (currentGlobalAudioOverride->gravitySettings == NULL)
        {
            currentGlobalAudioOverride->gravitySettings = new GravitySettings;
            initialiseGravitySettings(currentGlobalAudioOverride->gravitySettings);
        }

        for (int j = 0; j < currentAudio->shaderProps_count; j++)
        {
            ShaderProps *currentShader = &currentAudio->shaderProps[j];

            if (currentShader->className == NULL)
                currentShader->className = currentShader->shaderName;
            if (currentShader->configFileName == NULL)
            {
                char *full_text;
                full_text = new char[strlen(currentShader->shaderName) + strlen(".glsl") + 1];
                strcpy(full_text, currentShader->shaderName);
                strcat(full_text, ".glsl");
                currentShader->configFileName = full_text;
            }
            for (int k = 0; k < currentShader->paintableShaderProps_count; k++)
            {
                PaintableShaderProps *currentPaintableConfig = &currentShader->paintableShaderProps[k];
                if (currentPaintableConfig == NULL)
                    continue;
                if (currentPaintableConfig->className == NULL)
                {
                    char *className = new char[strlen(currentShader->className + 10)];
                    strcpy(className, currentShader->className);
                    strcat(className, "_paintable");

                    currentPaintableConfig->className = className;
                }
            }

            AudioOverride *currentOverridenAudio = currentShader->audioOverrides;
            if (currentOverridenAudio == NULL)
            {
                currentShader->audioOverrides = currentGlobalAudioOverride;
                continue;
            }

            if (currentOverridenAudio->audioMap == NULL)
                currentOverridenAudio->audioMap = currentGlobalAudioOverride->audioMap;

            if (currentOverridenAudio->audioMap->left == -1)
                currentOverridenAudio->audioMap->left = currentGlobalAudioOverride->audioMap->left;

            if (currentOverridenAudio->audioMap->right == -1)
                currentOverridenAudio->audioMap->right = currentGlobalAudioOverride->audioMap->right;

            if (currentOverridenAudio->gravitySettings == NULL)
                currentOverridenAudio->gravitySettings = currentGlobalAudioOverride->gravitySettings;

            if (currentOverridenAudio->gravitySettings->averageFrames == -1)
                currentOverridenAudio->gravitySettings->averageFrames = currentGlobalAudioOverride->gravitySettings->averageFrames;

            if (currentOverridenAudio->gravitySettings->gravityStep == -1)
                currentOverridenAudio->gravitySettings->gravityStep = currentGlobalAudioOverride->gravitySettings->gravityStep;

            if (currentOverridenAudio->maxFrequency == -1)
                currentOverridenAudio->maxFrequency = currentGlobalAudioOverride->maxFrequency;

            if (currentOverridenAudio->minFrequency == -1)
                currentOverridenAudio->minFrequency = currentGlobalAudioOverride->minFrequency;

            if (currentOverridenAudio->smoothAudio == NULL)
                currentOverridenAudio->smoothAudio = currentGlobalAudioOverride->smoothAudio;

            if (currentOverridenAudio->strictFrequencyBounds == NULL)
                currentOverridenAudio->strictFrequencyBounds = currentGlobalAudioOverride->strictFrequencyBounds;

            if (currentOverridenAudio->smoothSettings == NULL)
                currentOverridenAudio->smoothSettings = currentGlobalAudioOverride->smoothSettings;

            if (currentOverridenAudio->smoothSettings->sampleHybridWeight == -1)
                currentOverridenAudio->smoothSettings->sampleHybridWeight = currentGlobalAudioOverride->smoothSettings->sampleHybridWeight;

            if (currentOverridenAudio->smoothSettings->sampleMode == -1)
                currentOverridenAudio->smoothSettings->sampleMode = currentGlobalAudioOverride->smoothSettings->sampleMode;

            if (currentOverridenAudio->smoothSettings->roundFormula == -1)
                currentOverridenAudio->smoothSettings->roundFormula = currentGlobalAudioOverride->smoothSettings->roundFormula;

            if (currentOverridenAudio->smoothSettings->adjacentSampleNums == -1)
                currentOverridenAudio->smoothSettings->adjacentSampleNums = currentGlobalAudioOverride->smoothSettings->adjacentSampleNums;

            if (currentOverridenAudio->smoothSettings->sampleRange == -1)
                currentOverridenAudio->smoothSettings->sampleRange = currentGlobalAudioOverride->smoothSettings->sampleRange;

            if (currentOverridenAudio->smoothSettings->sampleScale == -1)
                currentOverridenAudio->smoothSettings->sampleScale = currentGlobalAudioOverride->smoothSettings->sampleScale;

            if (currentOverridenAudio->smoothSettings->smoothFactor == -1)
                currentOverridenAudio->smoothSettings->smoothFactor = currentGlobalAudioOverride->smoothSettings->smoothFactor;

            if (currentOverridenAudio->maxFrequency < currentOverridenAudio->minFrequency)
                Errors::throwError("Max Frequency cannot be smaller than Min Frequency", std::string(currentAudio->audioName), "For");

            if (currentOverridenAudio->maxFrequency > currentAudio->pipewireSettings->sampleRate / 2)
                Errors::throwError("Max Frequency cannot be greater than SampleRate/2", std::string(currentAudio->audioName), "For");
        }
    }
}

void YAMLConfig::loadYaml(YAMLConfig **config, std::string configFileName, std::string directoryPrefix)
{

    cyaml_err_t err = cyaml_load_file(((std::string(configFileName == "" ? directoryPrefix + "config" : directoryPrefix + configFileName) + std::string(".yaml")).c_str()), &libCYAMLConfig,
                                      &config_schema, (void **)config, NULL);

    if (err != CYAML_OK)
    {

        Errors::throwError(configFileName + ".yaml", std::string(cyaml_strerror(err)));
        return;
    }

    initialiseDefaultValues(*config);
}
