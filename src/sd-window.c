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

#include "project-tree.h"
#include "sd-window.h"

struct _SDWindowPrivate
{
  GFile *project_dir;
  GtkWidget *project_tree;
  GtkViewport *editor_viewport;
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
						SDWindow, project_tree);
  gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (klass),
						SDWindow, editor_viewport);
}

static gboolean
sd_window_build_project_tree (SDWindow *window, GFile *file)
{
  SDWindowPrivate *priv = sd_window_get_instance_private (window);
  GtkTreeView *view = GTK_TREE_VIEW (priv->project_tree);
  GtkTreeStore *store = GTK_TREE_STORE (gtk_tree_view_get_model (view));
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *col;
  GtkTreePath *path;
  GtkTreeIter parent;
  GError *err = NULL;
  GFileInfo *info =
    g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
		       G_FILE_QUERY_INFO_NONE, NULL, &err);
  if (err != NULL)
    {
      gchar *path = g_file_get_path (file);
      g_critical ("Failed to get info for %s: %s", path, err->message);
      g_free (path);
      g_error_free (err);
      return FALSE;
    }

  renderer = gtk_cell_renderer_text_new ();
  col = gtk_tree_view_column_new_with_attributes ("Project Tree", renderer,
						  "text", NAME_COLUMN,
						  "foreground", FG_COLUMN,
						  NULL);
  gtk_tree_view_append_column (view, col);

  priv->project_dir = file;
  g_object_ref (priv->project_dir);
  gtk_tree_store_append (store, &parent, NULL);
  gtk_tree_store_set (store, &parent, NAME_COLUMN,
		      g_file_info_get_display_name (info),
		      FG_COLUMN, "Black", -1);
  g_object_unref (info);
  gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &parent);
  sd_project_tree_populate (store, &parent, file);

  path = gtk_tree_path_new_first ();
  gtk_tree_view_expand_row (view, path, FALSE);
  gtk_tree_path_free (path);
  return TRUE;
}

SDWindow *
sd_window_new (SDApplication *app)
{
  return g_object_new (SD_TYPE_WINDOW, "application", app, NULL);
}

void
sd_window_open (SDWindow *window, GFile *file)
{
  SDWindowPrivate *priv;
  GtkWidget *label;
  g_return_if_fail (sd_window_build_project_tree (window, file));
  priv = sd_window_get_instance_private (window);

  priv->project_dir = file;
  g_object_ref (priv->project_dir);

  /* Default label */
  label = gtk_label_new ("Open a file in the project tree to open it here");
  gtk_container_add (GTK_CONTAINER (priv->editor_viewport), label);
  gtk_widget_show_all (GTK_WIDGET (priv->editor_viewport));

  g_signal_connect (GTK_TREE_VIEW (priv->project_tree), "row-activated",
		    G_CALLBACK (sd_project_tree_activated), priv->project_dir);
}
