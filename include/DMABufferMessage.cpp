#include "DMABufferMessage.h"
#include "Errors.h"
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

DMABufferMessageSender::DMABufferMessageSender(const char* path, int clientFd)
{
    this->clientFd = clientFd;
    this->path = path;
}

int DMABufferMessageSender::sendMessage(int dmabuf_fd, DMABufferMessage* message)
{
    std::cout << "Sending Message: " << clientFd << "  " << dmabuf_fd << "\n";
    if (clientFd < 0) {
        if (dmabuf_fd > 0)
            close(dmabuf_fd);

        return -1;
    }

    struct iovec iov = {
        .iov_base = message,
        .iov_len = sizeof(*message)
    };

    char control[CMSG_SPACE(sizeof(int))];
    memset(control, 0, sizeof(control));

    struct msghdr msg = { 0 };

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    memcpy(CMSG_DATA(cmsg), &dmabuf_fd, sizeof(int));

    if (sendmsg(clientFd, &msg, 0) < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            Errors::throwError(std::string("failed to send DMA-BUF to receiver: ") + strerror(errno), path, "For", 1);
        if (dmabuf_fd > 0)
            close(dmabuf_fd);

        return -1;
    }

    if (dmabuf_fd > 0)
        close(dmabuf_fd);

    std::cout << "Message Sent!\n";
    return 0;
}

DMABufferMessageSender::~DMABufferMessageSender()
{
    if (clientFd >= 0) {
        close(clientFd);
    }
}
