#pragma once
#include "cairo.h"
#include "context.h"
#include "gtk-layer-shell.h"
#include "gtk/gtk.h"
#include "gtkmm/widget.h"

struct Components{
  GtkWidget *window;
  GtkWidget *canvas;
} ;

class ClockGui {
public:
  ClockGui(Context *context);
  void loop();

  Context *get_config() { return m_context; }
  double& get_extent_h() {return m_text.max_h;}
  double& get_extent_w() {return m_text.max_w;}
  Components &get_components() {return m_components;}
private:
  Context *m_context = nullptr;
  struct {
    double max_h;
    double max_w;
  } m_text;
  Components m_components;
};
