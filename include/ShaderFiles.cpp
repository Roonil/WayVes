#include "ShaderFiles.h"
#include "Errors.h"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

std::string ShaderFiles::LoadFile(std::string fileName)
{
    std::string shaderCode;
    std::ifstream shaderFile;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        fs::path path = fs::path(directoryPrefix) / fs::path(fileName);

        shaderFile.open(path);

        std::stringstream shaderStream, fShaderStream;
        shaderStream << shaderFile.rdbuf();

        shaderFile.close();

        shaderCode = shaderStream.str();
    } catch (std::ifstream::failure& e) {
        return "";
    }

    fs::path path = fs::path(directoryPrefix) / fs::path(fileName);
    includedFiles.insert(path.lexically_normal().generic_string());

    return shaderCode;
}

std::size_t ShaderFiles::getIdx(std::string line, std::size_t startIdx, bool isSpace)
{
    while (
        (line[startIdx - 1] != '\0' && (isSpace ? (isspace(line[startIdx - 1])) : (!isspace(line[startIdx - 1]))))) {
        startIdx++;
    }

    return startIdx;
}

std::string ShaderFiles::process_variable_replacement(
    std::string contentString,
    const std::map<std::string, std::string> variables,
    std::map<std::string, int>& vars)
{
    const char* define_directive = "#define";

    std::string result = "";
    std::istringstream f(contentString);
    std::string line;

    while (std::getline(f, line)) {
        std::size_t lineLength = line.length();
        std::string::size_type start_pos = 0;

        std::string modifiedLine = line + "\n";

        while (
            std::string::npos != (start_pos = line.find(define_directive, start_pos))) {
            std::size_t defineIdx = start_pos + strlen(define_directive) - 1;

            defineIdx = getIdx(line, defineIdx, false);

            std::size_t variableNameStartIdx = getIdx(line, defineIdx, true);

            std::size_t variableNameEndIdx = getIdx(line, variableNameStartIdx, false);

            if (variableNameStartIdx >= lineLength)
                break;

            std::string variableName = line.substr(
                variableNameStartIdx - 1,
                variableNameEndIdx - variableNameStartIdx);

            std::size_t variableValueStartIdx = getIdx(line, variableNameEndIdx, true);

            std::size_t variableValueEndIdx = lineLength;

            if (variableValueStartIdx > lineLength)
                break;

            auto it = variables.find(variableName);

            if (it != variables.end()) {
                std::string overridenVariableValue = it->second.data();

                std::string newLine = line.substr(0, defineIdx) + variableName + " " + overridenVariableValue + "\n";

                modifiedLine = newLine;

                try {
                    vars[variableName] = stoi(overridenVariableValue);
                } catch (std::exception& err) {
                    break;
                }
            } else {
                try {
                    std::string variableValue = line.substr(
                        variableValueStartIdx - 1,
                        variableValueEndIdx - variableValueStartIdx + 1);

                    vars[variableName] = stoi(variableValue);
                } catch (std::exception& err) {
                    break;
                }
            }

            break;
        }

        result += modifiedLine;
    }

    return result;
}

std::string ShaderFiles::extractDirectory(std::string fileName, bool isFile)
{
    fs::path path(fileName);

    path = path.lexically_normal();

    if (isFile)
        return path.generic_string();

    fs::path directory = path.parent_path();

    if (directory.empty())
        return "";

    std::string result = directory.generic_string();

    if (result.back() != '/')
        result += '/';

    return result;
}

std::string ShaderFiles::process_includes(
    std::string content,
    std::string directory,
    std::string configName,
    std::map<std::string, std::string>& overridesMap,
    std::map<std::string, int>& vars)
{
    if (content.empty())
        return "";

    const std::string include_directive = "#include";
    const std::string wildCardName = ":$CONFIGFILE";

    std::istringstream f(content);
    std::string result = "";
    std::string line;

    while (std::getline(f, line)) {
        std::string::size_type start_pos = 0;
        std::string modifiedLine = line + "\n";

        while (
            std::string::npos != (start_pos = line.find(include_directive, start_pos))) {
            std::string::size_type startIdx = line.find("\"", start_pos);

            if (startIdx == std::string::npos)
                break;

            std::string::size_type endIdx = line.find("\"", startIdx + 1);

            if (endIdx == std::string::npos)
                break;

            std::string fileName = line.substr(
                startIdx + 1,
                endIdx - startIdx - 1);

            if (fileName == wildCardName) {
                fileName = configName;
                directory = "";
            }

            else if (fileName == wildCardName.substr(1)) {
                fileName = configName;
            }

            if (!fileName.empty() && fileName[0] == ':') {
                fileName = fileName.substr(1);
            } else {
                fileName = (fs::path(directory) / fileName)
                               .generic_string();
            }

            fileName = extractDirectory(fileName, true);

            std::string includedFile = LoadFile(fileName);

            if (includedFile.empty()) {
                Errors::throwError(
                    "does not exist, or has empty contents!",
                    fileName,
                    "",
                    1);
            }

            std::string extractedDirectory = extractDirectory(fileName, false);

            includedFile = process_includes(
                includedFile,
                extractedDirectory,
                configName,
                overridesMap,
                vars);

            includedFile = process_variable_replacement(
                includedFile,
                overridesMap,
                vars);

            modifiedLine = includedFile;

            break;
        }

        result += modifiedLine;
    }

    return result;
}

std::string ShaderFiles::process_expands(
    std::string content,
    const std::map<std::string, int>& variables,
    std::string errorContext)
{
    if (content.empty())
        return "";

    const std::string expand_directive = "#expand";

    std::string result = "";
    std::istringstream f(content);
    std::string line;

    while (std::getline(f, line)) {
        std::size_t lineLength = line.length();
        std::string::size_type start_pos = 0;

        std::string modifiedLine = line + "\n";

        while (
            std::string::npos != (start_pos = line.find(expand_directive, start_pos))) {
            std::size_t expandIdx = start_pos + expand_directive.length() - 1;

            expandIdx = getIdx(line, expandIdx, false);

            std::size_t functionNameStartIdx = getIdx(line, expandIdx, true);

            std::size_t functionNameEndIdx = getIdx(line, functionNameStartIdx, false);

            if (functionNameStartIdx >= lineLength)
                break;

            std::string functionName = line.substr(
                functionNameStartIdx - 1,
                functionNameEndIdx - functionNameStartIdx);

            std::size_t valueStartIdx = getIdx(line, functionNameEndIdx, true);

            std::size_t valueEndIdx = lineLength;

            if (valueStartIdx > lineLength)
                break;

            std::string valueString = line.substr(
                valueStartIdx - 1,
                valueEndIdx - valueStartIdx + 1);

            int value;

            if (std::isdigit(valueString[0]))
                value = stoi(valueString);
            else {
                auto it = variables.find(valueString);

                if (it != variables.end()) {
                    value = it->second;
                } else {
                    Errors::throwError(
                        "Variable '" + valueString + "' not found for expansion.",
                        errorContext,
                        "In");

                    break;
                }
            }

            std::string expandedFunctionString = "";

            for (int i = 0; i < value; i++) {
                expandedFunctionString += functionName;
                expandedFunctionString += "(";
                expandedFunctionString += std::to_string(i);
                expandedFunctionString += ");\n";
            }

            modifiedLine = expandedFunctionString;

            break;
        }

        result += modifiedLine;
    }

    return result;
}

void ShaderFiles::reloadFile()
{
    std::string directory;
    std::string shaderStringFormat = "";

    switch (shaderType) {

    case VERTEX: {
        directory = (fs::path(shaderName) / "vertex").generic_string() + "/";
        shaderStringFormat = ".vert";
        break;
    }

    case FRAGMENT: {
        directory = (fs::path(shaderName) / "fragment").generic_string() + "/";
        shaderStringFormat = ".frag";
        break;
    }

    case COMPUTE: {
        directory = (fs::path(shaderName) / "compute").generic_string() + "/";
        shaderStringFormat = ".comp";
        break;
    }

    default:
        break;
    }

    std::string shaderFile = (fs::path(directory) / (std::to_string(index) + shaderStringFormat)).generic_string();

    fileContent = LoadFile(shaderFile);

    if (fileContent.empty()) {
        if (index == 1 && shaderType == ShaderTypes::FRAGMENT)
            Errors::throwError(
                "Shader '" + shaderName + "' not found.",
                "",
                "");

        return;
    }

    fileContent = process_includes(
        fileContent,
        directory,
        configName,
        overridesMap,
        vars);

    fileContent = process_expands(
        fileContent,
        vars,
        shaderName);
}

void ShaderFiles::loadShaders(
    std::string shaderName,
    std::string configName,
    std::string directoryPrefix,
    enum ShaderTypes shaderType,
    std::map<std::string, std::string> overridesMap,
    std::map<std::string, int> vars)
{
    ShaderFiles* shaderFile = this;
    ShaderFiles* prevFile = NULL;

    std::string directory;
    int idx = 1;

    std::string shaderStringFormat = "";

    switch (shaderType) {

    case VERTEX: {
        directory = (fs::path(shaderName) / "vertex").generic_string() + "/";
        shaderStringFormat = ".vert";
        break;
    }

    case FRAGMENT: {
        directory = (fs::path(shaderName) / "fragment").generic_string() + "/";
        shaderStringFormat = ".frag";
        break;
    }

    case COMPUTE: {
        directory = (fs::path(shaderName) / "compute").generic_string() + "/";
        shaderStringFormat = ".comp";
        break;
    }

    default:
        break;
    }

    do {
        shaderFile->directoryPrefix = directoryPrefix;
        shaderFile->shaderName = shaderName;
        shaderFile->configName = configName;
        shaderFile->shaderType = shaderType;
        shaderFile->overridesMap = overridesMap;
        shaderFile->vars = vars;
        shaderFile->index = idx;

        std::string shaderFileName = (fs::path(directory) / (std::to_string(idx) + shaderStringFormat)).generic_string();

        shaderFile->fileContent = shaderFile->LoadFile(shaderFileName);

        if (shaderFile->fileContent.empty()) {

            if (idx == 1 && shaderType == ShaderTypes::FRAGMENT)
                Errors::throwError(
                    "Shader '" + shaderName + "' not found.",
                    "",
                    "");

            if (prevFile != NULL) {
                delete prevFile->next;
                prevFile->next = NULL;
            }

            return;
        }

        shaderFile->fileContent = shaderFile->process_includes(
            shaderFile->fileContent,
            directory,
            configName,
            overridesMap,
            vars);

        shaderFile->fileContent = shaderFile->process_expands(
            shaderFile->fileContent,
            vars,
            shaderName);

        shaderFile->next = new ShaderFiles;

        prevFile = shaderFile;
        shaderFile = shaderFile->next;

        idx++;

    } while (true);
}