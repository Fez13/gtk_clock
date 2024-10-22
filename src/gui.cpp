#include "gui.h"
#include "config.h"
#include "glib.h"
#include "gtk/gtk.h"
#include <algorithm>

static void on_window_destroy(GtkWindow *_window, void *_data) {
  gtk_main_quit();
}

static gboolean gui_update(GtkWidget *widget) {
  // Check for updates in config file
  // TODO USE: https://github.com/ThomasMonkman/filewatch
  Configuration::get().reload("configuration/config");
  gtk_widget_queue_draw(widget);
  return TRUE;
}

static gboolean gui_draw(GtkWidget *widget, cairo_t *cr, gpointer clock_p) {
  auto clock = static_cast<ClockGui *>(clock_p);
  auto [err_c, config_c] =
      Configuration::get().get_config("configuration/config");

  std::time_t t = std::time(0); // get time now
  std::tm *now = std::localtime(&t);

  // get text
  bool show_seconds = false;
  if (!err_c) {
    show_seconds = config_c->at("show_seconds");
  }
  std::string text_time = "";

  if (!show_seconds) {
    text_time =
        std::format("{}:{}",
                    (now->tm_hour < 10) ? "0" + std::to_string(now->tm_hour)
                                        : std::to_string(now->tm_hour),
                    (now->tm_min < 10) ? "0" + std::to_string(now->tm_min)
                                       : std::to_string(now->tm_min));
  } else {
    text_time =
        std::format("{}:{}:{}",
                    (now->tm_hour < 10) ? "0" + std::to_string(now->tm_hour)
                                        : std::to_string(now->tm_hour),
                    (now->tm_min < 10) ? "0" + std::to_string(now->tm_min)
                                       : std::to_string(now->tm_min),
                    (now->tm_sec < 10) ? "0" + std::to_string(now->tm_sec)
                                       : std::to_string(now->tm_sec));
  }
  const std::string text_date = std::format(
      "{}:{}:{}",
      (now->tm_year - 100 < 10) ? "0" + std::to_string(now->tm_year - 100)
                                : std::to_string(now->tm_year - 100),
      (now->tm_mon + 1 < 10) ? "0" + std::to_string(now->tm_mon + 1)
                             : std::to_string(now->tm_mon + 1),
      (now->tm_mday < 10) ? "0" + std::to_string(now->tm_mday)
                          : std::to_string(now->tm_mday));
  // Set clock position
  gint x = int(config_c->at("display_size_x")) / 2;
  gint y = int(config_c->at("display_size_x")) / 2;
  if (!err_c) {
    x = config_c->at("clock_position").at("x");
    y = config_c->at("clock_position").at("y");
  }

  // This does not work
  // gtk_window_move(GTK_WINDOW(clock->get_components().window), 100, 200 );
  // What will be done: The position will be set to the left corner and the size
  // to the whole screen then the clock will move inside the container.
  GdkRGBA color = {1, 1, 1, 1};

  if (!err_c) {
    color = {config_c->at("clock_color").at("r"),
             config_c->at("clock_color").at("g"),
             config_c->at("clock_color").at("b"),
             config_c->at("clock_color").at("a")};
  }
  gdk_cairo_set_source_rgba(cr, &color);
  cairo_set_font_face(cr, clock->get_config()->font.cairo_face);

  // Font size
  double clock_size = 42;
  if (!err_c)
    clock_size = config_c->at("clock_font_size");

  double date_size = 20;
  if (!err_c)
    date_size = config_c->at("date_font_size");

  // Clock time
  // Check if extents are correct
  cairo_text_extents_t current_clock_text_extent;
  cairo_text_extents_t current_date_text_extent;
  cairo_set_font_size(cr, date_size);
  cairo_text_extents(cr, text_date.c_str(), &current_date_text_extent);

  cairo_set_font_size(cr, clock_size);
  cairo_text_extents(cr, text_time.c_str(), &current_clock_text_extent);

  double max_h = std::max(current_clock_text_extent.height,
                          current_date_text_extent.height);
  double max_w =
      std::max(current_clock_text_extent.width, current_date_text_extent.width);

  double max_bearing_x = std::max(current_clock_text_extent.x_bearing,
                                  current_date_text_extent.x_bearing);

  if (clock->get_extent_h() != max_h || clock->get_extent_w() != max_w) {
    clock->get_extent_h() = max_h;
    clock->get_extent_w() = max_h;
    gtk_widget_set_size_request((clock->get_components().canvas), max_bearing_x,
                                max_h);
    /*

    gtk_widget_set_size_request((clock->get_components().window),
                                current_text_extent.width +
                                    current_text_extent.x_bearing,
                                current_text_extent.height);
                                */
    // Window positioning error walk around:
    gtk_widget_set_size_request((clock->get_components().window),
                                config_c->at("display_size_x"),
                                config_c->at("display_size_y"));
  }

  cairo_set_font_size(cr, clock_size);
  cairo_move_to(cr, x - (current_clock_text_extent.x_advance / 2),
                y - (current_clock_text_extent.y_advance / 2));
  cairo_show_text(cr, text_time.c_str());
  cairo_stroke(cr);

  // Clock date

  cairo_set_font_size(cr, date_size);
  double spacing = 1.618;
  if (!err_c)
    spacing = config_c->at("spacing");
  cairo_move_to(cr, x - (current_date_text_extent.x_advance / 2),
                y - (current_date_text_extent.y_advance / 2) +
                    current_date_text_extent.height + spacing);
  cairo_show_text(cr, text_date.c_str());
  cairo_stroke(cr);

  return FALSE;
}

ClockGui::ClockGui(Context *ctx) : m_context(ctx) {
  m_components.window = (gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_window_set_position(GTK_WINDOW(m_components.window),
                          GTK_WIN_POS_CENTER_ALWAYS);

  gtk_layer_init_for_window(GTK_WINDOW(m_components.window));

  gtk_layer_set_layer(GTK_WINDOW(m_components.window),
                      GTK_LAYER_SHELL_LAYER_BACKGROUND);
  gtk_layer_set_monitor(
      GTK_WINDOW(m_components.window),
      gdk_display_get_primary_monitor(gdk_display_get_default()));

  // Set transparent
  auto color = (GdkRGBA){1.0, 0.0, 0.0, 0.0};
  gtk_widget_override_background_color(m_components.window,
                                       GTK_STATE_FLAG_NORMAL,
                                       &color); // Set red color

  // Drawing area
  m_components.canvas = gtk_drawing_area_new();
  gtk_widget_set_size_request((m_components.canvas), 0, 0);

  g_signal_connect(m_components.canvas, "draw", G_CALLBACK(gui_draw), this);
  g_timeout_add_seconds(1, G_SOURCE_FUNC(gui_update), m_components.canvas);
  gtk_container_add(GTK_CONTAINER(m_components.window), m_components.canvas);
  g_signal_connect(m_components.window, "destroy",
                   G_CALLBACK(on_window_destroy), this);
  gtk_widget_show_all(GTK_WIDGET(m_components.window));
}

void ClockGui::loop() { gtk_main(); }
