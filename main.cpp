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

  ConfigsHandler configsHandler(configFileName, getDirectoryPrefix());
  configsHandler.initialiseGTKApps();

  NamedPipeHandler namedPipeHandler(configsHandler.classNames);
  namedPipeHandler.pollForPipes();
}