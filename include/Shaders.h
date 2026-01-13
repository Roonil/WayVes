#ifndef SHADERS_H
#define SHADERS_H

#include <epoxy/gl.h>
#include "Errors.h"

#include <map>
#include <string>
#include <iostream>

enum ShaderTypes
{
    VERTEX,
    FRAGMENT,
    COMPUTE
};

class ShaderCompilationArgs
{
public:
    unsigned int *glProgram = NULL;
    int windowWidth = 0, windowHeight = 0;
    std::map<std::string, int> *uniformLocations = NULL;
    std::map<std::string, std::string> *uniformValues = NULL;
    std::map<std::string, int> *uniformTypes = NULL;
};

class VertexShaderCompilationArgs : public ShaderCompilationArgs
{
public:
    VertexShaderCompilationArgs(int windowWidth, int windowHeight, unsigned int *glProgram,
                                std::map<std::string, int> *uniformLocations,
                                std::map<std::string, std::string> *uniformValues,
                                std::map<std::string, int> *uniformTypes)
    {
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
        this->glProgram = glProgram;
        this->uniformLocations = uniformLocations;
        this->uniformValues = uniformValues;
        this->uniformTypes = uniformTypes;
    }
};

class FragmentShaderCompilationArgs : public ShaderCompilationArgs
{

public:
    int numAtomicTextures, numImageTextures;
    unsigned int *atomicImageTexture, *imageTexture;
    FragmentShaderCompilationArgs(int windowWidth, int windowHeight, unsigned int *glProgram,
                                  std::map<std::string, int> *uniformLocations,
                                  std::map<std::string, std::string> *uniformValues,
                                  std::map<std::string, int> *uniformTypes,
                                  int numAtomicTextures = 0, int numImageTextures = 0, unsigned int *atomicImageTexture = NULL, unsigned int *imageTexture = NULL)
    {
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
        this->glProgram = glProgram;
        this->uniformLocations = uniformLocations;
        this->uniformValues = uniformValues;
        this->uniformTypes = uniformTypes;
        this->numAtomicTextures = numAtomicTextures;
        this->numImageTextures = numImageTextures;
        this->atomicImageTexture = atomicImageTexture;
        this->imageTexture = imageTexture;
    }
};

class ComputeShaderCompilationArgs : public ShaderCompilationArgs
{
public:
    ComputeShaderCompilationArgs(int windowWidth, int windowHeight, unsigned int *glProgram,
                                 std::map<std::string, int> *uniformLocations,
                                 std::map<std::string, std::string> *uniformValues,
                                 std::map<std::string, int> *uniformTypes)
    {
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
        this->glProgram = glProgram;
        this->uniformLocations = uniformLocations;
        this->uniformValues = uniformValues;
        this->uniformTypes = uniformTypes;
    }
};

class Shader
{
protected:
    virtual void compileShaderSource(

        ShaderCompilationArgs *args) = 0;

    void checkCompileErrors(std::string type);

public:
    int windowWidth = 0, windowHeight = 0;
    unsigned int *glProgram = NULL, shaderObject = 0;
    std::string shaderSource = "";
};

class VertexShader : public Shader
{
private:
    const std::string defaultVertexShaderSource = "#version 330 core\n"
                                                  "layout(location = 0) in vec3 aPos;\n"
                                                  "void main()\n"
                                                  "{\n"
                                                  "    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0);\n"
                                                  "}";

protected:
    void compileShaderSource(

        ShaderCompilationArgs *args);

public:
    unsigned int vertexArrayObject = 0, vertexBufferObject = 0;

    const GLfloat buf[18] = {-1, -1, 0, 1, -1, 0, -1, 1, 0,
                             1, 1, 0, 1, -1, 0, -1, 1, 0};

    VertexShader(std::string shaderSource,

                 ShaderCompilationArgs *args);
    void draw(unsigned int *texture = NULL);

    ~VertexShader();
};

class FragmentShader : public Shader
{
protected:
    void compileShaderSource(

        ShaderCompilationArgs *args);

public:
    unsigned int frameBufferObject = 0, outputTexture = 0;

    void bind2DTextureToFrameBuffer(char *errorContext)
    {

        glGenFramebuffers(1, &frameBufferObject);
        glGenTextures(1, &outputTexture);
        glBindTexture(GL_TEXTURE_2D, outputTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, windowWidth, windowHeight, 0, GL_BGRA,
                     GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObject);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               outputTexture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            Errors::throwError("an error occured while binding texture to framebuffer", std::string(errorContext), std::string("In"));
            exit(0);
        }
    }

    FragmentShader()
    {
    }

    FragmentShader(std::string shaderSource,

                   ShaderCompilationArgs *args);

    void updateUniforms(int windowWidth, int windowHeight, int left, int right, float ticks, unsigned int audioLSize, unsigned int audioLTexture, unsigned int audioRSize, unsigned int audioRTexture, std::map<std::string, std::string> uniformValues, std::map<std::string, int> uniformTypes,
                        std::map<std::string, int> uniformLocations);
    ~FragmentShader();
};

class GravityShader : public FragmentShader
{

public:
    unsigned int outputLTexture = 0;
    ~GravityShader();
};

class ComputeShader : public Shader
{
protected:
    void compileShaderSource(

        ShaderCompilationArgs *args);

public:
    unsigned int outputTexture = 0;

    ComputeShader()
    {
    }

    ComputeShader(std::string shaderSource,
                  ShaderCompilationArgs *args);

    ~ComputeShader();
};

#endif