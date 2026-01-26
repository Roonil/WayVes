#include "NamedPipeHandler.h"
#include "Errors.h"

#include <sys/socket.h>
#include <filesystem>
#include <sys/un.h>
#include <algorithm>

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

NamedPipeHandler::NamedPipeHandler(std::vector<std::string> classNames, std::string instance, std::string *configFileNamePointer)
{
    this->instance = instance;
    this->configFileNamePointer = configFileNamePointer;

    this->classNames = classNames;
    fds = new pollfd[classNames.size() + 1];

    dBusHandler = new DBusHandler();
}

void NamedPipeHandler::createPipes()
{
    createFIFOAtPath(pipeDirectory + "instance_" + instance);

    fds[0].fd = openFIFOFromPath(pipeDirectory + "instance_" + instance);
    fds[0].events = POLLIN;

    for (int i = 1; i < classNames.size() + 1; i++)
    {
        createFIFOAtPath(pipeDirectory + classNames.at(i - 1));

        fds[i].fd = openFIFOFromPath(pipeDirectory + classNames.at(i - 1));
        fds[i].events = POLLIN;
    }
}

void NamedPipeHandler::handleInstance(char *buffer)
{
    std::stringstream bufferStream;
    bufferStream << buffer;

    std::string line;

    while (std::getline(bufferStream, line, '\n'))
    {

        int equalsIdx = line.find("=");
        if (equalsIdx == std::string::npos)
        {
            Errors::throwError("Invalid format in Piped data", "", "", 1);
            continue;
        }

        std::string operationName = line.substr(0, equalsIdx);
        std::string operationValue = line.substr(equalsIdx + 1);

        operationName.erase(std::remove_if(operationName.begin(), operationName.end(), ::isspace), operationName.end());
        operationValue.erase(std::remove_if(operationValue.begin(), operationValue.end(), ::isspace), operationValue.end());

        if (operationName == "reload")
        {
            dBusHandler->emitTearDownForInstance(instance);
            *configFileNamePointer = std::string(operationValue);
        }
    }
}

void NamedPipeHandler::closeFDs()
{
    for (int i = 0; i < classNames.size() + 1; i++)
    {
        close(fds[i].fd);
    }
}

void NamedPipeHandler::listenForPipeInputs()
{
    while (1)
    {
        int ret = poll(fds, classNames.size() + 1, -1);
        if (ret > 0)
        {

            for (int i = 0; i < classNames.size() + 1; i++)
            {

                if (fds[i].revents & POLLIN)
                {

                    char buffer[bufferSize];

                    ssize_t n = read(fds[i].fd, buffer, sizeof(buffer) - 1);

                    if (n == 0)
                    {

                        close(fds[i].fd);

                        if (i != 0)
                            fds[i].fd = openFIFOFromPath(pipeDirectory + classNames.at(i - 1));
                        else
                            fds[i].fd = openFIFOFromPath(pipeDirectory + "instance_" + instance);

                        fds[i].events = POLLIN;
                    }
                    else if (n > 0)

                    {
                        if (i == 0)
                        {
                            buffer[n] = '\0';

                            std::cout << "Tearing Down for reload on instance " << instance << "\n";

                            handleInstance(buffer);
                            closeFDs();
                            return;
                        }
                        else
                        {
                            buffer[n] = '\0';
                            dBusHandler->emitUniforms(classNames.at(i - 1), std::string(buffer));
                        }
                    }
                }

                if (fds[i].revents & POLLHUP)
                {

                    close(fds[i].fd);
                    if (i != 0)
                        fds[i].fd = openFIFOFromPath(pipeDirectory + classNames.at(i - 1));
                    else
                        fds[i].fd = openFIFOFromPath(pipeDirectory + "instance_" + instance);

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