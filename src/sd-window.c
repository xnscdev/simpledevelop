/* sd-window.c -- This file is part of SimpleDevelop.
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
#include "sd-editor.h"
#include "sd-project-tree.h"

struct _SDWindowPrivate
{
  GtkHeaderBar *header;
  GtkMenuItem *preferences_item;
  GtkWidget *tree_window;
  GtkWidget *editor_view;
  SDEditor *editor;
  gchar *title;
};

typedef struct _SDWindowPrivate SDWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (SDWindow, sd_window, GTK_TYPE_WINDOW)

static void
sd_window_init (SDWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

static void
sd_window_class_init (SDWindowClass *klass)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass),
					       SD_RESOURCE_WINDOW_UI);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
						SDWindow, header);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
						SDWindow, preferences_item);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
						SDWindow, tree_window);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
						SDWindow, editor_view);
}

SDWindow *
sd_window_new (SDApplication *app)
{
  return g_object_new (SD_TYPE_WINDOW, "application", app, NULL);
}

void
sd_window_open (SDWindow *window, GFile *file)
{
  SDWindowPrivate *priv = sd_window_get_instance_private (window);
  SDProjectTree *tree = sd_project_tree_new (window, file);
  gchar *basename;

  g_return_if_fail (tree != NULL);
  gtk_container_add (GTK_CONTAINER (priv->tree_window), GTK_WIDGET (tree));
  gtk_widget_show_all (priv->tree_window);

  priv->editor = sd_editor_new (window);
  gtk_container_add (GTK_CONTAINER (priv->editor_view),
		     GTK_WIDGET (priv->editor));
  gtk_widget_show_all (priv->editor_view);

  basename = g_file_get_basename (file);
  priv->title = g_strdup_printf ("SimpleDevelop - %s", basename);
  g_free (basename);
  basename = g_strdup_printf ("%s - Startup", priv->title);
  gtk_header_bar_set_title (priv->header, basename);
  g_free (basename);

  g_signal_connect (priv->preferences_item, "activate",
		    G_CALLBACK (sd_preferences_activate), window);
}

void
sd_window_editor_open (SDWindow *self, const gchar *filename, GFile *file)
{
  SDWindowPrivate *priv = sd_window_get_instance_private (self);
  sd_editor_open_tab (priv->editor, filename, file);
}

void
sd_window_update_title (SDWindow *self, const gchar *name)
{
  SDWindowPrivate *priv = sd_window_get_instance_private (self);
  gchar *title = g_strdup_printf ("%s - %s", priv->title, name);
  gtk_header_bar_set_title (priv->header, title);
  g_free (title);
}
