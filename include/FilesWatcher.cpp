#include "FilesWatcher.h"
#include <cstring>
#include <linux/limits.h>
#include <signal.h>
#include <sys/inotify.h>

void FilesWatcher::addDependency(const std::string& fileName, ShaderFiles* file)
{
    int wd = inotify_add_watch(inotifyFd, (fileName).c_str(), IN_MODIFY | IN_CLOSE_WRITE);
    watchDescriptorToFileName[wd] = fileName;
    fileNameToWatchDescriptor[fileName] = wd;

    fileNameToShaderFiles[fileName].insert(file);
}

void FilesWatcher::removeDependency(const std::string& fileName, ShaderFiles* file)
{

    if (fileNameToWatchDescriptor.find(fileName) == fileNameToWatchDescriptor.end()) {
        Errors::throwError("Attempted to remove untracked file: " + fileName, "[Watcher]", "", 1);
        return;
    }

    int wd = fileNameToWatchDescriptor[fileName];

    fileNameToShaderFiles[fileName].erase(file);

    if (fileNameToShaderFiles[fileName].empty()) {
        watchDescriptorToFileName.erase(wd);
        fileNameToWatchDescriptor.erase(fileName);

        int result = inotify_rm_watch(inotifyFd, wd);
        if (result < 0) {
            Errors::throwError("Kernel failed to remove watch descriptor", "[Watcher]", "", 1);
        }
    }
}

void FilesWatcher::createThread()
{
    pthread_t pThread;
    int threadID;
    pthread_mutex_init(&mutex, NULL);

    sigset_t set;
    sigfillset(&set);

    threadID = pthread_sigmask(SIG_SETMASK, &set, NULL);
    threadID = pthread_create(&pThread, NULL, (void* (*)(void*))(watchLoop),
        (void*)this);

    sigemptyset(&set);

    pthread_sigmask(SIG_SETMASK, &set, NULL);
    pthread_detach(pThread);
}

void FilesWatcher::start()
{
    inotifyFd = inotify_init();
    if (inotifyFd < 0) {
        Errors::throwError("Failed to initialize the shared inotify instance, hot reloading will not work!", "", "", 1);
    }
}

void FilesWatcher::stop()
{
}

void FilesWatcher::watchLoop(FilesWatcher* filesWatcher)
{
    filesWatcher->start();

    constexpr size_t EVENT_SIZE = sizeof(struct inotify_event);
    constexpr size_t BUF_LEN = 1024 * (EVENT_SIZE + NAME_MAX + 1);
    char buffer[BUF_LEN];

    while (true) {
        ssize_t length = read(filesWatcher->inotifyFd, buffer, BUF_LEN);

        if (length < 0) {
            Errors::throwError(std::string("read failed with error: ") + std::strerror(errno), "", "While reading file for reload", 1);
            continue;
        }

        size_t i = 0;
        while (i < static_cast<size_t>(length)) {
            auto* event = reinterpret_cast<struct inotify_event*>(&buffer[i]);

            if (filesWatcher->watchDescriptorToFileName.find(event->wd) != filesWatcher->watchDescriptorToFileName.end()) {
                std::string target_path = filesWatcher->watchDescriptorToFileName[event->wd];

                if ((event->mask & IN_MODIFY) || (event->mask & IN_CLOSE_WRITE)) {
                    filesWatcher->notifyListenersForFileName(target_path);
                }

                if (event->mask & IN_IGNORED) {

                    filesWatcher->watchDescriptorToFileName.erase(event->wd);

                    sched_yield();

                    int new_wd = inotify_add_watch(filesWatcher->inotifyFd, target_path.c_str(), IN_MODIFY | IN_CLOSE_WRITE);
                    if (new_wd < 0 && errno == ENOENT) {
                        sched_yield();
                        new_wd = inotify_add_watch(filesWatcher->inotifyFd, target_path.c_str(), IN_MODIFY | IN_CLOSE_WRITE);
                    }

                    if (new_wd >= 0) {

                        filesWatcher->watchDescriptorToFileName[new_wd] = target_path;
                        filesWatcher->fileNameToWatchDescriptor[target_path] = new_wd;

                        filesWatcher->notifyListenersForFileName(target_path);
                    }
                }
            }

            i += EVENT_SIZE + event->len;
        }
    }
}

void FilesWatcher::notifyListenersForFileName(std::string fileName)
{
    for (auto i : fileNameToShaderFiles[fileName]) {
        i->fileIsDirty.store(true);
    }
}
