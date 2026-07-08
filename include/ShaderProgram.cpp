#include <epoxy/gl.h>

#include "ShaderProgram.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iomanip>

void ShaderProgram::initializeShaders(GtkGLArea* area)
{
    startStage = new ShaderStage;

    ShaderStage* currentStage = startStage;

    ShaderFiles* fragmentShaderFilesIterator = &files->fragmentShaderFiles;
    ShaderFiles* vertexShaderFilesIterator = &files->vertexShaderFiles;
    ShaderFiles* postProcessingFragmentShaderFilesIterator = &files->postProcessingFragmentShaderFiles;
    ShaderFiles* postProcessingVertexShaderFilesIterator = &files->postProcessingVertexShaderFiles;

    if (shaderProps->atomicTextures > 0 && atomicImageTexture == NULL) {
        atomicImageTexture = new unsigned int[shaderProps->atomicTextures];
        for (int i = 0; i < shaderProps->atomicTextures; i++)
            atomicImageTexture[i] = 0;
    }

    if (shaderProps->imageTextures > 0 && imageTexture == NULL) {
        imageTexture = new unsigned int[shaderProps->imageTextures];

        for (int i = 0; i < shaderProps->imageTextures; i++)
            imageTexture[i] = 0;
    }

    while (fragmentShaderFilesIterator != NULL) {
        VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 0, &currentStage->glProgram, &currentStage->uniformLocations, NULL, NULL);
        if (vertexShaderFilesIterator == NULL) {
            vertexShaderFilesIterator = new ShaderFiles;
            vertexShaderFilesIterator->fileContent = "";
        }

        currentStage->vertexShader = new VertexShader(vertexShaderFilesIterator->fileContent, vertexArgs);

        currentStage->vertexShaderFile = vertexShaderFilesIterator;
        currentStage->fragmentShaderFile = fragmentShaderFilesIterator;

        for (auto i : vertexShaderFilesIterator->includedFiles) {
            filesWatcherInstance->addDependency(i, vertexShaderFilesIterator);
        }

        for (auto i : fragmentShaderFilesIterator->includedFiles) {
            filesWatcherInstance->addDependency(i, fragmentShaderFilesIterator);
        }

        FragmentShaderCompilationArgs* args = currentStage != startStage ? new FragmentShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 0, &currentStage->glProgram, &currentStage->uniformLocations,
                                                                               &uniformValues,
                                                                               &uniformTypes)
                                                                         : new FragmentShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 0, &currentStage->glProgram,
                                                                               &currentStage->uniformLocations,
                                                                               &uniformValues,
                                                                               &uniformTypes,
                                                                               shaderProps->atomicTextures, shaderProps->imageTextures, atomicImageTexture, imageTexture);

        currentStage->fragmentShader = new FragmentShader(fragmentShaderFilesIterator->fileContent, args);

        if (gtk_gl_area_get_error(GTK_GL_AREA(area)) != NULL) {
            Errors::throwError("failed to initialize buffers", shaderProps->className, "For");
            return;
        }

        if (fragmentShaderFilesIterator->next != NULL || !postProcessingFragmentShaderFilesIterator->fileContent.empty())
            currentStage->fragmentShader->bind2DTextureToFrameBuffer(shaderProps->className);

        fragmentShaderFilesIterator = fragmentShaderFilesIterator->next;

        if (vertexShaderFilesIterator != NULL)
            vertexShaderFilesIterator = vertexShaderFilesIterator->next;

        if (currentStage->next == NULL)
            currentStage->next = new ShaderStage;

        currentStage = currentStage->next;
    }

    if (!postProcessingFragmentShaderFilesIterator->fileContent.empty())

        while (postProcessingFragmentShaderFilesIterator != NULL) {
            VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 0, &currentStage->glProgram, &currentStage->uniformLocations, NULL, NULL);

            if (postProcessingVertexShaderFilesIterator == NULL) {
                postProcessingVertexShaderFilesIterator = new ShaderFiles;
                postProcessingVertexShaderFilesIterator->fileContent = "";
            }

            currentStage->vertexShader = new VertexShader(postProcessingVertexShaderFilesIterator->fileContent, vertexArgs);

            currentStage->vertexShaderFile = postProcessingVertexShaderFilesIterator;
            currentStage->fragmentShaderFile = postProcessingFragmentShaderFilesIterator;

            if (postProcessingVertexShaderFilesIterator != NULL)
                for (auto i : postProcessingVertexShaderFilesIterator->includedFiles) {
                    filesWatcherInstance->addDependency(i, postProcessingVertexShaderFilesIterator);
                }

            for (auto i : postProcessingFragmentShaderFilesIterator->includedFiles) {
                filesWatcherInstance->addDependency(i, postProcessingFragmentShaderFilesIterator);
            }

            FragmentShaderCompilationArgs* args = currentStage != startStage ? new FragmentShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 0, &currentStage->glProgram, &currentStage->uniformLocations,
                                                                                   &uniformValues,
                                                                                   &uniformTypes)
                                                                             : new FragmentShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 0, &currentStage->glProgram,
                                                                                   &currentStage->uniformLocations,
                                                                                   &uniformValues,
                                                                                   &uniformTypes,
                                                                                   shaderProps->atomicTextures, shaderProps->imageTextures, atomicImageTexture, imageTexture);

            currentStage->fragmentShader = new FragmentShader(postProcessingFragmentShaderFilesIterator->fileContent, args);

            if (gtk_gl_area_get_error(GTK_GL_AREA(area)) != NULL) {
                Errors::throwError("failed to initialize buffers", shaderProps->className, "For");
                return;
            }

            if (postProcessingFragmentShaderFilesIterator->next != NULL)
                currentStage->fragmentShader->bind2DTextureToFrameBuffer(shaderProps->className);

            postProcessingFragmentShaderFilesIterator = postProcessingFragmentShaderFilesIterator->next;

            if (postProcessingVertexShaderFilesIterator != NULL)
                postProcessingVertexShaderFilesIterator = postProcessingVertexShaderFilesIterator->next;

            if (currentStage->next == NULL)
                currentStage->next = new ShaderStage;

            currentStage = currentStage->next;
        }
}

#define TWOPI 6.28318530718
// #define PI 3.14159265359
#define window(t, sz) \
    (0.53836 - (0.46164 * cos(TWOPI * (double)t / (double)sz)))

void low_pass_filter(float* data, int n_samples, float cutoff_freq, float sample_rate)
{
    float RC = 1.0 / (2 * M_PI * cutoff_freq);
    float dt = 1.0 / sample_rate;
    float alpha = dt / (RC + dt);

    float previous = data[0];
    for (int i = 1; i < n_samples; i++) {
        data[i] = previous + alpha * (data[i] - previous);
        previous = data[i];
    }
}

// Portions adapted from GLava's original implementation of Radix-2 DIT FFT by jarcode-foss
// Licensed under GPL-3.0

void applyFFT(float* samples, int n_samples, float fftScale, float fftCutOff)
{

    float* data = samples;
    unsigned long nn = (unsigned long)(n_samples) / 2;

    unsigned long n, mmax, m, j, istep, i;
    float wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi;

    /* apply window */
    for (i = 0; i < n_samples; ++i) {
        data[i] *= window(i, n_samples - 1);
    }

    /* reverse-binary reindexing */
    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
        if (j > i) {
            std::swap(data[j - 1], data[i - 1]);
            std::swap(data[j], data[i]);
        }
        m = nn;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    };

    /* here begins the Danielson-Lanczos section */
    mmax = 2;
    while (n > mmax) {
        istep = mmax << 1;
        theta = -(2 * M_PI / mmax);
        wtemp = sin(0.5 * theta);
        wpr = -2.0 * wtemp * wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 1; m < mmax; m += 2) {
            for (i = m; i <= n; i += istep) {
                j = i + mmax;
                tempr = wr * data[j - 1] - wi * data[j];
                tempi = wr * data[j] + wi * data[j - 1];

                data[j - 1] = data[i - 1] - tempr;
                data[j] = data[i] - tempi;
                data[i - 1] += tempr;
                data[i] += tempi;
            }
            wtemp = wr;
            wr += wr * wpr - wi * wpi;
            wi += wi * wpr + wtemp * wpi;
        }
        mmax = istep;
    }

    /* abs and log scale */
    for (n = 0; n < n_samples; n += 2) {

        if (data[n] < 0.0F)
            data[n] = -data[n];

        data[n] = sqrt(data[n] * data[n] + data[n + 1] * data[n + 1]);
        data[n] = log(data[n] + 1) / 3;
        data[n] *= std::max((((float)n / (float)n_samples) * fftScale) + (1.0F - fftCutOff), 1.0F);

        data[n + 1] = data[n];
    }
}

void ShaderProgram::applyAudioTransformations()
{

    pthread_mutex_lock(&pipeWireSetting->mutex);
    bool modified = pipeWireSetting->modified;
    if (modified)
        pipeWireSetting->modified = false;

    if (modified && pipeWireSetting->applyFFT) {

        applyFFT(pipeWireSetting->audioLData, pipeWireSetting->audioLSize, pipeWireSetting->fftScale, pipeWireSetting->fftCutOff);
        applyFFT(pipeWireSetting->audioRData, pipeWireSetting->audioRSize, pipeWireSetting->fftScale, pipeWireSetting->fftCutOff);
    }

    if (!pipeWireSetting->applyFFT || (shaderProps->audioOverrides->minFrequency == 0 && shaderProps->audioOverrides->maxFrequency == pipeWireSetting->sampleRate / 2)) {

        if (*shaderProps->audioOverrides->strictFrequencyBounds) {

            float rData[pipeWireSetting->audioRSize], lData[pipeWireSetting->audioLSize];

            audioShaderStages
                ->reduceRangeForFullSpectrum(pipeWireSetting->audioRData, rData,
                    pipeWireSetting->audioRSize, pipeWireSetting->sampleRate);

            audioShaderStages
                ->reduceRangeForFullSpectrum(pipeWireSetting->audioLData, lData,
                    pipeWireSetting->audioLSize, pipeWireSetting->sampleRate);

            audioShaderStages->updateAudioTextures(0, pipeWireSetting->audioLSize, lData, 0, pipeWireSetting->audioRSize, rData);
        }

        else
            audioShaderStages->updateAudioTextures(0, pipeWireSetting->audioLSize, pipeWireSetting->audioLData, 0, pipeWireSetting->audioRSize, pipeWireSetting->audioRData);

        pthread_mutex_unlock(&pipeWireSetting->mutex);

        if (audioShaderStages->passStage->shouldSyncFiles()) {
            VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(0, 0, 0, NULL, NULL, NULL, NULL);
            FragmentShaderCompilationArgs* args = new FragmentShaderCompilationArgs(0, 0, 0, NULL, NULL, &uniformValues, &uniformTypes);

            audioShaderStages->passStage->syncFiles(vertexArgs, args);
        }

        if (audioShaderStages->gravityStage->shouldSyncFiles()) {
            VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(0, 0, 0, NULL, NULL, NULL, NULL);
            FragmentShaderCompilationArgs* args = new FragmentShaderCompilationArgs(0, 0, 0, NULL, NULL, &uniformValues, &uniformTypes);

            audioShaderStages->gravityStage->syncFiles(vertexArgs, args);
        }

        if (audioShaderStages->averageStage->shouldSyncFiles()) {
            VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(0, 0, 0, NULL, NULL, NULL, NULL);
            FragmentShaderCompilationArgs* args = new FragmentShaderCompilationArgs(0, 0, 0, NULL, NULL, &uniformValues, &uniformTypes);

            audioShaderStages->averageStage->syncFiles(vertexArgs, args);
        }

        audioShaderStages->applyGravityPassShader(pipeWireSetting->audioRSize, 1, "audioR", shaderProps->shaderName);
        audioShaderStages->applyGravityPassShader(pipeWireSetting->audioLSize, 2, "audioR", shaderProps->shaderName);

        if (*shaderProps->audioOverrides->smoothAudio || isFirst) {

            if (audioShaderStages->smoothStage->shouldSyncFiles()) {
                VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(0, 0, 0, NULL, NULL, NULL, NULL);
                FragmentShaderCompilationArgs* args = new FragmentShaderCompilationArgs(0, 0, 0, NULL, NULL, &uniformValues, &uniformTypes);

                audioShaderStages->smoothStage->syncFiles(vertexArgs, args);
            }

            audioShaderStages->applySmoothPassShader(pipeWireSetting->audioRSize, shaderProps->audioOverrides->smoothSettings->adjacentSampleNums, 1, "audioR", shaderProps->shaderName);
            audioShaderStages->applySmoothPassShader(pipeWireSetting->audioLSize, shaderProps->audioOverrides->smoothSettings->adjacentSampleNums, 2, "audioR", shaderProps->shaderName);
        }
    } else {

        int rSize = pipeWireSetting->audioRSize,
            lSize = pipeWireSetting->audioLSize, lOffset = 0, rOffset = 0;

        float rData[rSize], lData[lSize];

        int* modifiedRBins = audioShaderStages
                                 ->applyRangeSelection(pipeWireSetting->audioRData, rData,
                                     rSize, (float)(shaderProps->audioOverrides->minFrequency), (float)(shaderProps->audioOverrides->maxFrequency),
                                     pipeWireSetting->sampleRate);

        int* modifiedLBins = audioShaderStages
                                 ->applyRangeSelection(pipeWireSetting->audioLData, lData,
                                     lSize, (float)(shaderProps->audioOverrides->minFrequency), (float)(shaderProps->audioOverrides->maxFrequency),
                                     pipeWireSetting->sampleRate);

        if (*shaderProps->audioOverrides->strictFrequencyBounds) {
            rSize = 2 * (modifiedRBins[1] - modifiedRBins[0]) + 1;
            lSize = 2 * (modifiedLBins[1] - modifiedLBins[0]) + 1;
            lOffset = 2 * modifiedLBins[0];
            rOffset = 2 * modifiedRBins[0];
        }

        audioShaderStages->updateAudioTextures(lOffset, lSize, lData, rOffset, rSize, rData);
        pthread_mutex_unlock(&pipeWireSetting->mutex);

        if (audioShaderStages->passStage->shouldSyncFiles()) {
            VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(0, 0, 0, NULL, NULL, NULL, NULL);
            FragmentShaderCompilationArgs* args = new FragmentShaderCompilationArgs(0, 0, 0, NULL, NULL, &uniformValues, &uniformTypes);

            audioShaderStages->passStage->syncFiles(vertexArgs, args);
        }

        if (audioShaderStages->gravityStage->shouldSyncFiles()) {
            VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(0, 0, 0, NULL, NULL, NULL, NULL);
            FragmentShaderCompilationArgs* args = new FragmentShaderCompilationArgs(0, 0, 0, NULL, NULL, &uniformValues, &uniformTypes);

            audioShaderStages->gravityStage->syncFiles(vertexArgs, args);
        }

        if (audioShaderStages->averageStage->shouldSyncFiles()) {
            VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(0, 0, 0, NULL, NULL, NULL, NULL);
            FragmentShaderCompilationArgs* args = new FragmentShaderCompilationArgs(0, 0, 0, NULL, NULL, &uniformValues, &uniformTypes);

            audioShaderStages->averageStage->syncFiles(vertexArgs, args);
        }

        audioShaderStages->applyGravityPassShader(rSize, 1, "audioR", shaderProps->shaderName);
        audioShaderStages->applyGravityPassShader(lSize, 2, "audioR", shaderProps->shaderName);

        if (*shaderProps->audioOverrides->smoothAudio || isFirst) {

            if (audioShaderStages->smoothStage->shouldSyncFiles()) {
                VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(0, 0, 0, NULL, NULL, NULL, NULL);
                FragmentShaderCompilationArgs* args = new FragmentShaderCompilationArgs(0, 0, 0, NULL, NULL, &uniformValues, &uniformTypes);

                audioShaderStages->smoothStage->syncFiles(vertexArgs, args);
            }
            audioShaderStages->applySmoothPassShader(rSize, shaderProps->audioOverrides->smoothSettings->adjacentSampleNums, 1, "audioR", shaderProps->shaderName);
            audioShaderStages->applySmoothPassShader(lSize, shaderProps->audioOverrides->smoothSettings->adjacentSampleNums, 2, "audioR", shaderProps->shaderName);
        }
    }

    glViewport(0, 0, shaderProps->windowWidth, shaderProps->windowHeight);
}

void ShaderProgram::loadAudioShaders()
{

    audioShaderStages->createAudioTextures();

    ShaderFiles* file = &files->passShaderFile;
    while (file != NULL) {
        audioShaderStages->passStage
            ->compile(NULL, &files->passShaderFile);

        for (auto i : files->passShaderFile.includedFiles) {
            filesWatcherInstance->addDependency(i, &files->passShaderFile);
        }
        file = file->next;
    }

    file = &files->smoothShaderFile;
    while (file != NULL) {

        audioShaderStages->smoothStage->compile(NULL, &files->smoothShaderFile);

        for (auto i : files->smoothShaderFile.includedFiles) {
            filesWatcherInstance->addDependency(i, &files->smoothShaderFile);
        }
        glUseProgram(audioShaderStages->smoothStage->glProgram);

        int uniformLoc = audioShaderStages->smoothStage->uniformLocations["smooth_factor"];
        glUniform1f(uniformLoc, shaderProps->audioOverrides->smoothSettings->smoothFactor);

        uniformLoc = audioShaderStages->smoothStage->uniformLocations["sample_hybrid_weight"];
        glUniform1f(uniformLoc, shaderProps->audioOverrides->smoothSettings->sampleHybridWeight);

        uniformLoc = audioShaderStages->smoothStage->uniformLocations["sample_scale"];
        glUniform1f(uniformLoc, shaderProps->audioOverrides->smoothSettings->sampleScale);

        uniformLoc = audioShaderStages->smoothStage->uniformLocations["sample_range"];
        glUniform1f(uniformLoc, shaderProps->audioOverrides->smoothSettings->sampleRange);

        uniformLoc = audioShaderStages->smoothStage->uniformLocations["sample_mode"];
        glUniform1i(uniformLoc, shaderProps->audioOverrides->smoothSettings->sampleMode);

        glUseProgram(0);

        file = file->next;
    }

    file = &files->gravityShaderFile;
    while (file != NULL) {

        audioShaderStages->gravityStage->compile(NULL, &files->gravityShaderFile);

        for (auto i : files->gravityShaderFile.includedFiles) {
            filesWatcherInstance->addDependency(i, &files->gravityShaderFile);
        }
        glUseProgram(audioShaderStages->gravityStage->glProgram);

        int uniformLoc = audioShaderStages->gravityStage->uniformLocations["diff"];
        glUniform1f(uniformLoc, (float)((shaderProps->audioOverrides->gravitySettings->gravityStep) / fps));
        glUseProgram(0);

        file = file->next;
    }

    file = &files->averageShaderFile;
    while (file != NULL) {

        audioShaderStages->averageStage->compile(NULL, &files->averageShaderFile);

        for (auto i : files->averageShaderFile.includedFiles) {
            filesWatcherInstance->addDependency(i, &files->averageShaderFile);
        }
        glUseProgram(audioShaderStages->averageStage->glProgram);
        int frameLoc = audioShaderStages->averageStage->uniformLocations["avgFrames"];
        glUniform1i(frameLoc, shaderProps->audioOverrides->gravitySettings->averageFrames);
        glUseProgram(0);
        file = file->next;
    }

    audioLoaded = true;
}

gboolean ShaderProgram::realize(GtkGLArea* area)
{
    gtk_gl_area_make_current(GTK_GL_AREA(area));

    if (gtk_gl_area_get_error(GTK_GL_AREA(area)) != NULL)
        Errors::throwError("failed to initialize buffers", shaderProps->className, "For");

    return TRUE;
}

gboolean ShaderProgram::render(GtkGLArea* area)
{

    int defaultID;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultID);

    if (renderAudio)
        applyAudioTransformations();

    if (ticks == 0)
        initializeShaders(area);

    if (!shouldRender) {

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
            defaultID);
        glClear(GL_COLOR_BUFFER_BIT);
        ticks--;
        return true;
    }

    ShaderStage* currentStage = startStage;

    while (currentStage->next != NULL) {

        if (currentStage->shouldSyncFiles()) {
            VertexShaderCompilationArgs* vertexArgs = new VertexShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 1, NULL, &currentStage->uniformLocations, NULL, NULL);

            FragmentShaderCompilationArgs* args = currentStage != startStage ? new FragmentShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 1, NULL, &currentStage->uniformLocations,
                                                                                   &uniformValues,
                                                                                   &uniformTypes)
                                                                             : new FragmentShaderCompilationArgs(shaderProps->windowWidth, shaderProps->windowHeight, 1, NULL,
                                                                                   &currentStage->uniformLocations,
                                                                                   &uniformValues,
                                                                                   &uniformTypes,

                                                                                   shaderProps->atomicTextures, shaderProps->imageTextures, atomicImageTexture, imageTexture);

            if (currentStage->syncFiles(vertexArgs, args) && (currentStage->next->next != NULL))
                currentStage->fragmentShader->bind2DTextureToFrameBuffer(shaderProps->className);
        }

        glUseProgram(currentStage->glProgram);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
            currentStage->next->next == NULL
                ? defaultID
                : currentStage->fragmentShader->frameBufferObject);

        currentStage
            ->fragmentShader
            ->updateUniforms(shaderProps->windowWidth, shaderProps->windowHeight, shaderProps->audioOverrides->audioMap->left, shaderProps->audioOverrides->audioMap->right, ticks,
                pipeWireSetting->audioLSize,
                *shaderProps->audioOverrides->smoothAudio ? audioShaderStages->smoothStage
                                                                ->outputLTexture
                                                          : audioShaderStages->averageStage
                                                                ->outputLTexture,
                pipeWireSetting->audioRSize,
                *shaderProps->audioOverrides->smoothAudio
                    ? audioShaderStages->smoothStage
                          ->fragmentShader->outputTexture
                    : audioShaderStages->averageStage
                          ->fragmentShader->outputTexture,
                uniformValues,
                uniformTypes,
                currentStage->uniformLocations);

        currentStage->vertexShader->draw(currentStage != startStage ? &prevStageTexture : NULL);

        prevStageTexture = currentStage->fragmentShader->outputTexture;

        currentStage = currentStage->next;
        glUseProgram(0);
    };

    return TRUE;
}
