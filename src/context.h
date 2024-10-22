#pragma once
#pragma once

#include "cairo.h"

#include <cstdio>
#include <ctime>
#include <freetype/freetype.h>
#include <gtkmm/application.h>

struct Context{
  public:
    void init();
  struct {
    FT_Library library;
    FT_Face face;
    cairo_font_face_t *cairo_face;
  } font;
};
