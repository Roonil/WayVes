#include "include/GApplicationHandler.h"

#include "include/ConfigsHandler.h"
#include "include/NamedPipeHandler.h"

#include <argp.h>

#include <iostream>

std::string getDirectoryPrefix()
{

  const char *xdgConfigHome = std::getenv("XDG_CONFIG_HOME");

  if (xdgConfigHome && *xdgConfigHome != '\0')
    return std::string(xdgConfigHome) + "/WayVes/";

  else
  {
    const char *home = std::getenv("HOME");

    if (home && *home != '\0')
      return std::string(home) + "/.config/WayVes/";
    else
      Errors::throwError("HOME environment variable not set");
  }

  return "";
}

int main(int argc, char *args[])
{

  Arguments *arguments = new Arguments(argc, args);
  arguments->signalGApps();

  char *configFileName = arguments->configFileName;

  std::string instance = std::string(arguments->instance == NULL ? "" : arguments->instance);

  if (instance.empty())
    instance = "default";

  std::string overrideFile = "";

  while (true)
  {

    ConfigsHandler *configsHandler = new ConfigsHandler(overrideFile.empty() ? configFileName : (char *)overrideFile.c_str(), getDirectoryPrefix());
    configsHandler->initialiseGTKApps(instance);

    NamedPipeHandler *namedPipeHandler = new NamedPipeHandler(configsHandler->classNames, instance, &overrideFile);
    namedPipeHandler->pollForPipes();

    delete configsHandler;
    delete namedPipeHandler;
  }
}