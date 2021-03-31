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

#include "callbacks.h"
#include "sd-window.h"

enum
{
  NAME_COLUMN = 0,
  FG_COLUMN,
  N_COLUMNS
};

struct _SDWindowPrivate
{
  GtkWidget *project_tree;
  GtkWidget *editor_view;
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
						SDWindow, editor_view);
}

static void
sd_window_populate_project_tree (GtkTreeStore *store, GtkTreeIter *parent,
				 GFile *file, guint level)
{
  GError *err = NULL;
  GtkTreeIter child;
  GFileEnumerator *en =
    g_file_enumerate_children (file, G_FILE_ATTRIBUTE_STANDARD_NAME ","
			       G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
			       G_FILE_QUERY_INFO_NONE, NULL, &err);
  if (err != NULL)
    {
      gchar *path = g_file_get_path (file);
      if (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY))
	g_warning ("%s is not a directory, skipping", path);
      else
	g_critical ("Failed to read contents of %s: %s", path, err->message);
      g_free (path);
      g_error_free (err);
      return;
    }

  while (TRUE)
    {
      GFileInfo *info;
      GFile *subdir;
      const gchar *dispname;
      if (!g_file_enumerator_iterate (en, &info, NULL, NULL, &err))
	goto finish;
      if (info == NULL)
	break;

      dispname = g_file_info_get_display_name (info);
      gtk_tree_store_append (store, &child, parent);
      gtk_tree_store_set (store, &child, NAME_COLUMN, dispname, FG_COLUMN,
			  "Black", -1);

      if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY)
        {
	  if (level > 2)
	    {
	      GtkTreeIter subchild;
	      gtk_tree_store_append (store, &subchild, &child);
	      gtk_tree_store_set (store, &subchild, NAME_COLUMN, "Loading...",
				  FG_COLUMN, "LightSlateGray", -1);
	    }
	  else
	    {
	      subdir = g_file_get_child (file, g_file_info_get_name (info));
	      sd_window_populate_project_tree (store, &child, subdir,
					       level + 1);
	    }
	}
    }

 finish:
  if (err != NULL)
    {
      gchar *path = g_file_get_path (file);
      g_critical ("Failed to read contents of %s: %s", path, err->message);
      g_free (path);
      g_error_free (err);
    }
  g_object_unref (en);
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
      return;
    }

  renderer = gtk_cell_renderer_text_new ();
  col = gtk_tree_view_column_new_with_attributes ("Project Tree", renderer,
						  "text", NAME_COLUMN,
						  "foreground", FG_COLUMN,
						  NULL);
  gtk_tree_view_append_column (view, col);

  gtk_tree_store_append (store, &parent, NULL);
  gtk_tree_store_set (store, &parent, NAME_COLUMN,
		      g_file_info_get_display_name (info),
		      FG_COLUMN, "Black", -1);
  g_object_unref (info);
  gtk_tree_model_get_iter_first (GTK_TREE_MODEL (store), &parent);
  sd_window_populate_project_tree (store, &parent, file, 0);

  path = gtk_tree_path_new_first ();
  gtk_tree_view_expand_row (view, path, FALSE);
  gtk_tree_path_free (path);

  g_signal_connect (G_OBJECT (view), "row-expanded",
  		    G_CALLBACK (sd_project_tree_expand), NULL);
}
