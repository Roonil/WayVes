#ifndef GD_BUS_HANDLER_H
#define GD_BUS_HANDLER_H

#include <gio/gio.h>
#include <string>
#include <iostream>

class GDBusHandler
{
private:
    GDBusConnection *conn = NULL;

public:
    GDBusHandler();

    void emitUniforms(std::string className, std::string uniforms);
    void emitCLIArgs(std::string className, char *args[]);

    ~GDBusHandler();
};

#endif