#ifndef FILES_WATCHER_H
#define FILES_WATCHER_H

#include "ShaderFiles.h"
#include <atomic>
#include <set>
#include <thread>
#include <unordered_map>

class FilesWatcher {
private:
    int inotifyFd;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    std::unordered_map<int, std::string> watchDescriptorToFileName;
    std::unordered_map<std::string, int> fileNameToWatchDescriptor;

    std::unordered_map<std::string, std::set<ShaderFiles*>> fileNameToShaderFiles;

    std::thread watchThread;
    std::atomic_bool running;

public:
    void addDependency(
        const std::string& fileName,
        ShaderFiles* file);

    void removeDependency(const std::string& fileName, ShaderFiles* file);

    void createThread();

    void start();
    void stop();

private:
    static void watchLoop(FilesWatcher* filesWatcher);
    void notifyListenersForFileName(std::string fileName);
};

#endif