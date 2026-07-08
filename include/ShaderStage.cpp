#include "ShaderStage.h"

bool ShaderStage::shouldSyncFiles()
{
    return ((fragmentShaderFile != NULL && fragmentShaderFile->fileIsDirty.load()) || (vertexShaderFile != NULL && vertexShaderFile->fileIsDirty.load()));
}

bool ShaderStage::syncFiles(VertexShaderCompilationArgs* vertexArgs, FragmentShaderCompilationArgs* fragmentArgs)
{
    if (fragmentShaderFile != NULL && fragmentShaderFile->fileIsDirty.load()) {
        fragmentShaderFile->fileIsDirty.exchange(false);
        fragmentShaderFile->reloadFile();
    }

    if (vertexShaderFile != NULL && vertexShaderFile->fileIsDirty.load()) {
        vertexShaderFile->fileIsDirty.exchange(false);
        vertexShaderFile->reloadFile();
    }

    unsigned int tempProgram = 0;

    vertexArgs->glProgram = &tempProgram;
    vertexArgs->uniformLocations = &uniformLocations;

    fragmentArgs->glProgram = &tempProgram;
    fragmentArgs->uniformLocations = &uniformLocations;

    VertexShader* tempVertexShader = new VertexShader(vertexShaderFile == NULL ? "" : vertexShaderFile->fileContent, vertexArgs);

    if (!tempVertexShader->isValid) {
        delete tempVertexShader;
        return false;
    }

    FragmentShader* tempFragmentShader = new FragmentShader(fragmentShaderFile == NULL ? "" : fragmentShaderFile->fileContent, fragmentArgs);
    if (!tempFragmentShader->isValid) {
        delete tempFragmentShader;
        return false;
    }

    delete vertexShader;
    delete fragmentShader;
    glDeleteProgram(glProgram);

    vertexShader = tempVertexShader;
    fragmentShader = tempFragmentShader;
    glProgram = tempProgram;

    return true;
}