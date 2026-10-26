
#include "DMABufHandler.h"
#include "Errors.h"
#include <cstring>
#include <filesystem>
#include <iostream>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

bool DMABufHandler::createImage()
{
    eglDisplay = eglGetCurrentDisplay();
    eglContext = eglGetCurrentContext();

    eglImage = eglCreateImage(eglDisplay, eglContext, EGL_GL_TEXTURE_2D_KHR, ((EGLClientBuffer)(uintptr_t)*exportTexture), attrs);
    EGLint err = eglGetError();

    if (eglImage == EGL_NO_IMAGE || err != EGL_SUCCESS) {
        Errors::throwError(std::string("failed to create EGL image for DMA-BUF export (") + eglGetErrorString(err) + ")", socketPath, "For", 1);
        return false;
    }

    return true;
}
void DMABufHandler::exportImage()
{

    PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC eglExportDMABUFImageQueryMESA = (PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC)eglGetProcAddress("eglExportDMABUFImageQueryMESA");

    PFNEGLEXPORTDMABUFIMAGEMESAPROC eglExportDMABUFImageMESA = (PFNEGLEXPORTDMABUFIMAGEMESAPROC)eglGetProcAddress("eglExportDMABUFImageMESA");

    if (eglExportDMABUFImageMESA == NULL || eglExportDMABUFImageQueryMESA == NULL)
        Errors::throwError("EGL_MESA_image_dma_buf_export is not supported by this driver, so rendering-sockets cannot be used", socketPath, "For");

    int numPlanes = 0;
    if (!eglExportDMABUFImageQueryMESA(eglDisplay, eglImage, &message->pixelFormat, &numPlanes, &message->modifier)) {
        Errors::throwError(std::string("failed to query DMA-BUF export (") + eglGetErrorString(eglGetError()) + ")", socketPath, "For", 1);
        fileDescriptor = -1;
        return;
    }

    if (numPlanes != 1) {
        Errors::throwError("DMA-BUF export needs " + std::to_string(numPlanes) + " planes, but rendering-sockets only support 1", socketPath, "For", 1);
        fileDescriptor = -1;
        return;
    }

    if (!eglExportDMABUFImageMESA(eglDisplay, eglImage, &fileDescriptor, &message->stride, &message->offset)) {
        Errors::throwError(std::string("failed to export DMA-BUF from EGL image (") + eglGetErrorString(eglGetError()) + ")", socketPath, "For", 1);
        fileDescriptor = -1;
        return;
    }
}
void DMABufHandler::destroyImage()
{
    PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");

    eglDestroyImageKHR(eglDisplay, eglImage);
}

void DMABufHandler::receiverAvailable()
{
    struct pollfd pfds[2];
    struct pollfd& pfd = pfds[1];

    pfds[0].fd = wakeFd;
    pfds[0].events = POLLIN;

    while (true) {
        if (!hasActiveReceiver) {
            pfd.fd = sock;
            pfd.events = POLLIN;
        } else {
            pfd.fd = clientFd;
            pfd.events = POLLIN | POLLHUP | POLLRDHUP;
        }

        int activity = poll(pfds, 2, -1);

        if (activity < 0) {
            if (errno == EINTR)
                continue;
            Errors::throwError(std::string("stopped watching for DMA-BUF receivers: ") + std::strerror(errno), socketPath, "For", 1);
            break;
        }

        if (pfds[0].revents & POLLIN)
            break;

        if (!hasActiveReceiver) {
            if (pfd.revents & POLLIN) {
                int new_client = accept4(sock, nullptr, nullptr, SOCK_CLOEXEC);
                if (new_client >= 0) {
                    std::lock_guard<std::mutex> lock(sendMutex);

                    clientFd = new_client;
                    hasActiveReceiver = true;

                    fcntl(clientFd, F_SETFL, O_NONBLOCK);
                    printf("Receiver connected! Ready to stream DMA-Buf frames.\n");

                    delete sender;
                    sender = new DMABufferMessageSender(socketPath.c_str(), clientFd);
                    sendCurrentBuffer();
                }
            }
        } else {
            if (pfd.revents & (POLLIN | POLLHUP | POLLRDHUP)) {
                char dummy;
                ssize_t res = recv(clientFd, &dummy, 1, MSG_PEEK | MSG_DONTWAIT);

                if (res == 0 || (res < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
                    std::cout << "DMABUF receiver disconnected or terminated.\n";

                    std::lock_guard<std::mutex> lock(sendMutex);

                    delete sender;
                    sender = NULL;
                    clientFd = -1;
                    hasActiveReceiver = false;
                }
            }
        }
    }
}

DMABufHandler::DMABufHandler(const char* dmaBufPath)
{
    socketPath = std::string(dmaBufPath);
    std::filesystem::create_directories(std::filesystem::path(socketPath).parent_path());

    clientFd = -1;
    hasActiveReceiver = false;

    std::cout << "Initialising Socket...\n";
    sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    if (sock < 0) {
        Errors::throwError(std::string("failed to create DMA-BUF socket: ") + std::strerror(errno), socketPath, "For", 1);
        return;
    }

    fcntl(sock, F_SETFL, O_NONBLOCK);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, dmaBufPath, sizeof(addr.sun_path) - 1);

    unlink(dmaBufPath);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        Errors::throwError(std::string("failed to bind DMA-BUF socket: ") + std::strerror(errno), socketPath, "For", 1);
        close(sock);
        sock = -1;
        return;
    }

    if (listen(sock, 5) < 0) {
        Errors::throwError(std::string("failed to listen on DMA-BUF socket: ") + std::strerror(errno), socketPath, "For", 1);
        close(sock);
        sock = -1;
        return;
    }
}

void DMABufHandler::createThread()
{
    if (sock < 0)
        return;

    wakeFd = eventfd(0, EFD_CLOEXEC);
    if (wakeFd < 0) {
        Errors::throwError(std::string("failed to create the receiver thread's wake-up eventfd, so no receiver can connect: ") + std::strerror(errno), socketPath, "For", 1);
        return;
    }

    receiverThread = std::thread(&DMABufHandler::receiverAvailable, this);
}

void DMABufHandler::setTexture(unsigned int* exportTexture, int windowWidth, int windowHeight)
{

    {
        std::lock_guard<std::mutex> lock(sendMutex);

        this->exportTexture = exportTexture;
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;

        if (message != NULL)
            delete message;

        message = new DMABufferMessage(windowWidth, windowHeight);

        if (fileDescriptor >= 0)
            close(fileDescriptor);
        fileDescriptor = -1;

        if (createImage()) {
            exportImage();
            destroyImage();
        }

        if (clientFd != -1) {
            if (sender == NULL)
                sender = new DMABufferMessageSender(socketPath.c_str(), clientFd);
            sendCurrentBuffer();
        }
    }

    if (!receiverThread.joinable())
        createThread();
}

void DMABufHandler::sendCurrentBuffer()
{
    if (sender == NULL || clientFd < 0 || fileDescriptor < 0 || message == NULL)
        return;

    int sendFd = fcntl(fileDescriptor, F_DUPFD_CLOEXEC, 0);
    if (sendFd < 0) {
        Errors::throwError(std::string("failed to duplicate DMA-BUF fd for sending: ") + std::strerror(errno), socketPath, "For", 1);
        return;
    }

    sender->sendMessage(sendFd, message);
}

DMABufHandler::~DMABufHandler()
{
    if (receiverThread.joinable()) {
        uint64_t wake = 1;
        if (write(wakeFd, &wake, sizeof(wake)) < 0)
            Errors::throwError(std::string("failed to wake the receiver thread: ") + std::strerror(errno), socketPath, "For", 1);
        receiverThread.join();
    }

    if (wakeFd >= 0)
        close(wakeFd);

    if (sock >= 0)
        close(sock);

    exportTexture = NULL;

    if (message != NULL)
        delete message;

    if (sender != NULL)
        delete sender;

    if (fileDescriptor >= 0)
        close(fileDescriptor);
}