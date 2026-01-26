#include "GApplicationHandler.h"

void GApplicationHandler::rendererThread(gpointer data)
{

  ShaderWindowHandler *windowHandler = (ShaderWindowHandler *)data;
  double ts = 0;
  double ls = 0;
  double sleepTime = 0;
  double fps = (double)(windowHandler->shaderProgram.fps);
  double minFrameTime = 1.0 / fps;
  struct timespec request = {0, 0};

  while (true)
  {

    if (GTK_IS_GL_AREA(windowHandler->glArea))
      g_idle_add_once((GSourceOnceFunc)gtk_gl_area_queue_render, windowHandler->glArea);

    ts = g_timer_elapsed(timer, NULL);

    if (ls == 0)
      ls = ts;

    double deltaTime = ts - ls;

    ls = ts;

    if (minFrameTime > 0)
    {
      sleepTime += minFrameTime - deltaTime;

      sleepTime = std::max(0.0, sleepTime);
      request.tv_nsec =
          (int)(sleepTime * 1000000000.0);

      if (request.tv_nsec > 0)
        nanosleep(&request, NULL);
    }
  }
}

void GApplicationHandler::activateApplication(Configs *configs, GDBusConnection *conn)
{
  configs->windowHandler->shaderProgram.pipeWireSetting->createPipeWireThread();

  timer = g_timer_new();

  std::string stylesCSSPath = (configs->directoryPrefix + "styles.css");

  GtkCssProvider *cssProvider = gtk_css_provider_new();
  GtkCssProvider *cssProviderGlobal = gtk_css_provider_new();

  gtk_css_provider_load_from_string(cssProviderGlobal, "* { background-color: transparent; background-image: none;}");
  gtk_css_provider_load_from_path(cssProvider, stylesCSSPath.c_str());

  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                             GTK_STYLE_PROVIDER(cssProviderGlobal),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);

  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                             GTK_STYLE_PROVIDER(cssProvider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);

  Configs *configsIterator = configs;

  while (configsIterator != NULL)
  {

    ShaderWindowHandler *windowHandler = configsIterator->windowHandler;

    windowHandler->setup(conn, configs->instance, NULL);

    g_thread_new("renderer", (GThreadFunc)rendererThread, windowHandler);

    configsIterator = configsIterator->next;
  }
}

void GApplicationHandler::onBusAcquired(GDBusConnection *conn, const gchar *name, gpointer configs)
{
  activateApplication((Configs *)configs, conn);
}

GApplicationHandler::GApplicationHandler(Configs *configs, std::string instance)
{
  gtk_init();
  this->configs = configs;

  configs->instance = instance;

  busName = std::string("com.r00n1l." + instance + "." + (configs->audioName));

  configs->windowHandler->gtkApplicationInstance = this;

  loop = g_main_loop_new(NULL, false);
}

void GApplicationHandler::runApp()
{
  g_bus_own_name(G_BUS_TYPE_SESSION, busName.c_str(),
                 G_BUS_NAME_OWNER_FLAGS_NONE, onBusAcquired, NULL, NULL, configs, NULL);
  g_main_loop_run(loop);
}

GApplicationHandler::~GApplicationHandler()
{
  g_main_loop_unref(loop);
  std::exit(0);
}