#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>
#include <vector>
#include <sstream>

#include "ShaderProgram.h"
#include "Arguments.h"
#include "gtk4-layer-shell/gtk4-layer-shell.h"

class WindowHandler
{
public:
    std::string appName = "com.r00n1l.";
    ShaderProgram shaderProgram;

    void setGtkWindow();

    virtual void handleGDBusSignals(GDBusConnection *conn) = 0;
    virtual void disableInputs() = 0;
    virtual void displayWindow() = 0;
    virtual void setup(GDBusConnection *conn, void *args) = 0;

protected:
    GtkWindow *window = NULL;
    cairo_rectangle_int_t rect = {-1, -1, 0, 0};
    cairo_region_t *region = cairo_region_create_rectangle(&rect);

    static void applyCLIArgs(Arguments arguments, WindowHandler *windowHandler);
    static void handleCLI(GDBusConnection *connection,
                          const gchar *sender_name,
                          const gchar *object_path,
                          const gchar *interface_name,
                          const gchar *signal_name,
                          GVariant *parameters,
                          gpointer user_data);
};

class PaintableWindowHandler : public WindowHandler
{
private:
    GdkPaintable *paintable;
    GtkWidget *picture;
    GtkWidget *glArea;

public:
    PaintableWindowHandler(PaintableShaderProps *shaderConfig);

    void getPaintable(GtkWidget *glArea);
    void handleGDBusSignals(GDBusConnection *conn);
    void setup(GDBusConnection *conn, void *args);
    void toggleVisibility(bool newVisibility);
    void disableInputs();
    void displayWindow();
};

class ShaderWindowHandler : public WindowHandler
{
private:
    static void parseMessage(char buffer[], int bytes, std::map<std::string, std::string> *runTimeOverrides);
    static void handleUniforms(GDBusConnection *connection,
                               const gchar *sender_name,
                               const gchar *object_path,
                               const gchar *interface_name,
                               const gchar *signal_name,
                               GVariant *parameters,
                               gpointer user_data);

    static gboolean realize(GtkGLArea *area, GdkGLContext *context,
                            gpointer *data);

    static void resize(GtkGLArea *self, gint width, gint height, gpointer *data);
    static gboolean render(GtkGLArea *area, GdkGLContext *context, gpointer *data);

    void setupPaintables(GDBusConnection *conn);

public:
    GtkWidget *glArea = NULL;
    int audioIdx = 0;

    PaintableWindowHandler **paintableWindows = NULL;

    int paintableWindowsCount = 0;

    ShaderWindowHandler(ShaderProps *shaderProps, PipeWireHandler *pipeWireHandler, AudioShaderStages *audioShaderStages, bool isFirst, std::string directoryPrefix, std::map<std::string, std::string> overridesMap);

    void setGLArea();

    void handleGDBusSignals(GDBusConnection *conn);
    void setRendererSignals();
    void setup(GDBusConnection *conn, void *args);
    void disableInputs();
    void displayWindow();
};

#endif