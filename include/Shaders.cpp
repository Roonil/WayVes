#include "Shaders.h"

#include <sstream>

VertexShader::VertexShader(std::string shaderSource, ShaderCompilationArgs *args)
{
    this->glProgram = args->glProgram;
    if (glProgram == NULL)
    {
        this->glProgram = new unsigned int;
        *this->glProgram = 0;
    }
    this->windowWidth = args->windowWidth;
    this->windowHeight = args->windowHeight;

    this->shaderSource = (shaderSource.empty() ? defaultVertexShaderSource : shaderSource);

    compileShaderSource(args);
}

void Shader::checkCompileErrors(std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderObject, 1024, NULL, infoLog);
            Errors::throwError("ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- ");
        }
    }
    else
    {
        glGetProgramiv(*glProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(*glProgram, 1024, NULL, infoLog);
            Errors::throwError("ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- ");
        }
    }
}

void VertexShader::compileShaderSource(

    ShaderCompilationArgs *args)
{

    char *charShaderSource = (char *)(shaderSource.c_str());

    shaderObject = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaderObject, 1, &charShaderSource,
                   NULL);

    glCompileShader(shaderObject);
    checkCompileErrors("VERTEX");

    if (*glProgram == 0)
        *glProgram = glCreateProgram();

    glAttachShader(*glProgram, shaderObject);

    glLinkProgram(*glProgram);
    checkCompileErrors("PROGRAM");

    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glDisableVertexAttribArray(0);

    glBindVertexArray(0);
    glUseProgram(0);

    if (args != NULL)
        delete args;
}

void VertexShader::draw(unsigned int *texture)
{
    glBindVertexArray(vertexArrayObject);
    if (texture != NULL)
        glBindTexture(GL_TEXTURE_2D, *texture);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

VertexShader::~VertexShader()
{
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteShader(shaderObject);
    if (glProgram != NULL)
        glDeleteProgram(*glProgram);
}

FragmentShader::
    FragmentShader(std::string shaderSource,
                   ShaderCompilationArgs *args)
{
    this->glProgram = args->glProgram;
    if (glProgram == NULL)
    {
        this->glProgram = new unsigned int;
        *this->glProgram = 0;
    }
    this->windowWidth = args->windowWidth;
    this->windowHeight = args->windowHeight;
    this->shaderSource = shaderSource;
    compileShaderSource(args);
}

void FragmentShader::compileShaderSource(

    ShaderCompilationArgs *args)
{

    char *charShaderSource = (char *)shaderSource.c_str();

    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(shaderObject, 1, &charShaderSource,
                   NULL);

    glCompileShader(shaderObject);
    checkCompileErrors("FRAGMENT");

    if (*glProgram == 0)
        *glProgram = glCreateProgram();

    glAttachShader(*glProgram, shaderObject);

    glLinkProgram(*glProgram);
    checkCompileErrors("PROGRAM");

    glUseProgram(*glProgram);

    int count;
    glGetProgramiv(*glProgram, GL_ACTIVE_UNIFORMS, &count);

    GLchar name[512];
    int length, size;
    GLenum type;

    for (int i = 0; i < count; i++)
    {
        glGetActiveUniform(*glProgram, (GLuint)i, 512, &length, &size, &type, name);
        (*args->uniformLocations)[name] = i;
        (*args->uniformTypes)[name] = type;
    }

    glBindFragDataLocation(*glProgram, 1, "FragColor");

    int numAtomicTextures = 0;

    int numImageTextures = 0;

    unsigned int *atomicImageTexture = NULL;
    unsigned int *imageTexture = NULL;

    if (args != NULL && ((FragmentShaderCompilationArgs *)args)->atomicImageTexture != NULL)
    {
        numAtomicTextures = ((FragmentShaderCompilationArgs *)args)->numAtomicTextures;

        atomicImageTexture = ((FragmentShaderCompilationArgs *)args)->atomicImageTexture;
    }

    if (args != NULL && ((FragmentShaderCompilationArgs *)args)->imageTexture != NULL)
    {
        numImageTextures = ((FragmentShaderCompilationArgs *)args)->numImageTextures;
        imageTexture = ((FragmentShaderCompilationArgs *)args)->imageTexture;
    }

    unsigned int imageUnit = 0;

    for (int i = 0; i < numAtomicTextures; i++)
    {

        glGenTextures(1, &atomicImageTexture[i]);
        glBindTexture(GL_TEXTURE_2D, atomicImageTexture[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, windowWidth, windowHeight);

        glBindImageTexture(imageUnit, atomicImageTexture[i], 0, GL_FALSE, 0,
                           GL_READ_WRITE, GL_R32UI);

        auto uniformLocationsIterator = (*args->uniformLocations).find(std::string("atomicImageTexture" + std::to_string(i)).c_str());
        if (uniformLocationsIterator == (*args->uniformLocations).end())
        {
            imageUnit++;
            continue;
        }
        GLint imageLocation = uniformLocationsIterator->second;

        glProgramUniform1i(*glProgram, imageLocation, imageUnit);
        glBindTextureUnit(0, atomicImageTexture[i]);

        imageUnit++;
    }

    for (int i = 0; i < numImageTextures; i++)
    {

        glGenTextures(1, &imageTexture[i]);
        glBindTexture(GL_TEXTURE_2D, imageTexture[i]);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, windowWidth, windowHeight);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindImageTexture(imageUnit, imageTexture[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        auto uniformLocationsIterator = (*args->uniformLocations).find(std::string("imageTexture" + std::to_string(i)).c_str());
        if (uniformLocationsIterator == (*args->uniformLocations).end())
        {
            imageUnit++;
            continue;
        }
        GLint imageLocation = uniformLocationsIterator->second;
        glProgramUniform1i(*glProgram, imageLocation, imageUnit);
        glBindTextureUnit(0, imageTexture[i]);

        imageUnit++;
    }

    glUseProgram(0);

    if (args != NULL)
        delete args;
}

// 0: bool, 1: num
int detectType(std::string value)
{

    std::string::size_type startIdx = value.find("false", 0);
    if (startIdx != std::string::npos)
        return 0;

    startIdx = value.find("true", 0);
    if (startIdx != std::string::npos)
        return 1;

    startIdx = value.find("rgb", 0);
    if (startIdx != std::string::npos)
        return 2;

    return 3;
}

void parseValue(std::string value, double *numArray, bool *boolArray)
{

    int type = detectType(value);

    std::string::size_type startIdx = value.find("(", 0);

    std::string::size_type endIdx = value.find(")", startIdx);

    std::string sanitisedValue = value.substr(startIdx + 1, endIdx - startIdx - 1);

    std::stringstream valueStream;
    std::string valueQuantum;
    valueStream << sanitisedValue;

    int idx = 0;
    while (std::getline(valueStream, valueQuantum, ','))
    {
        if (type == 2)
            numArray[idx] = std::stod(valueQuantum) / 255.0;
        else if (type == 3)
            numArray[idx] = std::stod(valueQuantum);
        else
            boolArray[idx] = (type);

        idx++;
    }
}

void FragmentShader::updateUniforms(int windowWidth, int windowHeight, int leftAudio, int rightAudio, float ticks, unsigned int audioLSize, unsigned int audioLTexture, unsigned int audioRSize, unsigned int audioRTexture, std::map<std::string, std::string> uniformValues,
                                    std::map<std::string, int> uniformTypes,
                                    std::map<std::string, int> uniformLocations)
{

    auto uniformLocationsIterator = uniformLocations.find("time");
    if (uniformLocationsIterator != uniformLocations.end())
        glUniform1f(uniformLocationsIterator->second, (float)ticks);

    uniformLocationsIterator = uniformLocations.find("resolution");
    if (uniformLocationsIterator != uniformLocations.end())
        glUniform2f(uniformLocationsIterator->second, windowWidth, windowHeight);

    uniformLocationsIterator = uniformLocations.find("audioRSize");
    if (uniformLocationsIterator != uniformLocations.end())
        glUniform1i(uniformLocationsIterator->second, rightAudio == 1 ? audioRSize : audioLSize);

    uniformLocationsIterator = uniformLocations.find("audioLSize");
    if (uniformLocationsIterator != uniformLocations.end())
        glUniform1i(uniformLocationsIterator->second, leftAudio == 0 ? audioLSize : audioRSize);

    uniformLocationsIterator = uniformLocations.find("audioR");
    if (uniformLocationsIterator != uniformLocations.end())

    {
        glActiveTexture(GL_TEXTURE0 + 1);

        glBindTexture(GL_TEXTURE_1D, rightAudio == 1 ? audioRTexture : audioLTexture);
        glUniform1i(uniformLocationsIterator->second, 1);

        glActiveTexture(GL_TEXTURE0);
    }

    uniformLocationsIterator = uniformLocations.find("audioL");
    if (uniformLocationsIterator != uniformLocations.end())
    {
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_1D, leftAudio == 0 ? audioLTexture : audioRTexture);
        glUniform1i(uniformLocationsIterator->second, 2);

        glActiveTexture(GL_TEXTURE0);
    }

    if (uniformValues.size() == 0)
        return;

    for (auto uniformsIterator = uniformValues.begin();
         uniformsIterator != uniformValues.end();
         uniformsIterator++)

    {
        std::string variableName = uniformsIterator->first;
        std::string variableValue = uniformValues[variableName];

        int uniformLoc = uniformLocations[variableName];
        int type = uniformTypes[variableName];

        double numArray[4] = {0, 0, 0, 0};
        bool boolArray[4] = {0, 0, 0, 0};

        parseValue(variableValue, numArray, boolArray);

        switch (type)
        {

        case GL_FLOAT_VEC4:
        {
            glUniform4f(uniformLoc, (float)numArray[0], (float)numArray[1], (float)numArray[2], (float)numArray[3]);
            break;
        }
        case GL_INT_VEC4:
        {

            glUniform4i(uniformLoc, (int)numArray[0], (int)numArray[1], (int)numArray[2], (int)numArray[3]);
            break;
        }
        case GL_DOUBLE_VEC4:
        {

            glUniform4d(uniformLoc, numArray[0], numArray[1], numArray[2], numArray[3]);
            break;
        }

        case GL_BOOL_VEC4:
        {
            glUniform4f(uniformLoc, (float)boolArray[0], (float)boolArray[1], (float)boolArray[2], (float)boolArray[3]);
            break;
        }

        case GL_FLOAT_VEC3:
        {
            glUniform3f(uniformLoc, (float)numArray[0], (float)numArray[1], (float)numArray[2]);
            break;
        }
        case GL_INT_VEC3:
        {

            glUniform3i(uniformLoc, (int)numArray[0], (int)numArray[1], (int)numArray[2]);
            break;
        }
        case GL_DOUBLE_VEC3:
        {

            glUniform3d(uniformLoc, numArray[0], numArray[1], numArray[2]);
            break;
        }
        case GL_BOOL_VEC3:
        {
            glUniform3f(uniformLoc, (float)boolArray[0], (float)boolArray[1], (float)boolArray[2]);
            break;
        }

        case GL_FLOAT_VEC2:
        {
            glUniform2f(uniformLoc, (float)numArray[0], (float)numArray[1]);
            break;
        }
        case GL_INT_VEC2:
        {

            glUniform2i(uniformLoc, (int)numArray[0], (int)numArray[1]);
            break;
        }
        case GL_DOUBLE_VEC2:
        {

            glUniform2d(uniformLoc, numArray[0], numArray[1]);
            break;
        }

        case GL_BOOL_VEC2:
        {
            glUniform2f(uniformLoc, (float)boolArray[0], (float)boolArray[1]);
            break;
        }
        case GL_FLOAT:
        {
            glUniform1f(uniformLoc, (float)numArray[0]);
            break;
        }
        case GL_INT:
        {

            glUniform1i(uniformLoc, (int)numArray[0]);
            break;
        }
        case GL_DOUBLE:
        {
            glUniform1d(uniformLoc, numArray[0]);
            break;
        }

        case GL_BOOL:
        {
            glUniform1f(uniformLoc, (float)boolArray[0]);
            break;
        }

        break;

        default:
            break;
        }
    }

    uniformValues.clear();
}

FragmentShader::~FragmentShader()
{
    glDeleteShader(shaderObject);
    glDeleteTextures(1, &outputTexture);
    glDeleteFramebuffers(1, &frameBufferObject);
    if (glProgram != NULL)
        glDeleteProgram(*glProgram);
}

GravityShader::~GravityShader()
{
    glDeleteShader(shaderObject);
    glDeleteTextures(1, &outputTexture);
    glDeleteTextures(1, &outputLTexture);
    glDeleteFramebuffers(1, &frameBufferObject);
    if (glProgram != NULL)
        glDeleteProgram(*glProgram);
}

ComputeShader::ComputeShader(std::string shaderSource, ShaderCompilationArgs *args)
{
    this->glProgram = args->glProgram;
    if (glProgram == NULL)
    {
        this->glProgram = new unsigned int;
        *this->glProgram = 0;
    }
    this->windowWidth = args->windowWidth;
    this->windowHeight = args->windowHeight;
    this->shaderSource = shaderSource;

    compileShaderSource(args);
}

void ComputeShader::compileShaderSource(

    ShaderCompilationArgs *args)
{

    char *charShaderSource = (char *)this->shaderSource.c_str();

    shaderObject = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shaderObject, 1, &charShaderSource, NULL);
    glCompileShader(shaderObject);
    checkCompileErrors("COMPUTE");

    if (*glProgram == 0)
        *glProgram = glCreateProgram();

    glAttachShader(*glProgram, shaderObject);
    glLinkProgram(*glProgram);
    checkCompileErrors("PROGRAM");
    // cleanup
    glDeleteShader(shaderObject);

    // create input/output textures
    glGenTextures(1, &outputTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, outputTexture);

    // turns out we need this. huh.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // create empty texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, windowWidth, windowHeight, 0, GL_RED, GL_FLOAT, NULL);
    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    glUseProgram(0);

    if (args != NULL)
        delete args;
}

ComputeShader::~ComputeShader()
{
    glDeleteShader(shaderObject);
    glDeleteTextures(1, &outputTexture);
    if (glProgram != NULL)
        glDeleteProgram(*glProgram);
}