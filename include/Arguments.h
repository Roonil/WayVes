#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <iostream>
#include <optional>

#include "DBusHandler.h"

class AudioArguments
{
public:
    std::optional<std::string> audioName, targetObject, audioFormat;
    std::optional<int> channels, sampleSize, fragmentSize;
    std::optional<float> fftScale, fftCutOff;
    std::optional<bool> captureMic, applyFFT;
    std::optional<unsigned int> sampleRate;
};

class Arguments
{
private:
    int argc;
    char **args = NULL;

    DBusHandler *dBusHandler = NULL;

    bool layoutArgsSpecified();
    bool configFileNameWithOtherArgs();

public:
    std::optional<int> marginLeft, marginRight, marginTop, marginBottom, anchorLeft, anchorRight,
        anchorTop, anchorBottom, layer, visibility, windowWidth, windowHeight, exclusiveLayer;
    std::optional<std::string> className, configFileName, version, instance;
    AudioArguments audioArguments;
    Arguments(int argc, char *args[]);
    void signalGApps();
};

#endif