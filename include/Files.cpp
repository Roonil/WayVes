#include "Files.h"

void Files::setupShaderFiles(std::map<std::string, std::string> overridesMap, PostProcessingShader *postProcessingShaders, int postProcessingShadersCount)
{
    vertexShaderFiles.LoadShaders(shaderName, configFileName, directoryPrefix, ShaderTypes::VERTEX, overridesMap, {});
    fragmentShaderFiles.LoadShaders(shaderName, configFileName, directoryPrefix, ShaderTypes::FRAGMENT, overridesMap, {});
    computeShaderFile.LoadShaders(shaderName, configFileName, directoryPrefix, ShaderTypes::COMPUTE, overridesMap, {});

    std::map<std::string, int> vars = {};

    ShaderFiles *postProcessingFragmentShaderFilesIterator = &postProcessingFragmentShaderFiles;
    ShaderFiles *postProcessingVertexShaderFilesIterator = &postProcessingVertexShaderFiles;

    for (int i = 0; i < postProcessingShadersCount; i++)
    {

        int offset = 0;
        std::string postProcessingShaderName = "utils/post-processing/";
        postProcessingShaderName += postProcessingShaders[i].shaderName;

        auto postProcessingPassesIterator = postProcessingPasses.find(postProcessingShaderName);
        if (postProcessingPassesIterator != postProcessingPasses.end())
            offset = postProcessingPasses[postProcessingShaderName];

        else
            postProcessingPasses[postProcessingShaderName] = postProcessingShaders[i].passes;

        int postProcessingNumber = postProcessingShaders[i].passes + offset;

        for (int j = 1; j <= postProcessingNumber; j++)
        {
            ShaderFiles *postProcessingTempFragmentFile, *postProcessingTempVertexFile;

            if (j == 1 && i == 0)
            {
                postProcessingTempFragmentFile = &postProcessingFragmentShaderFiles;
                postProcessingTempVertexFile = &postProcessingVertexShaderFiles;
            }
            else
            {
                postProcessingFragmentShaderFilesIterator->next = new ShaderFiles;
                postProcessingTempFragmentFile = postProcessingFragmentShaderFilesIterator->next;

                postProcessingVertexShaderFilesIterator->next = new ShaderFiles;
                postProcessingTempVertexFile = postProcessingVertexShaderFilesIterator->next;
            }

            vars["postProcessingNumber"] = j;
            postProcessingTempVertexFile->LoadShaders(postProcessingShaderName, configFileName, directoryPrefix, ShaderTypes::VERTEX, overridesMap, vars);

            postProcessingTempFragmentFile->LoadShaders(postProcessingShaderName, configFileName, directoryPrefix, ShaderTypes::FRAGMENT, overridesMap, vars);

            while (postProcessingFragmentShaderFilesIterator->next != NULL)
                postProcessingFragmentShaderFilesIterator = postProcessingFragmentShaderFilesIterator->next;

            while (postProcessingVertexShaderFilesIterator->next != NULL)
                postProcessingVertexShaderFilesIterator = postProcessingVertexShaderFilesIterator->next;
        }
    }
}

void Files::setupAudioShaderFiles(AudioOverride *audioOverrides)
{
    std::map<std::string, int> vars = {};
    std::map<std::string, std::string> overridesMap = {};

    vars["adjacentSampleNums"] = audioOverrides->smoothSettings->adjacentSampleNums;
    overridesMap["ROUND_FORMULA"] = audioOverrides->smoothSettings->roundFormula == 0   ? "sinusoidal"
                                    : audioOverrides->smoothSettings->roundFormula == 1 ? "linear"
                                                                                        : "circular";

    smoothShaderFile.LoadShaders(std::string("utils/audio/smooth"), "", directoryPrefix, ShaderTypes::FRAGMENT, overridesMap, vars);

    overridesMap.clear();
    vars.clear();

    gravityShaderFile.LoadShaders("utils/audio/gravity", "", directoryPrefix, ShaderTypes::FRAGMENT, {}, {});

    vars["avgFrames"] = audioOverrides->gravitySettings->averageFrames;

    averageShaderFile.LoadShaders("utils/audio/average", "", directoryPrefix, ShaderTypes::FRAGMENT, {}, vars);

    passShaderFile.LoadShaders("utils/audio/pass", "", directoryPrefix, ShaderTypes::FRAGMENT, {}, {});
}

void Files::LoadFiles(std::map<std::string, std::string> overridesMap, AudioOverride *audioOverrides, PostProcessingShader *postProcessingShaders, int postProcessingShadersCount)
{
    setupShaderFiles(overridesMap, postProcessingShaders, postProcessingShadersCount);

    setupAudioShaderFiles(audioOverrides);
}

Files::Files(std::string shaderName, std::string configFileName, std::string directoryPrefix)
{
    this->shaderName = shaderName;
    this->directoryPrefix = directoryPrefix;
    this->configFileName = configFileName;
}
