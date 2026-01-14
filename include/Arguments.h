#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <iostream>
#include <argp.h>

#include "GDBusHandler.h"

class Arguments
{
private:
    int argc;
    char **args = NULL;

    GDBusHandler *gdBusHandler = NULL;

    static error_t parseOptions(int key, char *arg, struct argp_state *state);
    inline static const char *doc = "WayVes - OpenGL-based Audio Visualiser for PipeWire on Wayland";

    inline static struct argp_option options[] = {
        {"class", 'c', "class-name", 0, "Class Name to target"},
        {"layer", 'z', "layer-value", 0, "Specify the Layer of the Shader Window"},
        {"file", 'f', "file-name", 0, "Specify the Name of the Configuration File"},
        {"exlusive-layer", 'x', "exclusive-layer-value", 0, "Toggle Exclusive Layer"},
        {"left-margin", 'l', "left-margin-value", 0, "Specify the Left Margin"},
        {"right-margin", 'r', "right-margin-value", 0, "Specify the Right Margin"},
        {"top-margin", 't', "top-margin-value", 0, "Specify the Top Margin"},
        {"bottom-margin", 'b', "bottom-margin-value", 0, "Specify the Bottom Margin"},
        {"left-anchor", 's', "left-anchor-value", 0, "Specify the Left Anchor"},
        {"right-anchor", 'p', "right-anchor-value", 0, "Specify the Right Anchor"},
        {"top-anchor", 'u', "top-anchor-value", 0, "Specify the Top Anchor"},
        {"bottom-anchor", 'd', "bottom-anchor-value", 0, "Specify the Bottom Anchor"},
        {"visibility", 'v', "visibility-value", OPTION_ARG_OPTIONAL, "Toggle Visibility, or set Visibility to the specified value"},
        {"width", 'w', "width-value", 0, "Specify Window Width"},
        {"height", 'h', "height-value", 0, "Specify Window Height"},
        {"version", 'V', 0, 0, "Show Version Information"},
        {0}};

    inline static struct argp argParser = {options, parseOptions, NULL, doc};

    bool fileNameWithOtherArgs();

public:
    int *marginLeft = NULL, *marginRight = NULL, *marginTop = NULL, *marginBottom = NULL, *anchorLeft = NULL, *anchorRight = NULL,
        *anchorTop = NULL, *anchorBottom = NULL, *layer = NULL, *visibility = NULL, *windowWidth = NULL, *windowHeight = NULL, *exclusiveLayer = NULL;
    char *className = NULL, *configFileName = NULL, *version = NULL;

    Arguments(int argc, char *args[]);
    void signalGApps();
};

#endif