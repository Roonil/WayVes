#ifndef SHADER_STAGE_H
#define SHADER_STAGE_H

#include "Shaders.h"

class ShaderStage
{
public:
    unsigned int glProgram = 0;
    FragmentShader *fragmentShader = NULL;
    VertexShader *vertexShader = NULL;
    ComputeShader *computeShader = NULL;

    /// A Map to hold locations for uniforms in the current shader stage.
    std::map<std::string, int> uniformLocations = {{"audioR", -1},
                                                   {"audioL", -1}};

    ShaderStage *next = NULL;

    ~ShaderStage()
    {
        if (computeShader != NULL)
            delete computeShader;
        if (fragmentShader != NULL)
            delete fragmentShader;
        if (vertexShader != NULL)
            delete vertexShader;
        next = NULL;
    }
};

#endif