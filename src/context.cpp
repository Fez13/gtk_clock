#include "context.h"
#include "config.h"
#include <cstdlib>
#include <string>

void Context::init() {
  auto [err_c, config_c] =
      Configuration::get().get_config("configuration/config");

  // Initalize Freetype
  auto error = FT_Init_FreeType(&font.library);
  if (error) {
    printf("Error creating freetype.\n");
    exit(1);
  }

  #ifdef WINDOWS_BUILD
  const std::string configPath = "$HOME/AppData/Local/gtk_clock/";
  #else
  const char* homeDir = getenv("HOME");
  const std::string configPath = std::string(homeDir) + "/.config/gtk_clock/";
  #endif

  std::string font_path =
      "../fonts/Open_Sans/OpenSans-VariableFont_wdth,wght.ttf";
  if (!err_c) {
    font_path = configPath + std::string(config_c->at("font_path"));
  }

  // Load font
  error = FT_New_Face(font.library,
                      font_path.c_str(),
                      0, &font.face);
  if (error) {
    printf("Error loading font. Path: %s \n", font_path.c_str());
    exit(1);
  }

  // Create cairo font
  font.cairo_face = cairo_ft_font_face_create_for_ft_face(font.face, 0);

  gtk_init(nullptr, nullptr);
}
