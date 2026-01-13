#include "ShaderFiles.h"
#include "CYAMLClasses.h"
#include <string>

class Files
{
private:
    std::string shaderName, configFileName, directoryPrefix;
    std::map<std::string, int> postProcessingPasses = {};

    void setupShaderFiles(std::map<std::string, std::string> overridesMap, PostProcessingShader *postProcessingShaders, int postProcessingShadersCount);
    void setupAudioShaderFiles(AudioOverride *audioOverrides);

public:
    void LoadFiles(std::map<std::string, std::string> overridesMap, AudioOverride *audioOverrides, PostProcessingShader *postProcessingShaders, int postProcessingShadersCount);
    Files(std::string shaderName, std::string configFileName, std::string directoryPrefix);

    ShaderFiles vertexShaderFiles, fragmentShaderFiles, smoothShaderFile, postProcessingVertexShaderFiles, postProcessingFragmentShaderFiles,
        gravityShaderFile, averageShaderFile, passShaderFile, computeShaderFile;
};