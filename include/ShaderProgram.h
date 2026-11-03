#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

// clang-format off
#include <epoxy/gl.h>
#include <vector>
#include <gtk/gtk.h>

#include "YAMLConfig.h"
#include "ShaderStage.h"
#include "AudioShaderStages.h"
#include "ShaderFiles.h"
#include "FilesWatcher.h"
#include "Files.h"
#include "UniformAnimation.h"
#include <memory>
// clang-format on

class ShaderProgram {
private:
    void initializeShaders(GtkGLArea* area);
    void resolveAnimationStart(const std::string& name, UniformAnimation& animation);
    void advanceUniformAnimations();

public:
    bool shouldRender = true, renderAudio = false, audioLoaded = false,
         shouldGLFinish = false, isFirst = false, exportDMABuf = false;
    uint32_t audioConsumerBit;
    int ticks = 0, fps = 60;
    unsigned int prevStageTexture;
    FilesWatcher* filesWatcherInstance = NULL;

    std::vector<std::unique_ptr<DMABufHandler>> dmaBufHandlers;

    GLuint *atomicImageTexture = NULL, *imageTexture = NULL;

    std::map<std::string, std::string> uniformValues = { };
    std::map<std::string, int> uniformTypes = { };
    std::map<std::string, UniformAnimation> uniformAnimations = { };

    ShaderProps* shaderProps = NULL;

    PaintableShaderProps* paintableShaderConfig = NULL;

    Files* files = NULL;

    ShaderStage* startStage = NULL;
    ShaderStage* finalStage = NULL;

    PipeWireHandler* pipeWireSetting;

    AudioShaderStages* audioShaderStages;

    void applyAudioTransformations();
    void loadAudioShaders();

    gboolean realize(GtkGLArea* area);
    gboolean render(GtkGLArea* area);
};

#endif