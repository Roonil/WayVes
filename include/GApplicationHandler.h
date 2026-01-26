#ifndef G_APPLICATION_HANDLER_H
#define G_APPLICATION_HANDLER_H

#include <gtk/gtk.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include "ShaderProgram.h"
#include "Configs.h"
#include "Arguments.h"

#include <argp.h>

#include <time.h>

#include <iostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>

class GApplicationHandler
{
private:
    GMainLoop *loop = NULL;
    Configs *configs = NULL;
    std::string busName;

    static inline GTimer *timer;

    static void rendererThread(gpointer data);

    static void activateApplication(Configs *configs, GDBusConnection *conn);
    static void onBusAcquired(GDBusConnection *conn, const gchar *name, gpointer configs);

public:
    GApplicationHandler(Configs *configs, std::string instance);
    void runApp();
    ~GApplicationHandler();
};

#endif