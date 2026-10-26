#ifndef PAINTABLE_SHADER_PROPS_H
#define PAINTABLE_SHADER_PROPS_H

#include <cstdint>
#include <gtk/gtk.h>
#include <iostream>

enum layers {
    background = 0,
    bottom,
    top,
    overlay
};

enum renderingModes {
    windowed = 0,
    detached,
    mixed
};

class PaintableShaderProps {
public:
    char* className = NULL;
    uint16_t windowWidth, windowHeight;
    enum layers layer;

    int marginLeft, marginRight, marginTop, marginBottom;
    bool exclusiveZone;
    bool anchorLeft, anchorRight, anchorTop, anchorBottom;
    bool initialVisibility;

    void setWindowProps(GtkWindow* gtk_window);
};

#endif