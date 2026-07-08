#ifndef SHADER_FILES_H
#define SHADER_FILES_H

#include <map>
#include <string>

#include "Shaders.h"
#include <atomic>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <string.h>

class ShaderFiles {
private:
    std::string LoadFile(std::string nm);
    std::size_t getIdx(std::string line, std::size_t startIdx, bool isSpace);

    std::string process_variable_replacement(std::string contentString, const std::map<std::string, std::string> variables, std::map<std::string, int>& vars);
    std::string process_includes(std::string content, std::string directory, std::string configName, std::map<std::string, std::string>& overridesMap, std::map<std::string, int>& vars);
    std::string process_expands(std::string content, const std::map<std::string, int>& variables, std::string errorContext);
    std::string extractDirectory(std::string fileName, bool isFile);

    std::string directoryPrefix;
    std::string shaderName;
    std::string configName;
    enum ShaderTypes shaderType;
    std::map<std::string, std::string> overridesMap;
    std::map<std::string, int> vars;

public:
    std::string fileContent;
    std::set<std::string> includedFiles;

    ShaderFiles* next = NULL;
    void reloadFile();
    int index = -1;

    std::atomic<bool> fileIsDirty = false;

    void loadShaders(std::string shaderName, std::string configName, std::string directoryPrefix, enum ShaderTypes shaderType, std::map<std::string, std::string> overridesMap, std::map<std::string, int> vars);
};

#endif