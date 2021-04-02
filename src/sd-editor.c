/* sd-editor.c -- This file is part of SimpleDevelop.
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
#include "sd-editor.h"

struct _SDEditorTabData
{
  GtkNotebook *nb;
  gint page;
};

typedef struct _SDEditorTabData SDEditorTabData;

struct _SDEditorPrivate
{
  GSettings *settings;
};

typedef struct _SDEditorPrivate SDEditorPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (SDEditor, sd_editor, GTK_TYPE_NOTEBOOK)

static void
sd_editor_init (SDEditor *self)
{
  SDEditorPrivate *priv = sd_editor_get_instance_private (self);
  priv->settings = g_settings_new (SD_SETTINGS_NAME);
}

static void
sd_editor_class_init (SDEditorClass *klass)
{
}

static GtkSourceLanguage *
sd_editor_guess_lang (const gchar *filename, const gchar *contents, gsize len)
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

static gboolean
sd_editor_close_tab (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  SDEditorTabData *data = user_data;
  g_debug ("Closing editor tab %d", data->page);
  gtk_notebook_remove_page (data->nb, data->page);
  return TRUE;
}

static void
sd_editor_switch_page (GtkNotebook *nb, GtkWidget *page, guint pnum,
		       gpointer user_data)
{
  SDWindow *window = SD_WINDOW (user_data);
  GtkWidget *child = gtk_notebook_get_nth_page (nb, pnum);
  GtkWidget *tab = gtk_notebook_get_tab_label (nb, child);
  GList *list = gtk_container_get_children (GTK_CONTAINER (tab));
  const gchar *name = NULL;
  GList *ptr;

  for (ptr = list; ptr != NULL; ptr = g_list_next (ptr))
    {
      if (GTK_IS_LABEL (ptr->data))
	{
	  name = gtk_label_get_text (GTK_LABEL (ptr->data));
	  break;
	}
    }
  g_list_free (list);

  g_return_if_fail (name != NULL);
  sd_window_update_title (window, name);
}

SDEditor *
sd_editor_new (SDWindow *window)
{
  SDEditor *editor = g_object_new (SD_TYPE_EDITOR, NULL);
  g_signal_connect (editor, "switch-page", G_CALLBACK (sd_editor_switch_page),
		    window);
  return editor;
}

void
sd_editor_open_tab (SDEditor *self, const gchar *filename,
		    const gchar *contents, gsize len)
{
  SDEditorPrivate *priv = sd_editor_get_instance_private (self);
  GtkSourceLanguage *lang;
  GtkSourceBuffer *buffer;
  GtkSourceView *view;
  GtkWidget *window;
  GtkWidget *tab;
  GtkWidget *event_box;
  GtkWidget *close_button;
  GtkTextTag *tag;
  GtkTextIter start;
  GtkTextIter end;
  SDEditorTabData *user_data;
  gint page;

  /* Create new editor view */
  view = GTK_SOURCE_VIEW (gtk_source_view_new ());
  g_settings_bind (priv->settings, "line-numbers", view,
		   "show-line-numbers", G_SETTINGS_BIND_DEFAULT);

  buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER (buffer), contents, len);

  tag = gtk_text_buffer_create_tag (GTK_TEXT_BUFFER (buffer), NULL, NULL);
  g_settings_bind (priv->settings, "font", tag, "font",
		   G_SETTINGS_BIND_DEFAULT);
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (buffer), &start);
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (buffer), &end);
  gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER (buffer), tag, &start, &end);

  /* Apply syntax highlighting to buffer */
  lang = sd_editor_guess_lang (filename, contents, len);
  if (lang == NULL)
    g_debug ("Failed to guess language, applying default highlighting");
  else
    {
      g_debug ("Guessed language as %s", gtk_source_language_get_name (lang));
      gtk_source_buffer_set_language (buffer, lang);
    }

  /* Add view to notebook */
  tab = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  event_box = gtk_event_box_new ();
  close_button = gtk_image_new_from_icon_name ("application-exit",
					       GTK_ICON_SIZE_BUTTON);
  window = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (event_box), close_button);
  gtk_container_add (GTK_CONTAINER (tab), event_box);
  gtk_container_add (GTK_CONTAINER (tab), gtk_label_new (filename));
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (view));
  page = gtk_notebook_append_page (GTK_NOTEBOOK (self), window, tab);

  user_data = g_malloc (sizeof (SDEditorTabData));
  user_data->nb = GTK_NOTEBOOK (self);
  user_data->page = page;
  g_signal_connect (event_box, "button-release-event",
		    G_CALLBACK (sd_editor_close_tab), user_data);

  gtk_widget_show_all (tab);
  gtk_widget_show_all (GTK_WIDGET (self));
}
