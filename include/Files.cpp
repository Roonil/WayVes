#include "Files.h"

void Files::setupShaderFiles(std::map<std::string, std::string> overridesMap, PostProcessingShader* postProcessingShaders, int postProcessingShadersCount)
{
    vertexShaderFiles.loadShaders(shaderProps->shaderName, shaderProps->configFileName, directoryPrefix, ShaderTypes::VERTEX, overridesMap, { });
    fragmentShaderFiles.loadShaders(shaderProps->shaderName, shaderProps->configFileName, directoryPrefix, ShaderTypes::FRAGMENT, overridesMap, { });
    computeShaderFile.loadShaders(shaderProps->shaderName, shaderProps->configFileName, directoryPrefix, ShaderTypes::COMPUTE, overridesMap, { });

    std::map<std::string, int> vars = { };

    ShaderFiles* postProcessingFragmentShaderFilesIterator = &postProcessingFragmentShaderFiles;
    ShaderFiles* postProcessingVertexShaderFilesIterator = &postProcessingVertexShaderFiles;

    for (int i = 0; i < postProcessingShadersCount; i++) {

        int offset = 0;
        std::string postProcessingShaderName = "utils/post-processing/";
        postProcessingShaderName += postProcessingShaders[i].shaderName;

        auto postProcessingPassesIterator = postProcessingPasses.find(postProcessingShaderName);
        if (postProcessingPassesIterator != postProcessingPasses.end())
            offset = postProcessingPasses[postProcessingShaderName];

        else
            postProcessingPasses[postProcessingShaderName] = postProcessingShaders[i].passes;

        int postProcessingNumber = postProcessingShaders[i].passes + offset;

        for (int j = 1; j <= postProcessingNumber; j++) {
            ShaderFiles *postProcessingTempFragmentFile, *postProcessingTempVertexFile;

            if (j == 1 && i == 0) {
                postProcessingTempFragmentFile = &postProcessingFragmentShaderFiles;
                postProcessingTempVertexFile = &postProcessingVertexShaderFiles;
            } else {
                postProcessingFragmentShaderFilesIterator->next = new ShaderFiles;
                postProcessingTempFragmentFile = postProcessingFragmentShaderFilesIterator->next;

                postProcessingVertexShaderFilesIterator->next = new ShaderFiles;
                postProcessingTempVertexFile = postProcessingVertexShaderFilesIterator->next;
            }

            vars["postProcessingNumber"] = j;
            postProcessingTempVertexFile->loadShaders(postProcessingShaderName, shaderProps->configFileName, directoryPrefix, ShaderTypes::VERTEX, overridesMap, vars);
            postProcessingTempFragmentFile->loadShaders(postProcessingShaderName, shaderProps->configFileName, directoryPrefix, ShaderTypes::FRAGMENT, overridesMap, vars);

            while (postProcessingFragmentShaderFilesIterator->next != NULL)
                postProcessingFragmentShaderFilesIterator = postProcessingFragmentShaderFilesIterator->next;

            while (postProcessingVertexShaderFilesIterator->next != NULL)
                postProcessingVertexShaderFilesIterator = postProcessingVertexShaderFilesIterator->next;
        }
    }
}

void Files::setupAudioShaderFiles(AudioOverride* audioOverrides)
{
    std::map<std::string, int> vars = { };
    std::map<std::string, std::string> overridesMap = { };

    vars["adjacentSampleNums"] = audioOverrides->smoothSettings->adjacentSampleNums;
    overridesMap["ROUND_FORMULA"] = audioOverrides->smoothSettings->roundFormula == 0 ? "sinusoidal"
        : audioOverrides->smoothSettings->roundFormula == 1                           ? "linear"
                                                                                      : "circular";

    smoothShaderFile.loadShaders(std::string("utils/audio/smooth"), "", directoryPrefix, ShaderTypes::FRAGMENT, overridesMap, vars);

    overridesMap.clear();
    vars.clear();

    gravityShaderFile.loadShaders("utils/audio/gravity", "", directoryPrefix, ShaderTypes::FRAGMENT, { }, { });

    vars["avgFrames"] = audioOverrides->gravitySettings->averageFrames;

    averageShaderFile.loadShaders("utils/audio/average", "", directoryPrefix, ShaderTypes::FRAGMENT, { }, vars);

    passShaderFile.loadShaders("utils/audio/pass", "", directoryPrefix, ShaderTypes::FRAGMENT, { }, { });
}

void Files::LoadFiles()
{
    setupShaderFiles(overridesMap, shaderProps->postProcessingShaders, shaderProps->postProcessingShaders_count);

    setupAudioShaderFiles(shaderProps->audioOverrides);
}

Files::Files(std::string directoryPrefix, ShaderProps* shaderProps, std::map<std::string, std::string> overridesMap)
{
    this->shaderProps = shaderProps;
    this->directoryPrefix = directoryPrefix;
    this->overridesMap = overridesMap;
}
