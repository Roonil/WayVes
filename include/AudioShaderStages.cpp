#include "ShaderStage.h"
#include "AudioShaderStages.h"

void AudioShaderStage::compile(std::string vertexShaderSource, std::string fragmentShaderSource)
{
    VertexShaderCompilationArgs *vertexArgs = new VertexShaderCompilationArgs(0, 0, &glProgram, &uniformLocations, NULL, NULL);
    vertexShader = new VertexShader(vertexShaderSource, vertexArgs);

    std::map<std::string, std::string> uniformValues = {};
    std::map<std::string, int> uniformTypes = {};

    FragmentShaderCompilationArgs *args = new FragmentShaderCompilationArgs(0, 0, &glProgram, &uniformLocations, &uniformValues, &uniformTypes);

    fragmentShader = new FragmentShader(fragmentShaderSource, args);
}

AudioShaderStage::AudioShaderStage(bool applyFFT)
{
    this->applyFFT = applyFFT;
}

void AudioShaderStage::bindAudio(int offset, int size, char *errorContext)
{
    unsigned int *outputTexture = 0;
    outputTexture = offset == 1 ? &fragmentShader->outputTexture : &outputLTexture;

    if (outputTexture == NULL || *outputTexture == 0)
    {
        glGenTextures(1, outputTexture);

        if (fragmentShader->frameBufferObject == 0)
            glGenFramebuffers(1, &fragmentShader->frameBufferObject);

        glBindTexture(GL_TEXTURE_1D, *outputTexture);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

        glTexImage1D(GL_TEXTURE_1D, 0, applyFFT ? GL_R16 : GL_R16_SNORM, size, 0, GL_RED, GL_FLOAT, NULL);

        glBindFramebuffer(GL_FRAMEBUFFER, fragmentShader->frameBufferObject);
        glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_1D, *outputTexture, 0);

        switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
        {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        default:
            Errors::throwError("an error occured while binding audio framebuffer", errorContext, "In");
        }
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fragmentShader->frameBufferObject);
        glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_1D, *outputTexture, 0);
    }
}

AudioShaderStages::AudioShaderStages(int numGravityFBOs, bool applyFFT)
{
    this->numGravityFBOs = numGravityFBOs;
    this->applyFFT = applyFFT;
    this->gravityFBOStages = new AudioShaderStage *[numGravityFBOs];
    smoothStage = new AudioShaderStage(applyFFT);
    averageStage = new AudioShaderStage(applyFFT);
    passStage = new AudioShaderStage(applyFFT);
    gravityStage = new AudioShaderStage(applyFFT);

    for (int i = 0; i < numGravityFBOs; i++)
    {
        gravityFBOStages[i] = new AudioShaderStage(applyFFT);
        gravityFBOStages[i]->fragmentShader = new FragmentShader();
    }
}

GLuint AudioShaderStages::create1DTexture()
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    return tex;
}

void AudioShaderStages::createAudioTextures()
{
    audioLTexture = create1DTexture();
    audioRTexture = create1DTexture();
}

void AudioShaderStages::reduceRangeForFullSpectrum(float *samples, float *out_data, unsigned int n_samples, float sample_rate)
{
    for (int i = 0; i < n_samples / 2; i++)
    {
        out_data[2 * i] = samples[i];
        out_data[2 * i + 1] = samples[i];
    }
}

int *AudioShaderStages::applyRangeSelection(float *samples, float *out_data, unsigned int n_samples,
                                            float min_freq, float max_freq, float sample_rate)
{

    float bin_width = 2 * sample_rate / (n_samples);
    int min_bin = (int)(min_freq / bin_width);
    int max_bin = (int)(max_freq / bin_width);

    min_bin = std::max(min_bin, 0);
    max_bin = std::min(max_bin, (int)n_samples / 4 - 1);

    for (int i = 0; i <= n_samples / 4; i++)
    {
        if (i < min_bin || i > max_bin)
        {
            out_data[2 * i] = 0;
            out_data[2 * i + 1] = 0;

            out_data[n_samples - 2 * i - 1] = 0;
            out_data[n_samples - 2 * i - 2] = 0;
        }
        else
        {
            out_data[2 * i] = samples[2 * i];
            out_data[2 * i + 1] = samples[2 * i + 1];

            out_data[n_samples - 2 * i - 1] = samples[n_samples - 2 * i - 1];
            out_data[n_samples - 2 * i - 2] = samples[n_samples - 2 * i - 2];
        }
    }

    int *binValues = new int[2];
    binValues[0] = min_bin;
    binValues[1] = max_bin;

    return binValues;
}

// Portions adapted from GLava's Gravity and Averaging Shaders by jarcode-foss
// Licensed under GPL-3.0

void AudioShaderStages::applyGravityPassShader(unsigned int audioTextureSize, int offset,
                                               std::string locationName, char *errorContext)
{

    if (applyFFT)
    {
        passStage->bindAudio(offset, audioTextureSize, errorContext);

        glUseProgram(passStage->glProgram);

        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_1D, offset == 1
                                         ? audioRTexture
                                         : audioLTexture);
        glUniform1i(passStage->uniformLocations[locationName],
                    offset);

        glEnable(GL_BLEND);
        glBlendEquation(GL_MAX);
        glViewport(0, 0, audioTextureSize, 1);
        passStage
            ->vertexShader->draw();

        glBlendEquation(GL_FUNC_ADD);
        glDisable(GL_BLEND);

        glTextureBarrierNV();

        glUseProgram(gravityStage->glProgram);

        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_1D,
                      offset == 1 ? passStage->fragmentShader
                                        ->outputTexture
                                  : passStage->outputLTexture);
        glUniform1i(gravityStage
                        ->uniformLocations[locationName],
                    offset);

        glViewport(0, 0, audioTextureSize, 1);
        gravityStage->vertexShader->draw();
    }
    else
    {
        passStage->fragmentShader->outputTexture = audioRTexture;
        passStage->outputLTexture = audioLTexture;
    }

    gravityFBOStages[offset == 1
                         ? out_idx
                         : out_idx_l]
        ->bindAudio(offset, audioTextureSize, errorContext);

    glUseProgram(passStage->glProgram);

    glActiveTexture(GL_TEXTURE0 + offset);
    glBindTexture(GL_TEXTURE_1D,
                  offset == 1 ? passStage->fragmentShader
                                    ->outputTexture
                              : passStage
                                    ->outputLTexture);
    glUniform1i(passStage->uniformLocations[locationName],
                offset);

    glViewport(0, 0, audioTextureSize, 1);

    passStage->vertexShader->draw();
    averageStage->bindAudio(offset, audioTextureSize, errorContext);

    glUseProgram(averageStage->glProgram);

    for (int t = 0; t < numGravityFBOs; ++t)
    {
        GLuint c_off = offset + 1 + t;

        std::string a = "audioR";
        a += std::to_string(t);

        int fr = (offset == 1 ? out_idx
                              : out_idx_l) -
                 t;
        if (fr < 0)
            fr = numGravityFBOs + fr;

        glActiveTexture(GL_TEXTURE0 + c_off);
        glBindTexture(
            GL_TEXTURE_1D,
            offset == 1 ? gravityFBOStages[fr]->fragmentShader->outputTexture
                        : gravityFBOStages[fr]->outputLTexture);
        GLuint audioLoc = averageStage->uniformLocations[(char *)&a[0]];
        glUniform1i(audioLoc, c_off);
    }

    glViewport(0, 0, audioTextureSize, 1);

    averageStage->vertexShader->draw();
    offset == 1 ? ++out_idx : ++out_idx_l;
    if (out_idx >= numGravityFBOs)
        out_idx = 0;
    if (out_idx_l >= numGravityFBOs)
        out_idx_l = 0;

    glEnable(GL_BLEND);
    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, 0);
}

// Portions adapted from GLava's Audio Smoothing Shader by jarcode-foss
// Licensed under GPL-3.0

void AudioShaderStages::applySmoothPassShader(unsigned int audioTextureSize, unsigned int adjacentSampleNums, int offset,
                                              std::string locationName, char *errorContext)
{

    smoothStage->bindAudio(offset, audioTextureSize, errorContext);

    glUseProgram(smoothStage->glProgram);
    int uniformLoc = smoothStage->uniformLocations["audioRSize"];
    glUniform1i(uniformLoc, audioTextureSize);

    uniformLoc = smoothStage->uniformLocations["adjacentSampleNums"];
    glUniform1i(uniformLoc, adjacentSampleNums);

    glActiveTexture(GL_TEXTURE0 + offset);
    glBindTexture(GL_TEXTURE_1D,
                  offset == 1
                      ? averageStage
                            ->fragmentShader->outputTexture
                      : averageStage
                            ->outputLTexture);
    glUniform1i(smoothStage
                    ->uniformLocations[locationName],
                offset);

    glDisable(GL_BLEND);
    glViewport(0, 0, audioTextureSize, 1);

    smoothStage->vertexShader->draw();
    glEnable(GL_BLEND);

    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, 0);
}

void AudioShaderStages::updateAudioTextures(int lOffset, size_t lSize, float *lData, int rOffset, size_t rSize, float *rData)
{

    glBindTexture(GL_TEXTURE_1D, audioLTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, applyFFT ? GL_R16 : GL_R16_SNORM, lSize, 0, GL_RED, GL_FLOAT, lData + lOffset);
    glBindTexture(GL_TEXTURE_1D, 0);

    glBindTexture(GL_TEXTURE_1D, audioRTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, applyFFT ? GL_R16 : GL_R16_SNORM, rSize, 0, GL_RED, GL_FLOAT, rData + rOffset);
    glBindTexture(GL_TEXTURE_1D, 0);
}
