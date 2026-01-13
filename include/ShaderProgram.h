#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <epoxy/gl.h>
#include <vector>
#include <gtk/gtk.h>

#include "YAMLConfig.h"
#include "ShaderStage.h"
#include "AudioShaderStages.h"
#include "ShaderFiles.h"
#include "Files.h"

class ShaderProgram
{
private:
    void initializeShaders(GtkGLArea *area);

public:
    bool shouldRender = true, renderAudio = false, audioLoaded = false,
         shouldGLFinish = false, isFirst = false;

    int ticks = 0, fps = 60;
    unsigned int prevStageTexture;

    GLuint *atomicImageTexture = NULL, *imageTexture = NULL;

    std::map<std::string, std::string> uniformValues = {};
    std::map<std::string, int> uniformTypes = {};

    ShaderProps *shaderProps = NULL;

    PaintableShaderProps *paintableShaderConfig = NULL;

    Files *files = NULL;

    ShaderStage *startStage = NULL;

    PipeWireHandler *pipeWireSetting;

    AudioShaderStages *audioShaderStages;

    void applyAudioTransformations();
    void loadAudioShaders();

    gboolean realize(GtkGLArea *area);
    gboolean render(GtkGLArea *area);
};

#endif