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

#include <gtksourceview/gtksource.h>
#include "project-tree.h"
#include "sd-window.h"

struct _SDWindowPrivate
{
  GtkWidget *project_tree;
  GtkWidget *editor_window;
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
						SDWindow, editor_window);
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

  window->project_dir = file;
  g_object_ref (window->project_dir);
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

static void
sd_window_viewport_clear (SDWindowPrivate *priv)
{
  GList *child =
    gtk_container_get_children (GTK_CONTAINER (priv->editor_window));
  GList *iter;
  for (iter = child; iter != NULL; iter = g_list_next (iter))
    gtk_widget_destroy (GTK_WIDGET (iter->data));
  g_list_free (child);
}

static void
sd_window_editor_init (SDWindowPrivate *priv)
{
  sd_window_viewport_clear (priv);
  priv->editor_view = gtk_source_view_new ();

  gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW (priv->editor_view),
					 TRUE);
  gtk_text_view_set_monospace (GTK_TEXT_VIEW (priv->editor_view), TRUE);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (priv->editor_view), TRUE);
  gtk_widget_set_hexpand (priv->editor_view, TRUE);

  gtk_container_add (GTK_CONTAINER (priv->editor_window), priv->editor_view);
  gtk_widget_show_all (GTK_WIDGET (priv->editor_window));
}

static GtkSourceLanguage *
sd_window_guess_lang (const gchar *filename, const gchar *contents, gsize len)
{
  GtkSourceLanguageManager *mgr = gtk_source_language_manager_get_default ();
  GtkSourceLanguage *lang;
  gboolean uncertain;
  gchar *content_type;

  /* Try guessing using libgtksourceview */
  content_type = g_content_type_guess (filename, NULL, 0, &uncertain);
  if (uncertain)
    {
      g_free (content_type);
      content_type = NULL;
    }
  lang = gtk_source_language_manager_guess_language (mgr, filename,
						     content_type);
  g_free (content_type);
  if (lang != NULL)
    return lang;

  /* Try guessing using hash bang */
  if (len > 2 && contents[0] == '#' && contents[1] == '!')
    {
      GtkSourceLanguage *lang;
      const gchar *ptr = &contents[2];
      const gchar *end;
      gchar *path;
      gchar *basename;
      GFile *file;
      gsize pathlen = 0;

      if (*ptr == ' ' && len > 2)
	ptr++, len--;

      end = ptr;
      while (!g_ascii_isspace (*end) && len > 2)
	end++, pathlen++, len--;
      path = g_strndup (ptr, pathlen);
      file = g_file_new_for_path (path);
      g_free (path);
      basename = g_file_get_basename (file);
      g_object_unref (file);

      if (!g_strcmp0 (basename, "sh") || !g_strcmp0 (basename, "bash"))
	{
	  lang = gtk_source_language_manager_get_language (mgr, "sh");
	  goto hashbang_found;
	}
      if (!g_strcmp0 (basename, "perl"))
	{
	  lang = gtk_source_language_manager_get_language (mgr, "perl");
	  goto hashbang_found;
	}
      if (!g_strcmp0 (basename, "python"))
	{
	  lang = gtk_source_language_manager_get_language (mgr, "python");
	  goto hashbang_found;
	}
      if (!g_strcmp0 (basename, "python3"))
	{
	  lang = gtk_source_language_manager_get_language (mgr, "python3");
	  goto hashbang_found;
	}

      g_free (basename);
      goto finish;

    hashbang_found:
      g_free (basename);
      return lang;
    }

 finish:
  return NULL;
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
  g_return_if_fail (sd_window_build_project_tree (window, file));
  priv = sd_window_get_instance_private (window);

  window->project_dir = file;
  sd_window_editor_clear (window);

  g_signal_connect (priv->project_tree, "row-activated",
		    G_CALLBACK (sd_project_tree_activated), window);
}

void
sd_window_editor_clear (SDWindow *self)
{
  SDWindowPrivate *priv = sd_window_get_instance_private (self);
  GtkWidget *label =
    gtk_label_new ("Open a file in the project tree to open it here");
  sd_window_viewport_clear (priv);
  priv->editor_view = NULL;
  gtk_container_add (GTK_CONTAINER (priv->editor_window), label);
  gtk_widget_show_all (GTK_WIDGET (priv->editor_window));
}

void
sd_window_editor_open (SDWindow *self, const gchar *filename,
		       const gchar *contents, gsize len)
{
  SDWindowPrivate *priv = sd_window_get_instance_private (self);
  GtkSourceLanguage *lang;
  GtkSourceBuffer *buffer;
  GtkTextView *view;

  if (priv->editor_view == NULL)
    sd_window_editor_init (priv);

  view = GTK_TEXT_VIEW (priv->editor_view);
  buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (view));
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (buffer), contents, len);

  lang = sd_window_guess_lang (filename, contents, len);
  if (lang == NULL)
    g_debug ("Failed to guess language, applying default highlighting");
  else
    {
      g_debug ("Guessed language as %s", gtk_source_language_get_name (lang));
      gtk_source_buffer_set_language (buffer, lang);
    }
}
