#include "NamedPipeHandler.h"
#include "Errors.h"
#include <filesystem>

void NamedPipeHandler::createFIFOAtPath(std::string fifoPath)
{
    std::filesystem::create_directories(std::filesystem::path(pipeDirectory).parent_path());

    if (mkfifo(fifoPath.c_str(), 0666) == -1)
    {
        if (errno != EEXIST)
        {
            perror("mkfifo");
            Errors::throwError("error while creating named pipe", fifoPath, "For");
        }
    }
}
int NamedPipeHandler::openFIFOFromPath(std::string fifoPath)
{
    int fifo_fd = open(fifoPath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fifo_fd == -1)
    {
        perror("open");
        Errors::throwError("error while opening named pipe.", fifoPath, "For");
    }

    return fifo_fd;
}

NamedPipeHandler::NamedPipeHandler(std::vector<std::string> classNames)
{
    this->classNames = classNames;
    gdBusHandler = new GDBusHandler;
    fds = new pollfd[classNames.size()];
}

void NamedPipeHandler::createPipes()
{
    for (int i = 0; i < classNames.size(); i++)
    {
        createFIFOAtPath(pipeDirectory + classNames.at(i));
        int fd = openFIFOFromPath(pipeDirectory + classNames.at(i));
        fds[i].fd = fd;
        fds[i].events = POLLIN;
    }
}

void NamedPipeHandler::listenForPipeInputs()
{
    while (1)
    {
        int ret = poll(fds, classNames.size(), -1);
        if (ret > 0)
        {
            for (int i = 0; i < classNames.size(); i++)
            {

                if (fds[i].revents & POLLIN)
                {

                    char buffer[bufferSize];

                    ssize_t n = read(fds[i].fd, buffer, sizeof(buffer) - 1);

                    if (n > 0)

                    {
                        buffer[n] = '\0';
                        gdBusHandler->emitUniforms(classNames.at(i), std::string(buffer));
                    }
                }

                if (fds[i].revents & POLLHUP)
                {

                    close(fds[i].fd);
                    fds[i].fd = openFIFOFromPath(pipeDirectory + classNames.at(i));
                    fds[i].events = POLLIN;
                }
            }
        }
    }
}

void NamedPipeHandler::pollForPipes()
{
    createPipes();
    listenForPipeInputs();
}