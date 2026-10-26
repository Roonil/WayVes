#ifndef ERRORS_H
#define ERRORS_H

#include <iostream>
#include <string>

class Errors {
public:
    static void throwError(std::string errorMessage, std::string errorContext = "", std::string prefix = "", int tolerable = 0)
    {
        std::cerr << (tolerable == 0 ? "Error: " : "Warning: ");

        if (!prefix.empty())
            std::cerr << prefix;

        if (!errorContext.empty())
            std::cerr << (prefix.empty() ? "" : " ") << errorContext;

        if (!prefix.empty())
            std::cerr << ", ";
        else if (!errorContext.empty())
            std::cerr << " ";

        std::cerr << errorMessage << "\n";
        if (tolerable == 0)
            std::exit(EXIT_FAILURE);
    }
};

#endif