#ifndef DMA_BUF_MESSAGE_H
#define DMA_BUF_MESAGE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <string>

class DMABufferMessage {
public:
    int width = 0, height = 0, pixelFormat = 0;
    EGLint stride = 0, offset = 0;
    EGLuint64KHR modifier = 0;

    DMABufferMessage(int width, int height)
    {
        this->width = width;
        this->height = height;
    }
};

class DMABufferMessageSender {

    int clientFd;
    std::string path;

public:
    DMABufferMessageSender(const char* path, int clientFd);
    int sendMessage(int dmabuf_fd, DMABufferMessage* message);
    ~DMABufferMessageSender();
};

#endif