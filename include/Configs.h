#ifndef CONFIGS_H
#define CONFIGS_H

#include "WindowHandler.h"

class Configs
{
public:
    ShaderWindowHandler *windowHandler;
    Configs *next = NULL;
    std::string audioName;
    int totalAudios = 0;
    std::string directoryPrefix;
};

#endif