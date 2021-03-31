/* sd-application.c -- This file is part of SimpleDevelop.
   Copyright (C) 2021 XNSC

   SimpleDevelop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   SimpleDevelop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with SimpleDevelop. If not, see <https://www.gnu.org/licenses/>. */

#include "sd-window.h"

G_DEFINE_TYPE (SDApplication, sd_application, GTK_TYPE_APPLICATION)

static void
sd_application_init (SDApplication *self)
{
}

static void
sd_application_activate (GApplication *app)
{
}

static void
sd_application_open_window (gpointer data, gpointer user_data)
{
  gtk_window_present (GTK_WINDOW (data));
}

static void
sd_application_open (GApplication *app, GFile **files, gint nfiles,
		     const gchar *hint)
{
  GList *windows = gtk_application_get_windows (GTK_APPLICATION (app));
  gint i;
  g_list_foreach (windows, sd_application_open_window, NULL);
  for (i = 0; i < nfiles; i++)
    {
      SDWindow *window =
	sd_window_new (SD_APPLICATION (app));
      sd_window_open (window, files[i]);
      gtk_window_present (GTK_WINDOW (window));
    }
}

static void
sd_application_class_init (SDApplicationClass *klass)
{
  G_APPLICATION_CLASS (klass)->activate = sd_application_activate;
  G_APPLICATION_CLASS (klass)->open = sd_application_open;
}

SDApplication *
sd_application_new (void)
{
  return g_object_new (SD_TYPE_APPLICATION, "application-id", SD_APPLICATION_ID,
		       "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
