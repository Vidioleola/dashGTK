#include "gisagauge.h"
#include <gtk-3.0/gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_W 800
#define WINDOW_H 480

// called when window is closed
void on_window_main_destroy() { gtk_main_quit(); }

int main(int argc, char **argv)
{

  GtkWidget *window;

  gtk_init(&argc, &argv);
  GError *err = NULL;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_W, WINDOW_H);
  gtk_window_resize(GTK_WINDOW(window), WINDOW_W, WINDOW_H);
  // gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
  gtk_window_set_title(GTK_WINDOW(window), "DASH FormUL 2021");
  g_signal_connect(window, "destroy", G_CALLBACK(on_window_main_destroy), NULL);

  GdkRGBA color;

  gdk_rgba_parse(&color, "rgba(100,100,100,255)");

  gtk_widget_override_background_color(GTK_WIDGET(window), GTK_STATE_NORMAL, &color);

  GtkWidget *mainGrid = gtk_grid_new();
  gtk_grid_set_column_homogeneous(GTK_GRID(mainGrid), TRUE);
  gtk_grid_set_row_homogeneous(GTK_GRID(mainGrid), TRUE);

  GtkWidget *gauge = gisa_gauge_new();
  gisa_gauge_set_max_value(GISA_GAUGE(gauge), 15000);
  gisa_gauge_set_min_value(GISA_GAUGE(gauge), 0);
  gisa_gauge_set_value(GISA_GAUGE(gauge), 14600);
  gtk_widget_set_hexpand(gauge, TRUE);
  gtk_widget_set_vexpand(gauge, TRUE);
  gtk_grid_attach(GTK_GRID(mainGrid), GTK_WIDGET(gauge), 1, 0, 2, 2);

  GtkWidget *label_Messages = gtk_label_new_with_mnemonic("Messages");
  gtk_grid_attach(GTK_GRID(mainGrid), label_Messages, 0, 0, 1, 1);

  GtkWidget *label_Time = gtk_label_new_with_mnemonic("Time");
  gtk_grid_attach(GTK_GRID(mainGrid), label_Time, 3, 0, 1, 1);

  GtkWidget *label_Voltage = gtk_label_new_with_mnemonic("Voltage");
  gtk_grid_attach(GTK_GRID(mainGrid), label_Voltage, 1, 2, 1, 1);

  GtkWidget *label_Temperature = gtk_label_new_with_mnemonic("Temperature");
  gtk_grid_attach(GTK_GRID(mainGrid), label_Temperature, 2, 2, 1, 1);

  GtkWidget *label_Voltage_Value = gtk_label_new_with_mnemonic("----");
  gtk_grid_attach(GTK_GRID(mainGrid), label_Voltage_Value, 1, 3, 1, 1);

  GtkWidget *label_Temperature_Value = gtk_label_new_with_mnemonic("----");
  gtk_grid_attach(GTK_GRID(mainGrid), label_Temperature_Value, 2, 3, 1, 1);

  GtkWidget *label_AMS_Errors = gtk_label_new_with_mnemonic("AMS Errors");
  gtk_grid_attach(GTK_GRID(mainGrid), label_AMS_Errors, 0, 2, 1, 1);

  GtkWidget *label_AMS_Errors_Value = gtk_label_new_with_mnemonic("---------");
  gtk_grid_attach(GTK_GRID(mainGrid), label_AMS_Errors_Value, 0, 3, 1, 1);

  GtkWidget *label_Inverters_Errors =
      gtk_label_new_with_mnemonic("Inverters Errors");
  gtk_grid_attach(GTK_GRID(mainGrid), label_Inverters_Errors, 3, 2, 1, 1);

  GtkWidget *label_Inverters_Errors_Value =
      gtk_label_new_with_mnemonic("---------");
  gtk_grid_attach(GTK_GRID(mainGrid), label_Inverters_Errors_Value, 3, 3, 1, 1);

  GtkWidget *empty_0 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_grid_attach(GTK_GRID(mainGrid), empty_0, 0, 1, 1, 1);
  GtkWidget *empty_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_grid_attach(GTK_GRID(mainGrid), empty_1, 3, 1, 1, 1);

  gtk_container_add(GTK_CONTAINER(window), mainGrid);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
