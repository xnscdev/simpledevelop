/* sd-project-tree.c -- This file is part of SimpleDevelop.
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

#include "sd-project-tree.h"

struct _SDProjectTreePrivate
{
  GtkTreeStore *store;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *col;
};

typedef struct _SDProjectTreePrivate SDProjectTreePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (SDProjectTree, sd_project_tree, GTK_TYPE_TREE_VIEW)

static void
sd_project_tree_populate (GtkTreeStore *store, GtkTreeIter *parent, GFile *file)
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
      GFile *subfile;
      const gchar *dispname;
      if (!g_file_enumerator_iterate (en, &info, NULL, NULL, &err))
	goto finish;
      if (info == NULL)
	break;

      dispname = g_file_info_get_display_name (info);
      gtk_tree_store_append (store, &child, parent);
      subfile = g_file_get_child (file, g_file_info_get_name (info));
      gtk_tree_store_set (store, &child, NAME_COLUMN, dispname, FG_COLUMN,
			  *dispname == '.' ? "LightSlateGray" : "Black",
			  FILE_COLUMN, subfile, -1);

      if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY)
        sd_project_tree_populate (store, &child, subfile);
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

static void
sd_project_tree_activated (GtkTreeView *view, GtkTreePath *path,
			   GtkTreeViewColumn *col, gpointer user_data)
{
  GtkTreeModel *model = gtk_tree_view_get_model (view);
  SDWindow *window = SD_WINDOW (user_data);
  GError *err = NULL;
  GFile *file;
  GtkTreeIter iter;
  gchar *contents;
  gchar *name;
  gsize len;

  g_return_if_fail (gtk_tree_model_get_iter (model, &iter, path));
  gtk_tree_model_get (model, &iter, NAME_COLUMN, &name, FILE_COLUMN, &file, -1);
  if (g_file_query_file_type (file, G_FILE_QUERY_INFO_NONE, NULL) !=
      G_FILE_TYPE_REGULAR)
    return;

  g_file_load_contents (file, NULL, &contents, &len, NULL, &err);
  if (err != NULL)
    {
      g_critical ("Failed to read file contents: %s", err->message);
      g_error_free (err);
      return;
    }

  /* Don't open binary files */
  g_return_if_fail (g_utf8_validate (contents, len, NULL));
  sd_window_editor_open (window, name, contents, len);
  g_free (name);
  g_free (contents);
}

static void
sd_project_tree_init (SDProjectTree *self)
{
  SDProjectTreePrivate *priv = sd_project_tree_get_instance_private (self);
  priv->store = gtk_tree_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING,
				    G_TYPE_FILE);
  priv->renderer = gtk_cell_renderer_text_new ();
  /* Causes warning because `file' is not an attribute of GtkCellRenderer
     but seems to work anyway */
  priv->col =
    gtk_tree_view_column_new_with_attributes ("Project Tree", priv->renderer,
					      "text", NAME_COLUMN,
					      "foreground", FG_COLUMN,
					      "file", FILE_COLUMN, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (self), priv->col);
  gtk_tree_view_set_model (GTK_TREE_VIEW (self), GTK_TREE_MODEL (priv->store));
}

static void
sd_project_tree_class_init (SDProjectTreeClass *klass)
{
}

SDProjectTree *
sd_project_tree_new (SDWindow *window, GFile *file)
{
  SDProjectTree *tree;
  SDProjectTreePrivate *priv;
  GtkTreeIter parent;
  GtkTreePath *path;
  GError *err = NULL;
  GFileInfo *info =
    g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
		       G_FILE_QUERY_INFO_NONE, NULL, &err);
  if (err != NULL)
    {
      gchar *str = g_file_get_path (file);
      g_critical ("Failed to get info for %s: %s", str, err->message);
      g_free (str);
      g_error_free (err);
      return NULL;
    }

  tree = g_object_new (SD_TYPE_PROJECT_TREE, NULL);
  priv = sd_project_tree_get_instance_private (tree);

  gtk_tree_store_append (priv->store, &parent, NULL);
  gtk_tree_store_set (priv->store, &parent, NAME_COLUMN,
		      g_file_info_get_display_name (info),
		      FG_COLUMN, "Black", FILE_COLUMN, file, -1);
  g_object_unref (info);
  gtk_tree_model_get_iter_first (GTK_TREE_MODEL (priv->store), &parent);
  sd_project_tree_populate (priv->store, &parent, file);

  path = gtk_tree_path_new_first ();
  gtk_tree_view_expand_row (GTK_TREE_VIEW (tree), path, FALSE);
  gtk_tree_path_free (path);

  g_signal_connect (tree, "row-activated",
		    G_CALLBACK (sd_project_tree_activated), window);
  return tree;
}
