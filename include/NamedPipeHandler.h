#ifndef NAMED_PIPE_HANDLER_H
#define NAMED_PIPE_HANDLER_H

#include <string>
#include <poll.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "GDBusHandler.h"
#include <vector>

class NamedPipeHandler
{
private:
    const int bufferSize = 1024;
    struct pollfd *fds = NULL;
    const std::string pipeDirectory = "/tmp/WayVes/";
    GDBusHandler *gdBusHandler = NULL;
    std::vector<std::string> classNames;

    void createFIFOAtPath(std::string fifoPath);
    int openFIFOFromPath(std::string fifoPath);
    void createPipes();
    void listenForPipeInputs();

public:
    NamedPipeHandler(std::vector<std::string> classNames);
    void pollForPipes();
};

#endif