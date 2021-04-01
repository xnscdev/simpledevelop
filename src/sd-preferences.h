/* sd-preferences.h -- This file is part of SimpleDevelop.
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

#ifndef _SD_PREFERENCES_H
#define _SD_PREFERENCES_H

#include "sd-window.h"

G_BEGIN_DECLS

#define SD_TYPE_PREFERENCES sd_preferences_get_type ()
G_DECLARE_FINAL_TYPE (SDPreferences, sd_preferences, SD, PREFERENCES, GtkDialog)

struct _SDPreferences
{
  GtkDialog parent;
};

SDPreferences *sd_preferences_new (SDWindow *window);
void sd_preferences_activate (GtkMenuItem *item, gpointer user_data);

G_END_DECLS

#endif
