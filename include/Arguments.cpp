#include "Arguments.h"
#include "Errors.h"

Arguments::Arguments(int argc, char *args[])
{
    this->argc = argc;
    this->args = args;

    argp_parse(&argParser, argc, args, 0, 0, this);
}

error_t Arguments::parseOptions(int key, char *arg, struct argp_state *state)
{

    Arguments *arguments = (Arguments *)state->input;
    switch (key)
    {
    case 'f':
        arguments->configFileName = arg;
        break;

    case 'v':
        arguments->visibility = new int;
        *arguments->visibility = arg == NULL ? -1 : (int)strtol(arg, NULL, 10);
        break;

    case 'V':
        arguments->version = (char *)"WayVes Version 1.0.0";
        break;

    case 'c':
        arguments->className = arg;
        break;

    case 'l':
        arguments->marginLeft = new int;

        *arguments->marginLeft = (int)strtol(arg, NULL, 10);
        break;

    case 'r':
        arguments->marginRight = new int;

        *arguments->marginRight = (int)strtol(arg, NULL, 10);
        break;

    case 't':
        arguments->marginTop = new int;

        *arguments->marginTop = (int)strtol(arg, NULL, 10);
        break;

    case 'b':
        arguments->marginBottom = new int;

        *arguments->marginBottom = (int)strtol(arg, NULL, 10);
        break;

    case 's':
        arguments->anchorLeft = new int;

        *arguments->anchorLeft = (int)strtol(arg, NULL, 10);
        break;
    case 'p':
        arguments->anchorRight = new int;

        *arguments->anchorRight = (int)strtol(arg, NULL, 10);
        break;
    case 'u':
        arguments->anchorTop = new int;

        *arguments->anchorTop = (int)strtol(arg, NULL, 10);
        break;
    case 'd':
        arguments->anchorBottom = new int;

        *arguments->anchorBottom = (int)strtol(arg, NULL, 10);
        break;

    case 'z':
        arguments->layer = new int;

        *arguments->layer = (int)strtol(arg, NULL, 10);
        break;

    case 'w':
        arguments->windowWidth = new int;

        *arguments->windowWidth = (int)strtol(arg, NULL, 10);
        break;

    case 'h':
        arguments->windowHeight = new int;

        *arguments->windowHeight = (int)strtol(arg, NULL, 10);
        break;

    case 'x':
        arguments->exclusiveLayer = new int;
        *arguments->exclusiveLayer = (int)strtol(arg, NULL, 10);
        *arguments->exclusiveLayer = std::max(0, std::min(1, *arguments->exclusiveLayer));
        break;

    case ARGP_KEY_END:
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

bool Arguments::fileNameWithOtherArgs()
{
    return configFileName != NULL &&
           (anchorBottom != NULL ||
            anchorTop != NULL ||
            anchorLeft != NULL ||
            anchorRight != NULL ||
            className != NULL ||
            layer != NULL ||
            marginBottom != NULL ||
            marginLeft != NULL ||
            marginRight != NULL ||
            marginTop != NULL ||
            visibility != NULL ||
            windowHeight != NULL ||
            windowWidth != NULL ||
            version != NULL);
}

void Arguments::signalGApps()
{
    if (fileNameWithOtherArgs())
    {
        Errors::throwError("Cannot specify Config File along with other Parameters");
        std::exit(1);
    }

    if (version != NULL)
    {
        std::cout << version << "\n";
        std::exit(0);
    }

    if (configFileName != NULL || argc == 1)
        return;

    gdBusHandler = new GDBusHandler;
    gdBusHandler->emitCLIArgs(std::string(className == NULL ? "" : className), args);

    std::exit(0);
}
