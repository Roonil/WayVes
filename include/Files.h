#ifndef FILES_H
#define FILES_H
#include "CYAMLClasses.h"
#include "ShaderFiles.h"
#include <string>

class Files {
private:
    ShaderProps* shaderProps;
    std::string directoryPrefix;
    std::map<std::string, int> postProcessingPasses = { };
    std::map<std::string, std::string> overridesMap;

    void setupShaderFiles(std::map<std::string, std::string> overridesMap, PostProcessingShader* postProcessingShaders, int postProcessingShadersCount);
    void setupAudioShaderFiles(AudioOverride* audioOverrides);

public:
    void LoadFiles();
    Files(std::string directoryPrefix, ShaderProps* shaderProps, std::map<std::string, std::string> overridesMap);

    ShaderFiles vertexShaderFiles, fragmentShaderFiles, smoothShaderFile, postProcessingVertexShaderFiles, postProcessingFragmentShaderFiles,
        gravityShaderFile, averageShaderFile, passShaderFile, computeShaderFile;
};

#endif