#ifndef AUDIO_SHADER_STAGES_H
#define AUDIO_SHADER_STAGES_H

#include <epoxy/gl.h>
#include <iostream>
#include <map>
#include <string>

class AudioShaderStage : public ShaderStage {
public:
    unsigned int outputLTexture = 0;
    int allocatedRSize = 0, allocatedLSize = 0;
    bool applyFFT = true;

    AudioShaderStage(bool applyFFT);

    void compile(ShaderFiles* vertexShaderFiles, ShaderFiles* fragmentShaderFiles);
    void bindAudio(int offset, int size, char* errorContext);
    void releaseOutputTextures();
};

class AudioShaderStages {
    GLuint create1DTexture();
    uint32_t consumerMask = 0;

public:
    int out_idx = 0, out_idx_l = 0;
    int numGravityFBOs = 5;
    bool applyFFT = true;

    AudioShaderStage *smoothStage = NULL,
                     *gravityStage = NULL,
                     *averageStage = NULL,
                     *passStage = NULL;
    AudioShaderStage** gravityFBOStages = NULL;

    unsigned int audioLTexture = 0, audioRTexture = 0;

    void createAudioTextures();

    void setConsumer(uint32_t bit, bool active);

    bool hasConsumers() const;

    void setApplyFFT(bool applyFFT);

    AudioShaderStages(int numGravityFBOs, bool applyFFT);

    int* applyRangeSelection(float* samples, float* out_data, unsigned int n_samples,
        float min_freq, float max_freq, float sample_rate);

    void reduceRangeForFullSpectrum(float* samples, float* out_data, unsigned int n_samples, float sample_rate);

    void applyGravityPassShader(unsigned int audioTextureSize, int offset,
        std::string locationName, char* errorContext);

    void applySmoothPassShader(unsigned int audioTextureSize, unsigned int adjacentSampleNums, int offset,
        std::string locationName, char* errorContext);

    void updateAudioTextures(int lOffset, size_t lSize, float* lData, int rOffset, size_t rSize, float* rData);
};

#endif