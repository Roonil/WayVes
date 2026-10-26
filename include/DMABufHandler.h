#ifndef DMA_BUF_HANDLER_H
#define DMA_BUF_HANDLER_H

#define EGL_EGLEXT_PROTOTYPES
#include "DMABufferMessage.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <drm/drm_fourcc.h>
#include <fcntl.h>

#include "stdint.h"
#include <mutex>
#include <string>
#include <sys/un.h>
#include <thread>

class DMABufHandler {
    unsigned int* exportTexture;
    int windowWidth, windowHeight;

    int fileDescriptor = -1;

    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLImage eglImage;

    std::thread receiverThread;
    int wakeFd = -1;

    DMABufferMessageSender* sender = NULL;
    DMABufferMessage* message = NULL;

    const EGLAttrib attrs[3] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };

    bool createImage();
    void exportImage();
    void destroyImage();

    void receiverAvailable();

    std::mutex sendMutex;
    void sendCurrentBuffer();

#define CASE_STR(value) \
    case value:         \
        return #value;
    const char* eglGetErrorString(EGLint error)
    {
        switch (error) {
            CASE_STR(EGL_SUCCESS)
            CASE_STR(EGL_NOT_INITIALIZED)
            CASE_STR(EGL_BAD_ACCESS)
            CASE_STR(EGL_BAD_ALLOC)
            CASE_STR(EGL_BAD_ATTRIBUTE)
            CASE_STR(EGL_BAD_CONTEXT)
            CASE_STR(EGL_BAD_CONFIG)
            CASE_STR(EGL_BAD_CURRENT_SURFACE)
            CASE_STR(EGL_BAD_DISPLAY)
            CASE_STR(EGL_BAD_SURFACE)
            CASE_STR(EGL_BAD_MATCH)
            CASE_STR(EGL_BAD_PARAMETER)
            CASE_STR(EGL_BAD_NATIVE_PIXMAP)
            CASE_STR(EGL_BAD_NATIVE_WINDOW)
            CASE_STR(EGL_CONTEXT_LOST)
        default:
            return "Unknown";
        }
    }
#undef CASE_STR

    int clientFd;

    int sock;
    bool hasActiveReceiver = false;

    struct sockaddr_un addr = { 0 };
    std::string socketPath;

public:
    DMABufHandler(const char* dmaBufPath);
    DMABufHandler(const DMABufHandler&) = delete;
    DMABufHandler& operator=(const DMABufHandler&) = delete;

    void createThread();
    void setTexture(unsigned int* exportTexture, int windowWidth, int windowHeight);
    ~DMABufHandler();
};

#endif