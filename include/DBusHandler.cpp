#include "DBusHandler.h"
#include "Errors.h"

DBusHandler::DBusHandler()
{
    DBusError err;
    dbus_error_init(&err);

    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

    if (dbus_error_is_set(&err))
    {
        Errors::throwError(err.message, "", "Got error while initiating DBus");

        dbus_error_free(&err);
        return;
    }

    if (!conn)
    {
        Errors::throwError("DBus Connection failed");
    }

    this->conn = conn;
}

void DBusHandler::emitUniforms(std::string className, std::string uniforms)
{
    const char *uniformsCChar = uniforms.c_str();

    std::string interfaceName = std::string("com.r00n1l." + className);
    interfaceName.push_back('\0');

    DBusMessage *message = dbus_message_new_signal("/com/r00n1l/audios", interfaceName.data(), "UniformValue");
    dbus_message_append_args(message, DBUS_TYPE_STRING, &uniformsCChar);

    dbus_connection_send(conn, message, NULL);
    dbus_connection_flush(conn);
    dbus_message_unref(message);
}
void DBusHandler::emitCLIArgs(std::string className, int argc, char *args[])
{
    DBusMessage *message = NULL;

    std::string interfaceName = std::string("com.r00n1l." + className);
    interfaceName.push_back('\0');

    if (className.empty())
        message = dbus_message_new_signal("/com/r00n1l/audios", "com.r00n1l.allcli", "AllCLI");
    else
        message = dbus_message_new_signal("/com/r00n1l/audios", interfaceName.data(), "ClassCLI");

    DBusMessageIter iter;
    DBusMessageIter array_iter;

    dbus_message_iter_init_append(message, &iter);

    dbus_message_iter_open_container(
        &iter,
        DBUS_TYPE_ARRAY,
        DBUS_TYPE_STRING_AS_STRING,
        &array_iter);

    for (int i = 0; i < argc; i++)
    {
        const char *val = args[i];
        dbus_message_iter_append_basic(
            &array_iter,
            DBUS_TYPE_STRING,
            &val);
    }

    dbus_message_iter_close_container(&iter, &array_iter);

    dbus_connection_send(conn, message, NULL);
    dbus_connection_flush(conn);
    dbus_message_unref(message);
}

void DBusHandler::emitTearDownForInstance(std::string instance)
{
    const char *str = "";
    DBusError *err = NULL;

    std::string interfaceName = std::string("com.r00n1l." + instance);
    interfaceName.push_back('\0');

    DBusMessage *message = dbus_message_new_signal("/com/r00n1l/audios", interfaceName.data(), "TearDown");
    dbus_message_append_args(message, DBUS_TYPE_STRING, &str);

    dbus_connection_send(conn, message, NULL);
    dbus_connection_flush(conn);
    dbus_message_unref(message);
}

DBusHandler::~DBusHandler()
{
    dbus_connection_close(conn);
}
