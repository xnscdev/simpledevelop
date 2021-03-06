/* sd-application.h -- This file is part of SimpleDevelop.
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

#ifndef _SD_APPLICATION_H
#define _SD_APPLICATION_H

#include <gtk/gtk.h>

#define SD_APPLICATION_ID "org.xnsc.simpledevelop"
#define SD_SETTINGS_NAME SD_APPLICATION_ID

#define SD_RESOURCE_WINDOW_UI "/org/xnsc/simpledevelop/window.glade"
#define SD_RESOURCE_PREFERENCES_UI "/org/xnsc/simpledevelop/preferences.ui"

G_BEGIN_DECLS

#define SD_TYPE_APPLICATION sd_application_get_type ()
G_DECLARE_FINAL_TYPE (SDApplication, sd_application, SD, APPLICATION,
		      GtkApplication)

struct _SDApplication
{
  GtkApplication parent;
};

SDApplication *sd_application_new (void);

G_END_DECLS

#endif
