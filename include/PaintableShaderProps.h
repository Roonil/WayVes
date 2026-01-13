#ifndef PAINTABLE_SHADER_PROPS_H
#define PAINTABLE_SHADER_PROPS_H

#include <iostream>

enum layers
{
    background = 0,
    bottom,
    top,
    overlay
};

class PaintableShaderProps
{
public:
    char *className = NULL;
    uint16_t windowWidth, windowHeight;
    enum layers layer;
    int marginLeft, marginRight, marginTop, marginBottom;
    bool exclusiveZone;
    bool anchorLeft, anchorRight, anchorTop, anchorBottom;

    void setWindowProps(GtkWindow *gtk_window);
};

#endif