/* sd-application-window.c -- This file is part of SimpleDevelop.
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

#include "sd-application-window.h"

G_DEFINE_TYPE (SDApplicationWindow, sd_application_window,
	       GTK_TYPE_APPLICATION_WINDOW)

static void
sd_application_window_init (SDApplicationWindow *self)
{
}

static void
sd_application_window_class_init (SDApplicationWindowClass *klass)
{
}

SDApplicationWindow *
sd_application_window_new (SDApplication *app)
{
  return g_object_new (SD_TYPE_APPLICATION_WINDOW, "application", app, NULL);
}

void
sd_application_window_open (SDApplicationWindow *window, GFile *file)
{
}
