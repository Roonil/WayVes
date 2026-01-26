#ifndef D_BUS_HANDLER_H
#define D_BUS_HANDLER_H

#include <gio/gio.h>
#include <string>
#include <iostream>
#include <dbus/dbus.h>

class DBusHandler
{
private:
    DBusConnection *conn = NULL;

public:
    DBusHandler();

    void emitUniforms(std::string className, std::string uniforms);
    void emitCLIArgs(std::string className, int argc, char *args[]);
    void emitTearDownForInstance(std::string instance);
    ~DBusHandler();
};

#endif