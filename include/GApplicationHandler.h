#ifndef G_APPLICATION_HANDLER_H
#define G_APPLICATION_HANDLER_H

#include "Arguments.h"
#include "Configs.h"
#include "ShaderProgram.h"
#include <gtk/gtk.h>
#include <gtk4-layer-shell/gtk4-layer-shell.h>

#include <iostream>
#include <sstream>
#include <string>

class GApplicationHandler {
private:
    GMainLoop* loop = NULL;
    Configs* configs = NULL;
    std::string busName;

    static inline GTimer* timer;

    static void rendererThread(gpointer data);

    static void activateApplication(Configs* configs, GDBusConnection* conn);
    static void onBusAcquired(GDBusConnection* conn, const gchar* name, gpointer configs);

public:
    GApplicationHandler(Configs* configs, std::string instance);
    void runApp();
    ~GApplicationHandler();
};

#endif