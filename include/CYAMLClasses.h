#ifndef CYAML_CLASSES_H
#define CYAML_CLASSES_H

#include "PaintableShaderProps.h"
#include "PipeWireHandler.h"
#include "PostProcessingShader.h"
#include <gtk/gtk.h>
#include <iostream>

class GravitySettings {
public:
    int averageFrames;
    float gravityStep;
};

class SmoothSettings {
public:
    int sampleMode, adjacentSampleNums, roundFormula;
    float sampleHybridWeight, sampleScale, sampleRange, smoothFactor;
};

class AudioMap {
public:
    int left = 0, right = 1;
};

class AudioOverride {
public:
    bool *smoothAudio = NULL, *strictFrequencyBounds = NULL;
    int minFrequency = -1, maxFrequency = -1;
    AudioMap* audioMap = NULL;
    SmoothSettings* smoothSettings = NULL;
    GravitySettings* gravitySettings = NULL;
};

class ShaderProps : public PaintableShaderProps {
public:
    char *shaderName = NULL, *configFileName = NULL;
    bool hotReload;
    char** shaderOverrides = NULL;
    int shaderOverrides_count;

    unsigned int fps;
    uint16_t atomicTextures, imageTextures;

    enum renderingModes renderingMode;

    char** dmaBufExportNodes = NULL;
    int dmaBufExportNodes_count;

    PaintableShaderProps* paintableShaderProps = NULL;
    int paintableShaderProps_count;

    PostProcessingShader* postProcessingShaders = NULL;
    int postProcessingShaders_count;

    AudioOverride* audioOverrides = NULL;
};

class Audio {
public:
    char* audioName = NULL;

    PipeWireHandler* pipewireSettings = NULL;
    AudioOverride* audioOverrides = NULL;

    ShaderProps* shaderProps;
    int shaderProps_count;
};

#endif