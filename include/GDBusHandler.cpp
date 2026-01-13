#include "GDBusHandler.h"

GDBusHandler::GDBusHandler()
{
    conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, NULL);
}

void GDBusHandler::emitUniforms(std::string className, std::string uniforms)
{
    g_dbus_connection_emit_signal(conn, NULL, "/com/r00n1l/audios", std::string("com.r00n1l." + className).c_str(), "UniformValue", g_variant_new("(s)", uniforms.c_str()), NULL);
}
void GDBusHandler::emitCLIArgs(std::string className, char *args[])
{
    GVariant *arrayVariant = g_variant_new_strv(args, -1);

    if (className.empty())
        g_dbus_connection_emit_signal(conn, NULL, "/com/r00n1l/audios", std::string("com.r00n1l.allcli").c_str(), "AllCLI", g_variant_new_tuple(&arrayVariant, 1), NULL);
    else
        g_dbus_connection_emit_signal(conn, NULL, "/com/r00n1l/audios", std::string("com.r00n1l." + className).c_str(), "ClassCLI", g_variant_new_tuple(&arrayVariant, 1), NULL);
}

GDBusHandler::~GDBusHandler()
{

    // g_dbus_connection_close_sync(conn, NULL, NULL);
    // g_object_unref(conn);
}
