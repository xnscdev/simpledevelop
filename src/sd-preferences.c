/* sd-preferences.c -- This file is part of SimpleDevelop.
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

#include "sd-preferences.h"

struct _SDPreferencesPrivate
{
  GSettings *settings;
  GtkWidget *linenos;
  GtkWidget *font;
};

typedef struct _SDPreferencesPrivate SDPreferencesPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (SDPreferences, sd_preferences, GTK_TYPE_DIALOG)

static void
sd_preferences_dispose (GObject *obj)
{
  SDPreferencesPrivate *priv =
    sd_preferences_get_instance_private (SD_PREFERENCES (obj));
  g_clear_object (&priv->settings);
  G_OBJECT_CLASS (sd_preferences_parent_class)->dispose (obj);
}

static void
sd_preferences_init (SDPreferences *self)
{
  SDPreferencesPrivate *priv = sd_preferences_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));
  priv->settings = g_settings_new (SD_SETTINGS_NAME);
  g_settings_bind (priv->settings, "line-numbers", priv->linenos, "active",
		   G_SETTINGS_BIND_DEFAULT);
  g_settings_bind (priv->settings, "font", priv->font, "font",
		   G_SETTINGS_BIND_DEFAULT);
}

static void
sd_preferences_class_init (SDPreferencesClass *klass)
{
  G_OBJECT_CLASS (klass)->dispose = sd_preferences_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass),
					       SD_RESOURCE_PREFERENCES_UI);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
						SDPreferences, linenos);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
						SDPreferences, font);
}

SDPreferences *
sd_preferences_new (SDWindow *window)
{
  return g_object_new (SD_TYPE_PREFERENCES, "transient-for", window,
		       "use-header-bar", TRUE, NULL);
}

void
sd_preferences_activate (GtkMenuItem *item, gpointer user_data)
{
  SDPreferences *prefs = sd_preferences_new (SD_WINDOW (user_data));
  gtk_window_present (GTK_WINDOW (prefs));
}
