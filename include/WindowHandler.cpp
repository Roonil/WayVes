#include "WindowHandler.h"
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <algorithm>

ShaderWindowHandler::ShaderWindowHandler(ShaderProps *shaderProps, PipeWireHandler *pipeWireHandler, AudioShaderStages *audioShaderStages, bool isFirst, std::string directoryPrefix, std::map<std::string, std::string> overridesMap)
{

    shaderProgram.shaderProps = shaderProps;

    shaderProgram.isFirst = isFirst;

    if (shaderProps->audioOverrides == NULL)
        return;

    shaderProgram.pipeWireSetting = pipeWireHandler;

    shaderProgram.files = new Files(std::string(shaderProgram.shaderProps->shaderName), std::string(shaderProps->configFileName), directoryPrefix);
    shaderProgram.files->LoadFiles(overridesMap, shaderProps->audioOverrides, shaderProps->postProcessingShaders, shaderProps->postProcessingShaders_count);

    shaderProgram.audioShaderStages = audioShaderStages;

    shaderProgram.fps = shaderProps->fps;
}

void ShaderWindowHandler::parseMessage(char buffer[], int bytes, std::map<std::string, std::string> *runTimeOverrides)
{
    std::stringstream bufferStream;
    bufferStream << buffer;

    std::string line;

    while (std::getline(bufferStream, line, '\n'))
    {

        int equalsIdx = line.find("=");
        if (equalsIdx == std::string::npos)
            Errors::throwError("Invalid format in Piped data");

        std::string variableName = line.substr(0, equalsIdx);
        std::string variableValue = line.substr(equalsIdx + 1);

        variableName.erase(std::remove_if(variableName.begin(), variableName.end(), ::isspace), variableName.end());
        variableValue.erase(std::remove_if(variableValue.begin(), variableValue.end(), ::isspace), variableValue.end());

        (*runTimeOverrides)[variableName] = variableValue;
    }
}

gboolean ShaderWindowHandler::realize(GtkGLArea *area, GdkGLContext *context,
                                      gpointer *data)
{
    ShaderProgram *n = (ShaderProgram *)data;

    return n->realize(area);
}

void ShaderWindowHandler::resize(GtkGLArea *self, gint width, gint height, gpointer *data)
{
    ShaderProgram *shaderProgram = (ShaderProgram *)data;

    if (shaderProgram->shaderProps->windowWidth == width && shaderProgram->shaderProps->windowHeight == height)
        return;

    shaderProgram->shaderProps->windowWidth = width;
    shaderProgram->shaderProps->windowHeight = height;

    ShaderStage *currentStage = shaderProgram->startStage;

    while (currentStage != NULL)
    {
        glDeleteTextures(shaderProgram->shaderProps->atomicTextures, shaderProgram->atomicImageTexture);
        glDeleteTextures(shaderProgram->shaderProps->imageTextures, shaderProgram->imageTexture);

        ShaderStage *tmpStage = currentStage;

        glDeleteProgram(currentStage->glProgram);
        currentStage = currentStage->next;
        delete tmpStage;
    }
    shaderProgram->startStage = NULL;
    shaderProgram->atomicImageTexture = NULL;
    shaderProgram->imageTexture = NULL;

    glDeleteProgram(0);

    shaderProgram->ticks = 0;
}

gboolean ShaderWindowHandler::render(GtkGLArea *area, GdkGLContext *context, gpointer *data)
{
    ShaderProgram *shaderProgram = (ShaderProgram *)data;

    if (shaderProgram->ticks == 0 && shaderProgram->renderAudio &&
        !shaderProgram->audioLoaded)
        shaderProgram->loadAudioShaders();

    shaderProgram->render(area);

    shaderProgram->ticks++;

    return FALSE;
}

void ShaderWindowHandler::setRendererSignals()
{
    g_signal_connect(glArea, "realize", G_CALLBACK(realize),
                     &shaderProgram);
    g_signal_connect(glArea, "render", G_CALLBACK(render),
                     &shaderProgram);
    g_signal_connect(glArea, "resize", G_CALLBACK(resize),
                     &shaderProgram);
}

void ShaderWindowHandler::handleUniforms(GDBusConnection *connection,
                                         const gchar *sender_name,
                                         const gchar *object_path,
                                         const gchar *interface_name,
                                         const gchar *signal_name,
                                         GVariant *parameters,
                                         gpointer user_data)
{
    if (g_strcmp0(signal_name, "UniformValue") == 0)
    {
        const gchar *msg;
        g_variant_get(parameters, "(&s)", &msg);
        std::string messageString = std::string(msg);

        parseMessage((char *)msg, messageString.size(), (std::map<std::string, std::string> *)user_data);
        // g_dbus_method_invocation_return_value(invocation, NULL);
    }
}

PaintableWindowHandler::PaintableWindowHandler(PaintableShaderProps *shaderConfig)
{
    shaderProgram.paintableShaderConfig = shaderConfig;
}

void PaintableWindowHandler::toggleVisibility(bool newVisibility)
{
    if (newVisibility == false)
    {

        gtk_window_destroy(window);

        setGtkWindow();

        return;
    }

    getPaintable((GtkWidget *)glArea);

    displayWindow();
}

void PaintableWindowHandler::getPaintable(GtkWidget *glArea)
{
    GdkPaintable *paintable = gtk_widget_paintable_new(glArea);
    GtkWidget *picture = gtk_picture_new_for_paintable(paintable);
    this->paintable = paintable;
    this->picture = picture;
    this->glArea = glArea;

    gtk_widget_add_css_class(picture, shaderProgram.paintableShaderConfig->className);
}

void PaintableWindowHandler::disableInputs()
{
    gdk_surface_set_input_region(
        gtk_native_get_surface(gtk_widget_get_native(GTK_WIDGET(window))),
        region);

    gdk_surface_set_input_region(
        gtk_native_get_surface(gtk_widget_get_native(GTK_WIDGET(picture))),
        region);

    gtk_widget_set_can_target(GTK_WIDGET(window), false);
    gtk_widget_set_can_target(picture, false);
    gtk_widget_set_can_focus(GTK_WIDGET(window), false);
    gtk_widget_set_can_focus(picture, false);
}

void PaintableWindowHandler::displayWindow()
{

    gtk_window_set_child(window, (GtkWidget *)picture);
    gtk_widget_set_visible((GtkWidget *)picture, true);
    gtk_window_present(window);

    disableInputs();
}

void PaintableWindowHandler::handleGDBusSignals(GDBusConnection *conn)
{
    g_dbus_connection_signal_subscribe(conn, NULL, std::string("com.r00n1l." + std::string(shaderProgram.paintableShaderConfig->className)).c_str(), "ClassCLI", NULL, NULL, G_DBUS_SIGNAL_FLAGS_NONE, handleCLI, this, NULL);

    g_dbus_connection_signal_subscribe(conn, NULL, std::string("com.r00n1l.allcli").c_str(), "AllCLI", NULL, NULL, G_DBUS_SIGNAL_FLAGS_NONE, handleCLI, this, NULL);
}

void PaintableWindowHandler::setup(GDBusConnection *conn, void *args)
{
    setGtkWindow();

    getPaintable((GtkWidget *)args);

    displayWindow();

    handleGDBusSignals(conn);
}

void WindowHandler::applyCLIArgs(Arguments arguments, WindowHandler *windowHandler)
{

    bool newRenderValue = !windowHandler->shaderProgram.shouldRender;

    {
        GtkWindow *currentWindow = windowHandler->window;

        const char *title = gtk_window_get_title(currentWindow);

        if ((arguments.className == NULL) ||
            (title != NULL && strcmp(title, arguments.className) == 0))
        {

            // margins
            if (arguments.marginTop != NULL)
                gtk_layer_set_margin(currentWindow, GTK_LAYER_SHELL_EDGE_TOP,
                                     *arguments.marginTop);
            if (arguments.marginBottom != NULL)
                gtk_layer_set_margin(currentWindow, GTK_LAYER_SHELL_EDGE_BOTTOM,
                                     *arguments.marginBottom);
            if (arguments.marginLeft != NULL)
                gtk_layer_set_margin(currentWindow, GTK_LAYER_SHELL_EDGE_LEFT,
                                     *arguments.marginLeft);
            if (arguments.marginRight != NULL)
                gtk_layer_set_margin(currentWindow, GTK_LAYER_SHELL_EDGE_RIGHT,
                                     *arguments.marginRight);

            // anchors
            if (arguments.anchorTop != NULL)
                gtk_layer_set_anchor(currentWindow, GTK_LAYER_SHELL_EDGE_TOP,
                                     (gboolean)*arguments.anchorTop);
            if (arguments.anchorBottom != NULL)
                gtk_layer_set_anchor(currentWindow, GTK_LAYER_SHELL_EDGE_BOTTOM,
                                     (gboolean)*arguments.anchorBottom);
            if (arguments.anchorLeft != NULL)
                gtk_layer_set_anchor(currentWindow, GTK_LAYER_SHELL_EDGE_LEFT,
                                     (gboolean)*arguments.anchorLeft);
            if (arguments.anchorRight != NULL)
                gtk_layer_set_anchor(currentWindow, GTK_LAYER_SHELL_EDGE_RIGHT,
                                     (gboolean)*arguments.anchorRight);

            // layer
            if (arguments.layer != NULL)
                gtk_layer_set_layer(currentWindow,
                                    GtkLayerShellLayer(*arguments.layer));

            // Exclusive layer
            if (arguments.exclusiveLayer != NULL)
                gtk_layer_set_exclusive_zone(currentWindow, *arguments.exclusiveLayer);

            int width, height;
            gtk_window_get_default_size(currentWindow, &width, &height);

            if (GTK_IS_WINDOW(currentWindow) && arguments.windowHeight != NULL || arguments.windowWidth != NULL)
                gtk_window_set_default_size(
                    currentWindow,
                    arguments.windowWidth == NULL ? width : *arguments.windowWidth,
                    arguments.windowHeight == NULL ? height : *arguments.windowHeight);

            // visibility

            if (arguments.visibility != NULL)
            {

                windowHandler->shaderProgram.shouldRender = newRenderValue;

                if (windowHandler->shaderProgram.shaderProps == NULL)
                    ((PaintableWindowHandler *)windowHandler)->toggleVisibility(newRenderValue);
            }
        }
    }
}

void WindowHandler::handleCLI(GDBusConnection *connection,
                              const gchar *sender_name,
                              const gchar *object_path,
                              const gchar *interface_name,
                              const gchar *signal_name,
                              GVariant *parameters,
                              gpointer user_data)
{
    if (g_strcmp0(signal_name, "ClassCLI") != 0 && g_strcmp0(signal_name, "AllCLI") != 0)
        return;

    GVariantIter *iterator = NULL;
    g_variant_get(parameters, "(as)", &iterator);

    int argc = g_variant_iter_n_children(iterator);

    GVariant *sArray;

    char **args = new char *[argc];

    int i = 0;

    while ((sArray = g_variant_iter_next_value(iterator)) != NULL)
        args[i++] = (char *)g_variant_get_string(sArray, NULL);

    Arguments *arguments = new Arguments(argc, args);

    applyCLIArgs(*arguments, (ShaderWindowHandler *)user_data);
}

void WindowHandler::setGtkWindow()
{
    GtkWindow *gtk_window = (GtkWindow *)gtk_window_new();
    this->window = gtk_window;

    if (shaderProgram.shaderProps != NULL)
        shaderProgram.shaderProps->setWindowProps(gtk_window);

    else
        shaderProgram.paintableShaderConfig->setWindowProps(gtk_window);

    gtk_window_set_default_size(GTK_WINDOW(gtk_window), shaderProgram.shaderProps != NULL ? shaderProgram.shaderProps->windowWidth : shaderProgram.paintableShaderConfig->windowWidth,
                                shaderProgram.shaderProps != NULL ? shaderProgram.shaderProps->windowHeight : shaderProgram.paintableShaderConfig->windowHeight);
}

void ShaderWindowHandler::setGLArea()
{
    GtkWidget *glArea = gtk_gl_area_new();

    gtk_gl_area_set_allowed_apis((GtkGLArea *)glArea, GDK_GL_API_GL);
    gtk_widget_add_css_class(glArea, shaderProgram.shaderProps->className);
    gtk_gl_area_set_auto_render((GtkGLArea *)glArea, false);

    this->glArea = glArea;
}

void ShaderWindowHandler::handleGDBusSignals(GDBusConnection *conn)
{
    g_dbus_connection_signal_subscribe(conn, NULL, std::string("com.r00n1l." + std::string(shaderProgram.shaderProps->className)).c_str(), "UniformValue", NULL, NULL, G_DBUS_SIGNAL_FLAGS_NONE, handleUniforms, &shaderProgram.uniformValues, NULL);
    g_dbus_connection_signal_subscribe(conn, NULL, std::string("com.r00n1l." + std::string(shaderProgram.shaderProps->className)).c_str(), "ClassCLI", NULL, NULL, G_DBUS_SIGNAL_FLAGS_NONE, handleCLI, this, NULL);
    g_dbus_connection_signal_subscribe(conn, NULL, std::string("com.r00n1l.allcli").c_str(), "AllCLI", NULL, NULL, G_DBUS_SIGNAL_FLAGS_NONE, handleCLI, this, NULL);
}

void ShaderWindowHandler::setupPaintables(GDBusConnection *conn)
{
    if (paintableWindows != NULL)
    {
        PaintableWindowHandler *currentPaintableConfig = paintableWindows[0];
        int paintableConfigIterator = 0;

        while (paintableConfigIterator < paintableWindowsCount)
        {

            currentPaintableConfig->setup(conn, (void *)glArea);

            currentPaintableConfig = paintableWindows[++paintableConfigIterator];
        }
    }
}

void ShaderWindowHandler::setup(GDBusConnection *conn, void *args = NULL)
{
    setGtkWindow();

    setGLArea();

    setRendererSignals();

    displayWindow();

    handleGDBusSignals(conn);

    setupPaintables(conn);
}

void ShaderWindowHandler::disableInputs()
{
    gtk_widget_set_can_target(GTK_WIDGET(window), false);
    gtk_widget_set_can_target(glArea, false);
    gtk_widget_set_can_focus(GTK_WIDGET(window), false);
    gtk_widget_set_can_focus(glArea, false);

    gdk_surface_set_input_region(
        gtk_native_get_surface(gtk_widget_get_native(GTK_WIDGET(window))),
        region);

    gdk_surface_set_input_region(
        gtk_native_get_surface(gtk_widget_get_native(GTK_WIDGET(glArea))),
        region);
}

void ShaderWindowHandler::displayWindow()
{
    gtk_window_set_child(window, (GtkWidget *)glArea);
    gtk_widget_set_visible((GtkWidget *)glArea, true);
    gtk_window_present(window);

    disableInputs();
}
