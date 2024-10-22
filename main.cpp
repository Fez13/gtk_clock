
#include "src/config.h"
#include "src/context.h"
#include "src/gui.h"
#include <cstdlib>




int main() {

  #ifdef WINDOWS_BUILD
  const std::string configPath = "$HOME/AppData/Local/gtk_clock/resources";
  #else
  const char* homeDir = getenv("HOME");
  const std::string configPath = std::string(homeDir) + "/.config/gtk_clock/resources";
  #endif

  Configuration::get().initialize(configPath);
  Context *ctx = new Context;

  ctx->init();

  ClockGui gui(ctx);

  gui.loop();
}
