/* sd-window.h -- This file is part of SimpleDevelop.
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

#ifndef _SD_WINDOW_H
#define _SD_WINDOW_H

#include "sd-application.h"

G_BEGIN_DECLS

#define SD_TYPE_WINDOW sd_window_get_type ()
G_DECLARE_FINAL_TYPE (SDWindow, sd_window, SD, WINDOW, GtkWindow)

struct _SDWindow
{
  GtkWindow parent;
};

SDWindow *sd_window_new (SDApplication *app);
void sd_window_open (SDWindow *window, GFile *file);
void sd_window_editor_clear (SDWindow *self);
void sd_window_editor_open (SDWindow *self, const gchar *filename,
			    const gchar *contents, gsize len);

G_END_DECLS

#endif
