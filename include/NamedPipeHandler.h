#ifndef NAMED_PIPE_HANDLER_H
#define NAMED_PIPE_HANDLER_H

#include <string>
#include <poll.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "DBusHandler.h"
#include "ConfigsHandler.h"

#include <vector>

class NamedPipeHandler
{
private:
    const int bufferSize = 1024;
    struct pollfd *fds = NULL;
    const std::string pipeDirectory = "/tmp/WayVes/";
    std::string *configFileNamePointer = NULL;

    DBusHandler *dBusHandler = NULL;

    std::vector<std::string> classNames;
    std::string instance;

    void createFIFOAtPath(std::string fifoPath);
    int openFIFOFromPath(std::string fifoPath);
    void handleInstance(char *buffer);
    void closeFDs();
    void createPipes();
    void listenForPipeInputs();

public:
    NamedPipeHandler(std::vector<std::string> classNames, std::string instance, std::string *configFileNamePointer);
    void pollForPipes();
};

#endif